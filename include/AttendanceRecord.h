#ifndef ATTENDANCE_RECORD_H
#define ATTENDANCE_RECORD_H

#include <string>
#include <ctime>

// One student's check-in for one session.
class AttendanceRecord
{
private:
    std::string studentId;
    std::string sessionId;
    std::time_t checkInTime;

public:
    AttendanceRecord();

    // Stamps the record with the current time.
    AttendanceRecord(
        const std::string& studentId,
        const std::string& sessionId
    );

    std::string getStudentId() const;

    std::string getSessionId() const;

    std::time_t getCheckInTime() const;
};

#endif
