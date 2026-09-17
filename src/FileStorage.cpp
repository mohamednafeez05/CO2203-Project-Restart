#include "FileStorage.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <set>
#include <stdexcept>
#include <utility>

// Validates and writes student records.
void FileStorage::saveAll(const std::vector<Student>& students,
                          const std::string& filename)
{
    std::ostringstream content;
    std::set<std::string> ids;
    content << "STUDENTS_V1\n";

    for (const Student& student : students)
    {
        const std::string fields[] = {
            student.getPersonId(),
            student.getName(),
            student.getPasswordHash(),
            student.getMajor()
        };

        for (const std::string& field : fields)
        {
            if (field.empty() ||
                field.find_first_of("\r\n") != std::string::npos)
            {
                throw std::runtime_error("Invalid student text field.");
            }
        }

        if (!ids.insert(fields[0]).second)
        {
            throw std::runtime_error("Duplicate student ID.");
        }

        if (student.getYearOfStudy() < 1)
        {
            throw std::runtime_error("Invalid student year.");
        }

        content << std::quoted(fields[0]) << ' '
                << std::quoted(fields[1]) << ' '
                << std::quoted(fields[2]) << ' '
                << student.getYearOfStudy() << ' '
                << std::quoted(fields[3]) << '\n';
    }

    std::ofstream output(filename);

    if (!output)
    {
        throw std::runtime_error(
            "Cannot open file for saving: " + filename);
    }

    output << content.str();
    output.close();

    if (!output)
    {
        throw std::runtime_error(
            "Failed to finish saving: " + filename);
    }
}

// Parses all records before replacing the collection.
void FileStorage::loadAll(std::vector<Student>& students,
                          const std::string& filename)
{
    std::ifstream input(filename);

    if (!input)
    {
        throw std::runtime_error(
            "Cannot open file for loading: " + filename);
    }

    std::string line;

    if (!std::getline(input, line) || line != "STUDENTS_V1")
    {
        throw std::runtime_error("Invalid student file header.");
    }

    std::vector<Student> loaded;
    std::set<std::string> ids;
    int lineNumber = 1;

    while (std::getline(input, line))
    {
        ++lineNumber;
        std::istringstream row(line);
        std::string id, name, hash, major;
        int year;

        if (!(row >> std::quoted(id)
                  >> std::quoted(name)
                  >> std::quoted(hash)
                  >> year
                  >> std::quoted(major)))
        {
            throw std::runtime_error(
                "Malformed student at line " +
                std::to_string(lineNumber));
        }

        row >> std::ws;

        if (!row.eof() || id.empty() || name.empty() ||
            hash.empty() || major.empty() || year < 1 ||
            !ids.insert(id).second)
        {
            throw std::runtime_error(
                "Invalid student at line " +
                std::to_string(lineNumber));
        }

        loaded.emplace_back(id, name, hash, year, major);
    }

    if (input.bad() || !input.eof())
    {
        throw std::runtime_error(
            "Failed to finish reading: " + filename);
    }

    students.swap(loaded);
}

// Validates records before opening the output file.
void FileStorage::saveAll(const std::vector<AttendanceRecord>& records,
                          const std::string& filename)
{
    std::ostringstream content;
    std::set<std::pair<std::string, std::string>> keys;
    content << "ATTENDANCE_V1\n";

    for (const AttendanceRecord& record : records)
    {
        std::string student = record.getStudentId();
        std::string session = record.getSessionId();

        if (student.empty() || session.empty() ||
            student.find_first_of("\r\n") != std::string::npos ||
            session.find_first_of("\r\n") != std::string::npos ||
            record.getCheckInTime() <= 0 ||
            !keys.insert({student, session}).second)
        {
            throw std::runtime_error(
                "Invalid or duplicate attendance record.");
        }

        content << std::quoted(student) << ' '
                << std::quoted(session) << ' '
                << record.getCheckInTime() << '\n';
    }

    std::ofstream output(filename);
    if (!output)
        throw std::runtime_error("Cannot open file for saving: " + filename);

    output << content.str();
    output.close();

    if (!output)
        throw std::runtime_error("Failed to finish saving: " + filename);
}

// Replaces the collection only after the whole file passes.
void FileStorage::loadAll(std::vector<AttendanceRecord>& records,
                          const std::string& filename)
{
    std::ifstream input(filename);
    if (!input)
        throw std::runtime_error("Cannot open file for loading: " + filename);

    std::string line;
    if (!std::getline(input, line) || line != "ATTENDANCE_V1")
        throw std::runtime_error("Invalid attendance file header.");

    std::vector<AttendanceRecord> loaded;
    std::set<std::pair<std::string, std::string>> keys;
    int lineNumber = 1;

    while (std::getline(input, line))
    {
        ++lineNumber;
        std::istringstream row(line);
        std::string student, session;
        std::time_t recordedTime;

        if (!(row >> std::quoted(student)
                  >> std::quoted(session)
                  >> recordedTime))
        {
            throw std::runtime_error("Malformed attendance at line " +
                                     std::to_string(lineNumber));
        }

        row >> std::ws;

        if (!row.eof() || student.empty() || session.empty() ||
            student.find_first_of("\r\n") != std::string::npos ||
            session.find_first_of("\r\n") != std::string::npos ||
            recordedTime <= 0 ||
            !keys.insert({student, session}).second)
        {
            throw std::runtime_error("Invalid attendance at line " +
                                     std::to_string(lineNumber));
        }

        loaded.emplace_back(student, session, recordedTime);
    }

    if (input.bad() || !input.eof())
        throw std::runtime_error("Failed to finish reading: " + filename);

    records.swap(loaded);
}