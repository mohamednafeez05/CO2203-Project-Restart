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

// Loads saved history or starts with empty collections.
AttendanceConsole::AttendanceConsole(const std::string& directory)
{
    const std::filesystem::path folder(directory);
    std::filesystem::create_directories(folder);

    attendanceFile = (folder / "attendance.txt").string();
    correctionFile = (folder / "corrections.txt").string();

    // Stop if a previous save left temporary files behind.
    if (std::filesystem::exists(attendanceFile + ".tmp") ||
        std::filesystem::exists(correctionFile + ".tmp"))
    {
        throw std::runtime_error(
            "An unfinished save exists. Inspect the .tmp and .bak "
            "files before starting.");
    }

    const bool hasAttendance =
        std::filesystem::exists(attendanceFile);

    const bool hasCorrections =
        std::filesystem::exists(correctionFile);

    // Missing originals with backups are not a fresh installation.
    if (!hasAttendance && !hasCorrections &&
        (std::filesystem::exists(attendanceFile + ".bak") ||
         std::filesystem::exists(correctionFile + ".bak")))
    {
        throw std::runtime_error(
            "Saved files are missing but backups exist. "
            "Recover the saved files before starting.");
    }

    if (hasAttendance != hasCorrections)
    {
        throw std::runtime_error(
            "One attendance data file is missing. "
            "Restore the missing file before starting.");
    }

    if (hasAttendance)
    {
        attendance.load(attendanceFile, correctionFile);
        std::cout << "Saved attendance history loaded.\n";
    }
    else
    {
        std::cout
            << "No saved attendance history yet. Starting empty.\n";
    }
}

// An empty filter displays every attendance entry.
void AttendanceConsole::showRecords(
    const std::string& studentId) const
{
    std::size_t count = 0;

    for (const AttendanceRecord& record : attendance.getRecords())
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
         attendance.getCorrections())
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

    for (const AttendanceRecord& record : attendance.getRecords())
    {
        students.insert(record.getStudentId());
        sessions.insert(record.getSessionId());
    }

    std::cout
        << "Attendance entries: "
        << attendance.getRecords().size() << '\n'

        << "Correction entries: "
        << attendance.getCorrections().size() << '\n'

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
        std::cout << "\nAttendance History\n"
                  << "1. View all attendance\n"
                  << "2. View correction history\n"
                  << "3. Find student history\n"
                  << "4. View summary\n"
                  << "0. Save and exit\n";

        const int choice =
            ConsoleInput::readInt("Choice: ", 0, 4);

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

            showRecords(id);
            showCorrections(id);
            break;
        }

        case 4:
            showSummary();
            break;

        case 0:
            try
            {
                attendance.save(attendanceFile, correctionFile);

                std::cout
                    << "Attendance and correction history saved.\n";

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