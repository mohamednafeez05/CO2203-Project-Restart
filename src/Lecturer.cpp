#include "Lecturer.h"
#include "Course.h"
#include "Student.h"

#include <ctime>
#include <algorithm>
#include <stdexcept>

Lecturer::Lecturer(const std::string& id,
                   const std::string& name,
                   const std::string& storedHash,
                   const std::string& department)
    : Person(id, name, storedHash),
      department(department)
{
    if (department.empty())
    {
        throw std::invalid_argument(
            "Department cannot be empty.");
    }
}

void Lecturer::displayMenu()
{
    std::cout << "Lecturer dashboard: sign in through UniversityConsole.\n";
}

void Lecturer::viewEnrolmentList(Course& course)
{
    if (!isAssignedTo(course))
    {
        std::cout << "Lecturer is not assigned to this course.\n";
        return;
    }

    const std::vector<Student*>& students =
        course.getEnrolledStudents();

    if (students.empty())
    {
        std::cout << "No students are enrolled in this course.\n";
        return;
    }

    for (const Student* student : students)
    {
        if (student != nullptr)
        {
            std::cout << student->getPersonId()
                      << " | "
                      << student->getName()
                      << '\n';
        }
    }
}
std::string Lecturer::getDepartment() const
{
    return department;
}

const std::vector<Course*>& Lecturer::getAssignedCourses() const
{
    return assignedCourses;
}
void Lecturer::addAssignedCourse(Course& course)
{
    if (!isAssignedTo(course)) assignedCourses.push_back(&course);
}

bool Lecturer::isAssignedTo(const Course& course) const
{
    for (const Course* assigned : assignedCourses)
    {
        if (assigned == &course)
        {
            return true;
        }
    }

    return false;
}
AttendanceSession Lecturer::openAttendanceSession(
    Course& course,
    TimeSlot slot,
    int durationMins)
{
    if (!isAssignedTo(course))
    {
        throw std::logic_error(
            "Lecturer is not assigned to this course.");
    }

    if (durationMins <= 0)
    {
        throw std::invalid_argument(
            "Attendance duration must be positive.");
    }

    const TimeSlot* courseSlot = nullptr;

    for (const TimeSlot& existingSlot : course.getSlots())
    {
        if (existingSlot.getDay() == slot.getDay() &&
            existingSlot.getStartTime() == slot.getStartTime() &&
            existingSlot.getEndTime() == slot.getEndTime() &&
            existingSlot.getLocation() == slot.getLocation())
        {
            courseSlot = &existingSlot;
            break;
        }
    }

    if (courseSlot == nullptr)
    {
        throw std::invalid_argument(
            "Time slot does not belong to this course.");
    }

    std::string sessionId =
        course.getCourseCode() + "-" +
        std::to_string(std::time(nullptr));

    AttendanceSession session(
        sessionId,
        courseSlot,
        durationMins * 60);

    session.open();

    return session;
}
void Lecturer::removeAssignedCourse(Course& course)
{
    assignedCourses.erase(std::remove(assignedCourses.begin(), assignedCourses.end(), &course), assignedCourses.end());
}

#include "UniversityConsole.h"
void Lecturer::displayMenu(UniversityConsole& console) { console.lecturerMenu(*this); }
