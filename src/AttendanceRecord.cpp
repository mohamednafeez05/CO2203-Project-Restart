#include "AttendanceRecord.h"

AttendanceRecord::AttendanceRecord()
    : studentId(""),
      sessionId(""),
      checkInTime(0)
{
}

AttendanceRecord::AttendanceRecord(
    const std::string& studentId,
    const std::string& sessionId)
    : studentId(studentId),
      sessionId(sessionId),
      checkInTime(std::time(nullptr))
{
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
