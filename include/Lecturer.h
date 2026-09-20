#ifndef LECTURER_H
#define LECTURER_H

#include "Person.h"
#include "AttendanceSession.h"

#include <string>
#include <vector>

class Course;
class TimeSlot;

class Lecturer : public Person
{
private:
    std::string department;
    std::vector<Course*> assignedCourses;

public:
    Lecturer(const std::string& id,
             const std::string& name,
             const std::string& storedHash,
             const std::string& department);
             void addAssignedCourse(Course& course);
            bool isAssignedTo(const Course& course) const;

    void viewEnrolmentList(Course& course);

    AttendanceSession openAttendanceSession(
        Course& course,
        TimeSlot slot,
        int durationMins);

    void displayMenu() override;

    std::string getDepartment() const;

    const std::vector<Course*>& getAssignedCourses() const;
};

#endif