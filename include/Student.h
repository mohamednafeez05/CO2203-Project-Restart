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
    std::vector<Course*> completedCourses;

public:
                Student(const std::string& id,
                const std::string& name,
                const std::string& storedHash,
                int year,
                const std::string& major);
                void markCourseCompleted(Course& course);
                 bool hasCompletedCourse(const Course& course) const;
                 const std::vector<Course*>& getCompletedCourses() const;

    void enrol(Course& course);
    void drop(Course& course);
    void viewTimetable();

    void displayMenu() override;

    int getYearOfStudy() const;
    std::string getMajor() const;

    const std::vector<Course*>& getEnrolledCourses() const;
};

#endif