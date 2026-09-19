#include "Student.h"
#include <stdexcept>
#include "Course.h"
#include "ConsoleInput.h"
#include "CourseFullException.h"
#include "PrerequisiteNotMetException.h"
#include "TimetableClashException.h"
#include <algorithm>

// Displays the student's available actions.
void Student::displayMenu()
{
    while (true)
    {
        std::cout << "\nStudent Menu\n"
                  << "1. View profile\n"
                  << "2. View enrolled courses\n"
                  << "3. View timetable\n"
                  << "0. Return\n";

        int choice = ConsoleInput::readInt("Choice: ", 0, 3);

        switch (choice)
        {
        case 1:
            std::cout << "\nID: " << getPersonId()
                      << "\nName: " << getName()
                      << "\nYear: " << yearOfStudy
                      << "\nMajor: " << major << '\n';
            break;

        case 2:
            if (enrolledCourses.empty())
            {
                std::cout << "No enrolled courses.\n";
            }
            else
            {
                for (const Course* course : enrolledCourses)
                {
                    if (course != nullptr)
                    {
                        std::cout << course->getCourseCode()
                                  << " | " << course->getTitle()
                                  << '\n';
                    }
                }
            }
            break;

        case 3:
            viewTimetable();
            break;

        case 0:
            return;
        }
    }
}

void Student::viewTimetable()
{
    std::cout << timetable << std::endl;
}

void Student::drop(Course& course)
{
    auto it = std::find(
        enrolledCourses.begin(),
        enrolledCourses.end(),
        &course
    );

    if (it != enrolledCourses.end())
    {
        // Remove timetable references before removing the enrolment.
        for (const TimeSlot& slot : course.getSlots())
        {
            timetable.removeTimeSlot(slot);
        }

        enrolledCourses.erase(it);
        course.removeStudent(*this);
    }
}

void Student::enrol(Course& course)
{
    // Repeated enrolment leaves the existing registration unchanged.
    if (course.isStudentEnrolled(*this))
    {
        return;
    }

    if (course.isFull())
    {
        throw CourseFullException();
    }

    if (!course.prerequisitesMet(*this))
    {
        throw PrerequisiteNotMetException();
    }

    // Check all slots before changing either course list.
    for (const TimeSlot& slot : course.getSlots())
    {
        if (timetable.hasClashWith(slot))
        {
            throw TimetableClashException();
        }
    }

    course.addStudent(*this);
    enrolledCourses.push_back(&course);

    for (const TimeSlot& slot : course.getSlots())
    {
        timetable.addTimeSlot(slot);
    }
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

std::ostream& operator<<(std::ostream& os, const Student& student)
{
    os << student.getPersonId() << " | " << student.getName()
       << " | Year " << student.getYearOfStudy()
       << " | " << student.getMajor();

    return os;
}