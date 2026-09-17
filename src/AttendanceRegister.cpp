#include "AttendanceRegister.h"

#include "Student.h"
#include "Course.h"

#include <stdexcept>

AttendanceRegister::AttendanceRegister()
{
}

bool AttendanceRegister::alreadyMarked(
    std::string studentId,
    std::string sessionId) const
{
    for (int i = 0; i < records.size(); i++)
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
        student.getId();

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

    for (int i = 0; i < records.size(); i++)
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