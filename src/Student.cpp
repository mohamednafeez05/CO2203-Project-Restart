#include "Student.h"
#include "Course.h"
#include "ConsoleInput.h"

#include "CourseFullException.h"
#include "PrerequisiteNotMetException.h"
#include "TimetableClashException.h"

#include <algorithm>
#include <stdexcept>

Student::Student(const std::string& id,
                 const std::string& name,
                 const std::string& storedHash,
                 int year,
                 const std::string& major)
    : Person(id, name, storedHash),
      yearOfStudy(year),
      major(major)
{
    if (year < 1 || major.empty())
    {
        throw std::invalid_argument(
            "Student year and major are invalid.");
    }
}

void Student::displayMenu()
{
    while (true)
    {
        std::cout << "\nStudent Menu\n"
                  << "1. View profile\n"
                  << "2. View enrolled courses\n"
                  << "3. View timetable\n"
                  << "0. Return\n";

        int choice =
            ConsoleInput::readInt("Choice: ", 0, 3);

        switch (choice)
        {
        case 1:
            std::cout << "\nID: " << getPersonId()
                      << "\nName: " << getName()
                      << "\nYear: " << yearOfStudy
                      << "\nMajor: " << major
                      << '\n';
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
                        std::cout
                            << course->getCourseCode()
                            << " | "
                            << course->getTitle()
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

void Student::enrol(Course& course)
{
    // Student is already enrolled.
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

    // Check every course slot for timetable clashes first.
    for (const TimeSlot& slot : course.getSlots())
    {
        if (timetable.hasClashWith(slot))
        {
            throw TimetableClashException();
        }
    }

    // Update both sides of the relationship.
    course.addStudent(*this);
    enrolledCourses.push_back(&course);

    // Add the course slots to the student's timetable.
    for (const TimeSlot& slot : course.getSlots())
    {
        timetable.addTimeSlot(slot);
    }
}

void Student::drop(Course& course)
{
    auto it = std::find(
        enrolledCourses.begin(),
        enrolledCourses.end(),
        &course);

    if (it == enrolledCourses.end())
    {
        return;
    }

    // Remove this course's slots from the student's timetable.
    for (const TimeSlot& slot : course.getSlots())
    {
        timetable.removeTimeSlot(slot);
    }

    // Remove the relationship from both sides.
    enrolledCourses.erase(it);
    course.removeStudent(*this);
}

int Student::getYearOfStudy() const
{
    return yearOfStudy;
}

std::string Student::getMajor() const
{
    return major;
}
void Student::markCourseCompleted(Course& course)
{
    if (!hasCompletedCourse(course))
    {
        completedCourses.push_back(&course);
    }
}
bool Student::hasCompletedCourse(const Course& course) const
{
    for (const Course* completed : completedCourses)
    {
        if (completed == &course)
        {
            return true;
        }
    }

    return false;
}
const std::vector<Course*>& Student::getCompletedCourses() const
{
    return completedCourses;
}
const std::vector<Course*>& Student::getEnrolledCourses() const
{
    return enrolledCourses;
}