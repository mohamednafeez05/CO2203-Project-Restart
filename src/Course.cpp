#include "Course.h"
#include "Student.h"
#include <algorithm>
#include <stdexcept>

Course::Course(const std::string& code,
               const std::string& title,
               int credits,
               int capacity)
    : courseCode(code),
      title(title),
      creditValue(credits),
      capacity(capacity),
      lecturer(nullptr)
{
    if (code.empty() || title.empty() ||
        credits <= 0 || capacity <= 0)
    {
        throw std::invalid_argument(
            "Course details are invalid.");
    }
}

bool Course::isFull()
{
    return capacity <= 0 ||
           enrolledStudents.size() >=
               static_cast<std::size_t>(capacity);
}

const std::vector<TimeSlot>& Course::getSlots() const
{
    return slots;
}

void Course::addStudent(Student& student)
{
    enrolledStudents.push_back(&student);
}

void Course::removeStudent(Student& student)
{
    auto it = std::find(
        enrolledStudents.begin(),
        enrolledStudents.end(),
        &student);

    if (it != enrolledStudents.end())
    {
        enrolledStudents.erase(it);
    }
}

bool Course::isStudentEnrolled(const Student& student) const
{
    for (const Student* enrolled : enrolledStudents)
    {
        if (enrolled == &student)
        {
            return true;
        }
    }

    return false;
}

bool Course::prerequisitesMet(Student& student)
{
    /*
        Temporary until the team decides how
        completed prerequisite courses are represented.

        Student currently stores enrolledCourses,
        not completedCourses.
    */

    return true;
}

std::string Course::getCourseCode() const
{
    return courseCode;
}

std::string Course::getTitle() const
{
    return title;
}

int Course::getCreditValue() const
{
    return creditValue;
}

int Course::getCapacity() const
{
    return capacity;
}

Lecturer* Course::getLecturer() const
{
    return lecturer;
}

const std::vector<Course*>& Course::getPrerequisites() const
{
    return prerequisites;
}

const std::vector<Student*>& Course::getEnrolledStudents() const
{
    return enrolledStudents;
}

std::ostream& operator<<(std::ostream& os, Course& c)
{
    os << "Course Code: " << c.courseCode << std::endl;
    os << "Title: " << c.title << std::endl;

    return os;
}

Course::~Course()
{
}