#ifndef COURSE_H
#define COURSE_H

#include <iostream>
#include <string>
#include <vector>
#include "TimeSlot.h"

class Lecturer;
class Student;


class Course
{

private:
    std::string courseCode;
    std::string title;
    int creditValue;
    int capacity;
    Lecturer* lecturer;
    std::vector<Course*> prerequisites;
    std::vector<Student*> enrolledStudents;
    std::vector<TimeSlot> slots;

public:
    bool isFull();
    bool checkPrerequisitesMet(Student& student);
    const std::vector<TimeSlot>& getSlots() const;
    void addStudent(Student& student);
    void removeStudent(Student& student);
    virtual double calculateFinalGrade();

    friend std::ostream& operator<<(std::ostream& os, Course& c);

    virtual ~Course();
};

#endif