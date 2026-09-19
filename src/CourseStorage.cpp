#include "CourseStorage.h"
#include "LectureCourse.h"
#include "LabCourse.h"
#include <fstream>
#include <iomanip>
#include <set>
#include <sstream>
#include <stdexcept>
#include <typeinfo>
#include <utility>

namespace
{
    Course* findCourse(CourseStorage::Courses& courses,
                       const std::string& code)
    {
        for (auto& course : courses)
            if (course->getCourseCode() == code)
                return course.get();

        throw std::runtime_error("Unknown course: " + code);
    }

    Student* findStudent(std::vector<Student>& students,
                         const std::string& id)
    {
        for (Student& student : students)
            if (student.getPersonId() == id)
                return &student;

        throw std::runtime_error("Unknown student: " + id);
    }

    void validText(const std::string& text)
    {
        if (text.empty() ||
            text.find_first_of("\r\n") != std::string::npos)
        {
            throw std::runtime_error("Invalid course text field.");
        }
    }
}

void CourseStorage::load(const std::string& filename,
                         std::vector<Student>& students,
                         Courses& courses)
{
    if (!courses.empty())
        throw std::runtime_error("Load courses into a fresh collection.");

    for (const Student& student : students)
        if (!student.getEnrolledCourses().empty())
            throw std::runtime_error("Load student profiles before enrolments.");

    std::ifstream input(filename);
    std::string line;

    if (!std::getline(input, line) || line != "COURSE_ENROLMENTS_V1")
        throw std::runtime_error("Cannot read course file or invalid header.");

    Courses loaded;
    std::vector<Student> loadedStudents = students;
    std::set<std::string> codes;
    std::set<std::pair<std::string, std::string>> enrolments;

    while (std::getline(input, line))
    {
        std::istringstream row(line);
        std::string kind, code;

        if (!(row >> kind))
            throw std::runtime_error("Empty course file row.");

        if (kind == "COURSE")
        {
            std::string type, title;
            int credits, capacity;

            if (!(row >> std::quoted(type)
                      >> std::quoted(code)
                      >> std::quoted(title)
                      >> credits >> capacity))
            {
                throw std::runtime_error("Malformed COURSE row.");
            }

            validText(code);
            validText(title);

            if (!codes.insert(code).second)
                throw std::runtime_error("Duplicate course: " + code);

            if (type == "LECTURE")
            {
                loaded.push_back(std::make_unique<LectureCourse>(
                    code, title, credits, capacity));
            }
            else if (type == "LAB")
            {
                loaded.push_back(std::make_unique<LabCourse>(
                    code, title, credits, capacity));
            }
            else
            {
                throw std::runtime_error(
                    "Unsupported course type: " + type);
            }
        }
        else if (kind == "SLOT")
        {
            std::string day, location;
            int start, end;

            if (!(row >> std::quoted(code)
                      >> std::quoted(day)
                      >> start >> end
                      >> std::quoted(location)))
            {
                throw std::runtime_error("Malformed SLOT row.");
            }

            validText(day);
            validText(location);

            findCourse(loaded, code)->addTimeSlot(
                TimeSlot(day, start, end, location));
        }
        else if (kind == "ENROL")
        {
            std::string id;

            if (!(row >> std::quoted(id) >> std::quoted(code)))
                throw std::runtime_error("Malformed ENROL row.");

            if (!enrolments.insert({id, code}).second)
                throw std::runtime_error("Duplicate enrolment.");
        }
        else
        {
            throw std::runtime_error("Unknown course row: " + kind);
        }

        row >> std::ws;

        if (!row.eof())
            throw std::runtime_error("Extra data in course row.");
    }

    if (input.bad() || !input.eof())
        throw std::runtime_error("Course file read failed.");

    for (const auto& course : loaded)
        if (course->getSlots().empty())
            throw std::runtime_error("Course needs at least one slot.");

    // Link objects after creating every course slot.
    for (const auto& entry : enrolments)
    {
        findStudent(loadedStudents, entry.first)->enrol(
            *findCourse(loaded, entry.second));
    }

    students.swap(loadedStudents);
    courses.swap(loaded);
}

std::string CourseStorage::encode(
    const std::vector<Student>& students,
    const Courses& courses)
{
    std::ostringstream output;
    output << "COURSE_ENROLMENTS_V1\n";

    std::set<const Course*> knownCourses;
    std::set<const Student*> knownStudents;
    std::set<std::string> codes, ids;

    for (const Student& student : students)
    {
        validText(student.getPersonId());

        if (!ids.insert(student.getPersonId()).second)
            throw std::runtime_error("Duplicate student ID.");

        knownStudents.insert(&student);
    }

    for (const auto& owner : courses)
    {
        if (!owner)
            throw std::runtime_error("Null course.");

        const Course& course = *owner;
        knownCourses.insert(&course);

        const std::string code = course.getCourseCode();
        validText(code);
        validText(course.getTitle());

        if (!codes.insert(code).second || course.getSlots().empty())
            throw std::runtime_error("Duplicate course or missing slots.");

        if (course.getLecturer() || !course.getPrerequisites().empty())
        {
            throw std::runtime_error(
                "Lecturer/prerequisite storage is not connected yet.");
        }

        std::string type;

        if (typeid(course) == typeid(LectureCourse))
            type = "LECTURE";
        else if (typeid(course) == typeid(LabCourse))
            type = "LAB";
        else
            throw std::runtime_error("Unsupported course type for saving.");

        output << "COURSE " << std::quoted(type)
               << ' ' << std::quoted(code)
               << ' ' << std::quoted(course.getTitle())
               << ' ' << course.getCreditValue()
               << ' ' << course.getCapacity() << '\n';

        for (const TimeSlot& slot : course.getSlots())
        {
            validText(slot.getDay());
            validText(slot.getLocation());

            output << "SLOT " << std::quoted(code)
                   << ' ' << std::quoted(slot.getDay())
                   << ' ' << slot.getStartTime()
                   << ' ' << slot.getEndTime()
                   << ' ' << std::quoted(slot.getLocation()) << '\n';
        }

        std::set<const Student*> roster;

        for (const Student* student : course.getEnrolledStudents())
        {
            if (!knownStudents.count(student) ||
                !roster.insert(student).second)
            {
                throw std::runtime_error("Invalid course roster.");
            }

            unsigned matches = 0;

            for (const Course* enrolled : student->getEnrolledCourses())
                if (enrolled == &course)
                    ++matches;

            if (matches != 1)
                throw std::runtime_error("Inconsistent enrolment.");
        }
    }

    for (const Student& student : students)
    {
        std::set<const Course*> seen;

        for (const Course* course : student.getEnrolledCourses())
        {
            if (!knownCourses.count(course) ||
                !seen.insert(course).second ||
                !course->isStudentEnrolled(student))
            {
                throw std::runtime_error("Invalid student enrolment.");
            }

            output << "ENROL " << std::quoted(student.getPersonId())
                   << ' ' << std::quoted(course->getCourseCode()) << '\n';
        }
    }

    return output.str();
}