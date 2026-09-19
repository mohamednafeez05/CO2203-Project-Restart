#include "Lecturer.h"
#include "Course.h"
#include "Student.h"

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
}

void Lecturer::viewEnrolmentList(Course& course)
{
    const std::vector<Student*>& students =
        course.getEnrolledStudents();

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