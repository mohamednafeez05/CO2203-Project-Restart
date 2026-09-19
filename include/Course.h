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
    Course(const std::string& code,
           const std::string& title,
           int credits,
           int capacity);

    bool isFull();

    bool prerequisitesMet(Student& student);

    const std::vector<TimeSlot>& getSlots() const;

    void addStudent(Student& student);
    void removeStudent(Student& student);

    bool isStudentEnrolled(const Student& student) const;

    // Each course type provides its own grading rule.
    virtual double calculateFinalGrade() = 0;

    std::string getCourseCode() const;
    std::string getTitle() const;
    int getCreditValue() const;
    int getCapacity() const;
    Lecturer* getLecturer() const;

    const std::vector<Course*>& getPrerequisites() const;
    const std::vector<Student*>& getEnrolledStudents() const;

    friend std::ostream& operator<<(std::ostream& os, Course& c);

    virtual ~Course();
};

#endif