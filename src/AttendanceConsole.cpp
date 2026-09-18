#include "AttendanceConsole.h"
#include "ConsoleInput.h"

#include <ctime>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>

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
                  << "0. Save and exit\n";

        const int choice =
            ConsoleInput::readInt("Choice: ", 0, 5);

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

        case 0:
            try
            {
                data.save(dataDirectory);

                std::cout
                    << "Student, attendance and correction data saved.\n";

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