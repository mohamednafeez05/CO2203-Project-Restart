#ifndef STUDENT_H
#define STUDENT_H

#include "Person.h"
#include "Timetable.h"
#include <vector>
#include <string>

class Course;

class Student : public Person
{
    private:
        int yearOfStudy;
        std::string major;
        std::vector<Course*> enrolledCourses;
        Timetable timetable;
public:
    // Initialises personal and academic details.
    Student(const std::string& id, const std::string& name,
            const std::string& storedHash, int year,
            const std::string& major);
    void enrol(Course& course);
    void drop(Course& course);
    void viewTimetable();

    // Displays the student menu.
    void displayMenu() override;

    // Returns the student's year.
    int getYearOfStudy() const;

    // Returns the student's major.
    std::string getMajor() const;

    // Returns the enrolled course references.
    const std::vector<Course*>& getEnrolledCourses() const;
};

#endif