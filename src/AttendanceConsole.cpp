#include "AttendanceConsole.h"
#include "ConsoleInput.h"

#include <ctime>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include "AttendanceRegister.h"

namespace
{
    // Formats timestamps using the computer's local time.
    std::string formatTime(std::time_t value)
    {
        const std::tm* local = std::localtime(&value);

        if (!local)
            return "Timestamp " + std::to_string(value);

        std::ostringstream text;
        text << std::put_time(local, "%Y-%m-%d %H:%M:%S");
        return text.str();
    }
}

// Load the complete dataset before showing any menu.
AttendanceConsole::AttendanceConsole(const std::string& directory)
    : dataDirectory(directory)
{
    data.load(directory);
    std::cout << "Student and attendance data ready.\n";
}

void AttendanceConsole::showStudents() const
{
    for (const Student& student : data.getStudents())
    {
        std::cout << student << '\n';
    }

    std::cout << "Registered students: "
              << data.getStudents().size() << '\n';
}

bool AttendanceConsole::showProfile(const std::string& id) const
{
    const Student* student = data.findStudent(id);

    if (!student)
    {
        std::cout << "Student not found: " << id << '\n';
        return false;
    }

    std::cout << "Student: " << *student << '\n';

    for (const Course* course : student->getEnrolledCourses())
        std::cout << *course;

    return true;
}

// An empty filter displays every attendance entry.
void AttendanceConsole::showRecords(
    const std::string& studentId) const
{
    std::size_t count = 0;

    for (const AttendanceRecord& record : data.getRecords())
    {
        if (!studentId.empty() &&
            record.getStudentId() != studentId)
        {
            continue;
        }

        std::cout << record.getStudentId() << " | "
                  << record.getSessionId() << " | "
                  << formatTime(record.getCheckInTime()) << '\n';

        ++count;
    }

    std::cout << "Attendance entries: " << count << '\n';
}

// Displays audit notes without changing attendance totals.
void AttendanceConsole::showCorrections(
    const std::string& studentId) const
{
    std::size_t count = 0;

    for (const CorrectionRecord& correction :
         data.getCorrections())
    {
        if (!studentId.empty() &&
            correction.getStudentId() != studentId)
        {
            continue;
        }

        std::cout << correction.getStudentId() << " | "
                  << correction.getSessionId() << " | "
                  << formatTime(correction.getCorrectionTime()) << '\n'
                  << "Reason: " << correction.getReason() << '\n';

        ++count;
    }

    std::cout << "Correction entries: " << count << '\n';
}

// Counts IDs represented by the attendance entries.
void AttendanceConsole::showSummary() const
{
    std::set<std::string> students;
    std::set<std::string> sessions;

    for (const AttendanceRecord& record : data.getRecords())
    {
        students.insert(record.getStudentId());
        sessions.insert(record.getSessionId());
    }

    std::cout
        << "Registered students: " << data.getStudents().size() << '\n'
        << "Attendance entries: "
        << data.getRecords().size() << '\n'

        << "Correction entries: "
        << data.getCorrections().size() << '\n'

        << "Students with attendance entries: "
        << students.size() << '\n'

        << "Sessions with attendance entries: "
        << sessions.size() << '\n';
}

// Keeps the menu open if saving fails.
void AttendanceConsole::run()
{
    while (true)
    {
        std::cout << "\nStudent and Attendance History\n"
                  << "1. View all attendance\n"
                  << "2. View correction history\n"
                  << "3. Find student history\n"
                  << "4. View summary\n"
                  << "5. List students\n"
                  << "6. List courses\n"
                  << "7. Enrol a student\n"
                  << "8. Drop a course\n"
                  << "9. View student timetable\n"
                  << "10. Import attendance batch\n"
                  << "11. Link attendance session to course\n"
                  << "0. Save and exit\n";

        const int choice =
            ConsoleInput::readInt("Choice: ", 0, 11);

        switch (choice)
        {
        case 1:
            showRecords("");
            break;

        case 2:
            showCorrections("");
            break;

        case 3:
        {
            const std::string id =
                ConsoleInput::readText("Student ID: ");

            if (showProfile(id))
            {
                showRecords(id);
                showCorrections(id);
            }
            break;
        }

        case 4:
            showSummary();
            break;

        case 5:
            showStudents();
            break;

        case 6:
            showCourses();
            break;

        case 7:
            changeEnrolment(true);
            break;

        case 8:
            changeEnrolment(false);
            break;

        case 9:
            showTimetable();
            break;

        case 10:
            importAttendanceBatch();
            break;

        case 11:
            linkAttendanceSession();
            break;

        case 0:
            try
            {
                data.save(dataDirectory);

                std::cout
                    << "Student, course, enrolment, attendance and correction data saved.\n";

                return;
            }
            catch (const std::exception& error)
            {
                std::cerr
                    << "Save failed: " << error.what()
                    << "\nCheck the data folder and try again.\n";
            }
            break;
        }
    }
}

void AttendanceConsole::showCourses() const
{
    const auto courses = data.getCourses();

    if (courses.empty())
    {
        std::cout << "No courses available.\n";
        return;
    }

    for (const Course* course : courses)
    {
        std::cout << *course
                  << "Enrolled: "
                  << course->getEnrolledStudents().size()
                  << " / " << course->getCapacity() << '\n';

        for (const TimeSlot& slot : course->getSlots())
        {
            std::cout << slot.getDay()
                      << " | " << slot.getStartTime()
                      << " - " << slot.getEndTime()
                      << " | " << slot.getLocation() << '\n';
        }
    }
}

void AttendanceConsole::changeEnrolment(bool enrol)
{
    const std::string id = ConsoleInput::readText("Student ID: ");
    const std::string code = ConsoleInput::readText("Course code: ");

    const Student* student = data.findStudent(id);
    const Course* course = data.findCourse(code);

    if (!student || !course)
    {
        std::cout << "Student or course not found.\n";
        return;
    }

    const bool enrolled = course->isStudentEnrolled(*student);

    if (enrol && enrolled)
    {
        std::cout << "Student is already enrolled.\n";
        return;
    }

    if (!enrol && !enrolled)
    {
        std::cout << "Student is not enrolled in this course.\n";
        return;
    }

    try
    {
        if (enrol)
            data.enrol(id, code);
        else
            data.drop(id, code);

        std::cout
            << (enrol ? "Enrolment successful.\n" : "Course dropped.\n")
            << "Choose 0 to save your changes before exiting.\n";
    }
    catch (const std::exception& error)
    {
        std::cout << "Action failed: " << error.what() << '\n';
    }
}

void AttendanceConsole::showTimetable() const
{
    const std::string id = ConsoleInput::readText("Student ID: ");
    const Student* student = data.findStudent(id);

    if (!student)
    {
        std::cout << "Student not found: " << id << '\n';
        return;
    }

    std::cout << *student << '\n' << student->getTimetable();
}

// Imports both files before changing the history collections.
void AttendanceConsole::importAttendanceBatch()
{
    const std::string attendanceFile =
        ConsoleInput::readText("Attendance batch file: ");

    const std::string correctionFile =
        ConsoleInput::readText("Correction batch file: ");

    try
    {
        AttendanceRegister batch;
        batch.load(attendanceFile, correctionFile);
        data.importAttendance(batch);

        std::cout << "Imported " << batch.getRecords().size()
                  << " attendance entries and "
                  << batch.getCorrections().size()
                  << " correction notes.\n"
                  << "Choose 0 to save your changes before exiting.\n";
    }
    catch (const std::exception& error)
    {
        std::cout << "Import failed: " << error.what() << '\n';
    }
}

void AttendanceConsole::linkAttendanceSession()
{
    const std::string sessionId =
        ConsoleInput::readText("Session ID: ");

    const std::string courseCode =
        ConsoleInput::readText("Course code: ");

    try
    {
        data.linkSession(sessionId, courseCode);
        std::cout << "Session linked. Choose 0 to save.\n";
    }
    catch (const std::exception& error)
    {
        std::cout << "Link failed: " << error.what() << '\n';
    }
}