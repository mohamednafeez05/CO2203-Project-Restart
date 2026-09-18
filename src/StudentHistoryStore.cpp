#include "StudentHistoryStore.h"
#include "FileStorage.h"
#include "SafeFile.h"
#include <filesystem>
#include <set>
#include <stdexcept>

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
    std::vector<Student> loadedStudents;
    std::vector<AttendanceRecord> loadedRecords;
    std::vector<CorrectionRecord> loadedCorrections;
    if (checkFiles(folder))
    {
        FileStorage::loadAll(loadedStudents, (folder / "students.txt").string());
        FileStorage::loadAll(loadedRecords, (folder / "attendance.txt").string());
        FileStorage::loadAll(loadedCorrections, (folder / "corrections.txt").string());
        validateLinks(loadedStudents, loadedRecords, loadedCorrections);
    }
    // No changes to the live collections until all parsing and checks succeed.
    students.swap(loadedStudents);
    records.swap(loadedRecords);
    corrections.swap(loadedCorrections);
}

void StudentHistoryStore::save(const std::string& directory) const
{
    validateLinks(students, records, corrections);
    const auto studentText = FileStorage::encode(students);
    const auto attendanceText = FileStorage::encode(records);
    const auto correctionText = FileStorage::encode(corrections);
    const std::filesystem::path folder(directory);
    checkFiles(folder);
    std::filesystem::create_directories(folder);
    SafeFile::writeAll({
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
