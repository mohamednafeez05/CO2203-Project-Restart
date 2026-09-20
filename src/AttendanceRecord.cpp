#include "AttendanceRecord.h"

AttendanceRecord::AttendanceRecord()
{
    studentId = "";
    sessionId = "";
    checkInTime = 0;
}

AttendanceRecord::AttendanceRecord(
    std::string studentId,
    std::string sessionId)
{
    this->studentId = studentId;
    this->sessionId = sessionId;

    checkInTime = std::time(0);
}

std::string AttendanceRecord::getStudentId() const
{
    return studentId;
}

std::string AttendanceRecord::getSessionId() const
{
    return sessionId;
}

std::time_t AttendanceRecord::getCheckInTime() const
{
    return checkInTime;
}

// Creates a record using its saved timestamp.
AttendanceRecord::AttendanceRecord(std::string studentId,
                                   std::string sessionId,
                                   std::time_t recordedTime)
    : studentId(studentId),
      sessionId(sessionId),
      checkInTime(recordedTime)
{
}
#include <stdexcept>
AttendanceRecord::AttendanceRecord(std::string student, std::string session, std::time_t at, std::string state, std::string source)
    : status(state), method(source), studentId(student), sessionId(session), checkInTime(at)
{
    if ((state != "present" && state != "late") || source.empty() || at <= 0)
        throw std::invalid_argument("Invalid attendance metadata.");
}
std::string AttendanceRecord::getStatus() const { return status; }
std::string AttendanceRecord::getMethod() const { return method; }
