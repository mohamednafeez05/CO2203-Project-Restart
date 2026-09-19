#include "AttendanceConsole.h"
#include "FileStorage.h"
#include "SafeFile.h"
#include <cassert>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <stdexcept>

std::string runMenu(const std::string& folder,
                    const std::string& commands)
{
    std::istringstream input(commands);
    std::ostringstream output;

    auto* oldIn = std::cin.rdbuf(input.rdbuf());
    auto* oldOut = std::cout.rdbuf(output.rdbuf());

    try
    {
        AttendanceConsole console(folder);
        console.run();
    }
    catch (...)
    {
        std::cin.rdbuf(oldIn);
        std::cout.rdbuf(oldOut);
        throw;
    }

    std::cin.rdbuf(oldIn);
    std::cout.rdbuf(oldOut);

    return output.str();
}

int main()
{
    namespace fs = std::filesystem;

    const std::string folder = "data/registration_console_test";
    assert(!fs::exists(folder));
    fs::create_directories(folder);

    std::vector<Student> students;

    students.emplace_back(
        "S001", "Nafeez", "TEST_ONLY", 2, "Engineering");

    students.emplace_back(
        "S002", "Student Two", "TEST_ONLY", 2, "Engineering");

    SafeFile::writeAll({
        {folder + "/students.txt", FileStorage::encode(students)},
        {folder + "/attendance.txt", "ATTENDANCE_V1\n"},
        {folder + "/corrections.txt", "CORRECTIONS_V1\n"},
        {folder + "/course_enrolments.txt",
         "COURSE_ENROLMENTS_V1\n"
         "COURSE \"LECTURE\" \"C1\" \"OOP\" 3 1\n"
         "SLOT \"C1\" \"Monday\" 540 600 \"Room A\"\n"
         "COURSE \"LAB\" \"C2\" \"Programming Lab\" 1 20\n"
         "SLOT \"C2\" \"Monday\" 570 630 \"Lab B\"\n"}
    });

    const std::string text = runMenu(
        folder,
        "6\n7\nS001\nC1\n7\nS001\nC1\n7\nS002\nC1\n"
        "7\nS001\nC2\n9\nS001\n8\nS001\nC1\n8\nS001\nC1\n"
        "7\nS001\nC2\n7\nS999\nC2\n7\nS001\nMISSING\n9\nS999\n0\n"
    );

    for (const char* expected : {
        "Course Code: C1",
        "Enrolment successful.",
        "Student is already enrolled.",
        "Course is full",
        "Timetable clash detected",
        "Course dropped.",
        "Student is not enrolled in this course.",
        "Student or course not found.",
        "Student not found: S999",
        "=== Weekly Timetable ==="})
    {
        assert(text.find(expected) != std::string::npos);
    }

    StudentHistoryStore restored;
    restored.load(folder);

    const Student* student = restored.findStudent("S001");

    assert(student->getEnrolledCourses().size() == 1);
    assert(restored.findCourse("C2")->isStudentEnrolled(*student));
    assert(restored.findCourse("C1")->getEnrolledStudents().empty());
    assert(restored.findStudent("S002")->getEnrolledCourses().empty());

    const std::string restarted = runMenu(folder, "9\nS001\n0\n");

    assert(restarted.find("Lab B") != std::string::npos);
    assert(restarted.find("Room A") == std::string::npos);

    bool ended = false;

    try
    {
        runMenu(folder, "7\n");
    }
    catch (const std::runtime_error& error)
    {
        ended = std::string(error.what()) == "Console input ended.";
    }

    assert(ended);

    std::cout
        << "PASS: registration menu, errors, timetable, save and restart.\n";
}