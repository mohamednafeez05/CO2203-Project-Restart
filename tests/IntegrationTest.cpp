#include "Student.h"
#include "Course.h"
#include <cassert>
#include <iostream>
#include <sstream>

class TestCourse : public Course
{
public:
    TestCourse() : Course("CO2203", "OOP", 3, 30) {}

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