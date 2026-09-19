#include "Student.h"
#include "Course.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include "CourseFullException.h"

class TestCourse : public Course
{
public:
    explicit TestCourse(int capacity = 30)
    : Course("CO2203", "OOP", 3, capacity) {}

    double calculateFinalGrade() override
    {
        return 0.0;
    }
};

int main()
{
    Student first(
        "S001", "Nafeez", "TEST_ONLY", 2, "Computer Engineering"
    );

    Student second(
        "S002", "Student Two", "TEST_ONLY", 2, "Computer Engineering"
    );

    const Person& person = first;
    assert(person.getId() == "S001");
    assert(person.getId() == person.getPersonId());

    TestCourse course;
    course.addStudent(first);
    course.addStudent(second);

    course.removeStudent(first);
    assert(!course.isStudentEnrolled(first));
    assert(course.isStudentEnrolled(second));
    assert(course.getEnrolledStudents().size() == 1);

    // Removing someone already removed should change nothing.
    course.removeStudent(first);
    assert(course.getEnrolledStudents().size() == 1);

    course.removeStudent(second);
    assert(course.getEnrolledStudents().empty());

        TestCourse limitedCourse(1);

    first.enrol(limitedCourse);
    assert(limitedCourse.isStudentEnrolled(first));
    assert(first.getEnrolledCourses().size() == 1);
    assert(first.getEnrolledCourses().front() == &limitedCourse);

    first.enrol(limitedCourse);
    assert(first.getEnrolledCourses().size() == 1);
    assert(limitedCourse.getEnrolledStudents().size() == 1);

    bool fullRejected = false;

    try
    {
        second.enrol(limitedCourse);
    }
    catch (const CourseFullException&)
    {
        fullRejected = true;
    }

    assert(fullRejected);
    assert(second.getEnrolledCourses().empty());
    assert(!limitedCourse.isStudentEnrolled(second));
    assert(limitedCourse.getEnrolledStudents().size() == 1);

    first.drop(limitedCourse);
    assert(first.getEnrolledCourses().empty());
    assert(limitedCourse.getEnrolledStudents().empty());

    first.drop(limitedCourse);
    assert(limitedCourse.getEnrolledStudents().empty());

    second.enrol(limitedCourse);
    assert(limitedCourse.isStudentEnrolled(second));
    assert(second.getEnrolledCourses().size() == 1);

    second.drop(limitedCourse);
    assert(second.getEnrolledCourses().empty());
    assert(limitedCourse.getEnrolledStudents().empty());

    std::cout
        << "PASS: enrolment, duplicate prevention, capacity and dropping.\n";

    std::ostringstream studentText;
    const Student& readOnlyStudent = first;
    studentText << readOnlyStudent;

    assert(studentText.str() ==
        "S001 | Nafeez | Year 2 | Computer Engineering");

    assert(studentText.str().find("TEST_ONLY") == std::string::npos);

    std::ostringstream personText;
    personText << person;

    assert(personText.str().find("S001") != std::string::npos);
    assert(personText.str().find("TEST_ONLY") == std::string::npos);

    std::ostringstream courseText;
    const Course& readOnlyCourse = course;
    courseText << readOnlyCourse;

    assert(courseText.str().find("CO2203") != std::string::npos);

    std::cout
        << "PASS: read-only student, person and course printing.\n";

    std::cout
        << "PASS: ID compatibility and course roster removal.\n";
}