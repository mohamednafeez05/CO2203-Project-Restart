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
        std::vector<Course*> completedCourses;
        int yearOfStudy;
        std::string major;
        std::vector<Course*> enrolledCourses;
        Timetable timetable;
public:
    void markCourseCompleted(Course& course);
    bool hasCompletedCourse(const Course& course) const;
    const std::vector<Course*>& getCompletedCourses() const;

    // Initialises personal and academic details.
    Student(const std::string& id, const std::string& name,
            const std::string& storedHash, int year,
            const std::string& major);
    void enrol(Course& course);
    void drop(Course& course);
    void viewTimetable();
    const Timetable& getTimetable() const;

    // Displays the student menu.
    void displayMenu() override;
    void displayMenu(UniversityConsole& console) override;

    // Returns the student's year.
    int getYearOfStudy() const;

    // Returns the student's major.
    std::string getMajor() const;

    // Returns the enrolled course references.
    const std::vector<Course*>& getEnrolledCourses() const;
};

std::ostream& operator<<(std::ostream& os, const Student& student);

#endif