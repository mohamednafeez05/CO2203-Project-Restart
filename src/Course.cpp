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

double Course::calculateFinalGrade()
{
    return 0.0;
}

bool Course::checkPrerequisitesMet(Student& student)
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