#include "AttendanceRegister.h"

#include "Student.h"
#include "Course.h"
#include <stdexcept>
#include "FileStorage.h"

AttendanceRegister::AttendanceRegister()
{
}

bool AttendanceRegister::alreadyMarked(
    std::string studentId,
    std::string sessionId) const
{
    for (std::size_t i = 0; i < records.size(); i++)
    {
        if (records[i].getStudentId() == studentId &&
            records[i].getSessionId() == sessionId)
        {
            return true;
        }
    }

    return false;
}

void AttendanceRegister::markPresent(
    Student& student,
    Course& course,
    AttendanceSession& session)
{
    if (session.isOpen() == false)
    {
        throw std::runtime_error(
            "Attendance session is closed."
        );
    }

    if (course.isStudentEnrolled(student) == false)
    {
        throw std::runtime_error(
            "Student is not enrolled in this course."
        );
    }

    std::string studentId =
        student.getPersonId();

    if (alreadyMarked(
            studentId,
            session.getSessionId()))
    {
        throw std::runtime_error(
            "Duplicate attendance is not allowed."
        );
    }

    AttendanceRecord record(
        studentId,
        session.getSessionId()
    );

    records.push_back(record);
}

void AttendanceRegister::addCorrection(
    std::string studentId,
    std::string sessionId,
    std::string reason)
{
    CorrectionRecord correction(
        studentId,
        sessionId,
        reason
    );

    corrections.push_back(correction);
}

int AttendanceRegister::getPresentCount(
    std::string studentId) const
{
    int count = 0;

    for (std::size_t i = 0; i < records.size(); i++)
    {
        if (records[i].getStudentId()
            == studentId)
        {
            count++;
        }
    }

    return count;
}

double AttendanceRegister::getAttendancePercentage(
    std::string studentId,
    int totalSessions) const
{
    if (totalSessions == 0)
    {
        return 0.0;
    }

    int present =
        getPresentCount(studentId);

    return
        ((double)present / totalSessions)
        * 100.0;
}

const std::vector<AttendanceRecord>&
AttendanceRegister::getRecords() const
{
    return records;
}

const std::vector<CorrectionRecord>&
AttendanceRegister::getCorrections() const
{
    return corrections;
}

// Writes both attendance collections.
void AttendanceRegister::save(const std::string& attendanceFile,
                              const std::string& correctionFile) const
{
    if (attendanceFile == correctionFile)
    {
        throw std::invalid_argument(
            "Use different attendance and correction files.");
    }

    FileStorage::saveAll(records, attendanceFile);
    FileStorage::saveAll(corrections, correctionFile);
}

// Replaces both collections only after both files pass.
void AttendanceRegister::load(const std::string& attendanceFile,
                              const std::string& correctionFile)
{
    if (attendanceFile == correctionFile)
    {
        throw std::invalid_argument(
            "Use different attendance and correction files.");
    }

    std::vector<AttendanceRecord> loadedRecords;
    std::vector<CorrectionRecord> loadedCorrections;

    FileStorage::loadAll(loadedRecords, attendanceFile);
    FileStorage::loadAll(loadedCorrections, correctionFile);

    records.swap(loadedRecords);
    corrections.swap(loadedCorrections);
}