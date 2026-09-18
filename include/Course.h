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
    // Initialises course details and capacity.
    Course(const std::string& code, const std::string& title,
        int credits, int capacity);
        
    bool isFull();
    bool prerequisitesMet(Student& student);
    const std::vector<TimeSlot>& getSlots() const;
    void addStudent(Student& student);
    void removeStudent(Student& student);

    // Checks whether the student is enrolled.
    bool isStudentEnrolled(const Student& student) const;

    // Requires each course type to provide its grading rule.
    virtual double calculateFinalGrade() = 0;

    friend std::ostream& operator<<(std::ostream& os, const Course& c);

    virtual ~Course();

    // Returns the course identifier.
    std::string getCourseCode() const;

    // Returns the course title.
    std::string getTitle() const;

    // Returns the credit value.
    int getCreditValue() const;

    // Returns the enrolment limit.
    int getCapacity() const;

    // Returns the assigned lecturer.
    Lecturer* getLecturer() const;

    // Returns the prerequisite course references.
    const std::vector<Course*>& getPrerequisites() const;

    // Returns the enrolled student references.
    const std::vector<Student*>& getEnrolledStudents() const;
};

#endif