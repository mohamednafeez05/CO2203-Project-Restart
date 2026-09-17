#include "Student.h"

void Student::displayMenu()
{
}

void Student::viewTimetable()
{
    std::cout << timetable << std::endl;
}

void Student::drop(Course& course)
{
}

void Student::enrol(Course& course)
{
}

int Student::getYearOfStudy() const
{
    return yearOfStudy;
}

std::string Student::getMajor() const
{
    return major;
}

const std::vector<Course*>& Student::getEnrolledCourses() const
{
    return enrolledCourses;
}