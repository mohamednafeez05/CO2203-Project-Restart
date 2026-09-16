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
    void enrol(Course& course);
    void drop(Course& course);
    void viewTimetable();
    void displayMenu();
};

#endif