#include "StudentHistoryStore.h"
#include "FileStorage.h"
#include "SafeFile.h"
#include <filesystem>
#include <set>
#include <stdexcept>
#include <fstream>

namespace
{
    // Recognise partial saves before accepting any dataset.
    bool checkFiles(const std::filesystem::path& folder)
    {
        if (std::filesystem::exists(folder) && !std::filesystem::is_directory(folder))
            throw std::runtime_error("Data directory is not a folder: " + folder.string());
        unsigned present = 0;
        bool hasBackup = false;
        for (const char* name : {"students.txt", "attendance.txt", "corrections.txt"})
        {
            const auto path = folder / name;
            if (std::filesystem::exists(path.string() + ".tmp"))
                throw std::runtime_error("Unfinished save: inspect .tmp and .bak files before starting.");
            if (std::filesystem::exists(path))
            {
                if (!std::filesystem::is_regular_file(path) || std::filesystem::is_symlink(path))
                    throw std::runtime_error("Data path must be a regular, unlinked file: " + path.string());
                ++present;
            }
            hasBackup = hasBackup || std::filesystem::exists(path.string() + ".bak");
        }
        if (present == 0 && hasBackup)
            throw std::runtime_error("Saved files are missing but backups exist. Recover the complete dataset.");
        if (present != 0 && present != 3)
            throw std::runtime_error("Incomplete dataset: students.txt, attendance.txt and corrections.txt are required together. Older history folders need a matching students.txt file.");

        for (const char* name : {"course_enrolments.txt", "dataset.txt"})
        {
            const auto path = folder / name;

            if (std::filesystem::exists(path.string() + ".tmp"))
                throw std::runtime_error("Unfinished registration save.");

            if (std::filesystem::exists(path))
            {
                if (present != 3 ||
                    !std::filesystem::is_regular_file(path) ||
                    std::filesystem::is_symlink(path))
                {
                    throw std::runtime_error(
                        "Invalid or incomplete registration dataset.");
                }
            }
            else if (std::filesystem::exists(path.string() + ".bak"))
            {
                throw std::runtime_error(
                    "Registration file missing but backup exists.");
            }
        }

        if (std::filesystem::exists(folder / "dataset.txt"))
        {
            std::ifstream marker(folder / "dataset.txt");
            std::string version;
            marker >> version >> std::ws;

            if (marker.bad() || !marker.eof() ||
                version != "DATASET_V2" ||
                !std::filesystem::exists(folder / "course_enrolments.txt"))
            {
                throw std::runtime_error(
                    "Invalid dataset marker or missing course file.");
            }
        }
        return present == 3;
    }
}

void StudentHistoryStore::validateLinks(
    const std::vector<Student>& users,
    const std::vector<AttendanceRecord>& attendance,
    const std::vector<CorrectionRecord>& notes)
{
    std::set<std::string> ids;
    for (const Student& student : users)
        ids.insert(student.getPersonId());
    for (const AttendanceRecord& record : attendance)
        if (ids.count(record.getStudentId()) == 0)
            throw std::runtime_error("Attendance references unknown student: " + record.getStudentId());
    for (const CorrectionRecord& note : notes)
        if (ids.count(note.getStudentId()) == 0)
            throw std::runtime_error("Correction references unknown student: " + note.getStudentId());
    // A correction may describe missing attendance, so no base-record match is required.
}

void StudentHistoryStore::load(const std::string& directory)
{
    const std::filesystem::path folder(directory);

    CourseStorage::Courses loadedCourses;
    std::vector<Student> loadedStudents;
    std::vector<AttendanceRecord> loadedRecords;
    std::vector<CorrectionRecord> loadedCorrections;

    if (checkFiles(folder))
    {
        FileStorage::loadAll(
            loadedStudents, (folder / "students.txt").string());

        FileStorage::loadAll(
            loadedRecords, (folder / "attendance.txt").string());

        FileStorage::loadAll(
            loadedCorrections, (folder / "corrections.txt").string());

        const auto registration = folder / "course_enrolments.txt";

        if (std::filesystem::exists(registration))
        {
            CourseStorage::load(
                registration.string(), loadedStudents, loadedCourses);
        }

        validateLinks(loadedStudents, loadedRecords, loadedCorrections);
    }

    // Replace live data only after the complete load succeeds.
    courses.swap(loadedCourses);
    students.swap(loadedStudents);
    records.swap(loadedRecords);
    corrections.swap(loadedCorrections);
}

void StudentHistoryStore::save(const std::string& directory) const
{
    validateLinks(students, records, corrections);

    const auto courseText = CourseStorage::encode(students, courses);
    const auto studentText = FileStorage::encode(students);
    const auto attendanceText = FileStorage::encode(records);
    const auto correctionText = FileStorage::encode(corrections);

    const std::filesystem::path folder(directory);
    checkFiles(folder);
    std::filesystem::create_directories(folder);

    SafeFile::writeAll({
        {(folder / "dataset.txt").string(), "DATASET_V2\n"},
        {(folder / "course_enrolments.txt").string(), courseText},
        {(folder / "students.txt").string(), studentText},
        {(folder / "attendance.txt").string(), attendanceText},
        {(folder / "corrections.txt").string(), correctionText}
    });
}

const std::vector<Student>& StudentHistoryStore::getStudents() const { return students; }
const std::vector<AttendanceRecord>& StudentHistoryStore::getRecords() const { return records; }
const std::vector<CorrectionRecord>& StudentHistoryStore::getCorrections() const { return corrections; }

const Student* StudentHistoryStore::findStudent(const std::string& id) const
{
    for (const Student& student : students)
        if (student.getPersonId() == id)
            return &student;
    return nullptr;
}

const Course* StudentHistoryStore::findCourse(
    const std::string& code) const
{
    for (const auto& course : courses)
        if (course->getCourseCode() == code)
            return course.get();

    return nullptr;
}

void StudentHistoryStore::enrol(const std::string& studentId,
                                const std::string& courseCode)
{
    for (Student& student : students)
        if (student.getPersonId() == studentId)
            for (auto& course : courses)
                if (course->getCourseCode() == courseCode)
                {
                    student.enrol(*course);
                    return;
                }

    throw std::runtime_error("Student or course not found.");
}

void StudentHistoryStore::drop(const std::string& studentId,
                               const std::string& courseCode)
{
    for (Student& student : students)
        if (student.getPersonId() == studentId)
            for (auto& course : courses)
                if (course->getCourseCode() == courseCode)
                {
                    student.drop(*course);
                    return;
                }

    throw std::runtime_error("Student or course not found.");
}

std::vector<const Course*> StudentHistoryStore::getCourses() const
{
    std::vector<const Course*> result;

    for (const auto& course : courses)
        result.push_back(course.get());

    return result;
}