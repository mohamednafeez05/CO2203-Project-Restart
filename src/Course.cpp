#include "Course.h"

bool Course::isFull()
{
    return enrolledStudents.size() >= capacity;
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
}

bool Course::prerequisitesMet(Student& student)
{
    return true;
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