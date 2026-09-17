#include "Student.h"
#include <stdexcept>

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

// Initialises and validates student details.
Student::Student(const std::string& id, const std::string& name,
                 const std::string& storedHash, int year,
                 const std::string& major)
    : Person(id, name, storedHash), yearOfStudy(year), major(major)
{
    if (year < 1 || major.empty())
    {
        throw std::invalid_argument("Student year and major are invalid.");
    }
}