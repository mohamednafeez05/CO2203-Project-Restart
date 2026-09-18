#ifndef ATTENDANCE_RECORD_H
#define ATTENDANCE_RECORD_H

#include <string>
#include <ctime>

class AttendanceRecord
{
private:
    std::string studentId;
    std::string sessionId;
    std::time_t checkInTime;

public:
    AttendanceRecord();

    AttendanceRecord(
        std::string studentId,
        std::string sessionId
    );
    // Restores the original check-in time.
    AttendanceRecord(std::string studentId,
                    std::string sessionId,
                    std::time_t recordedTime);
    std::string getStudentId() const;

    std::string getSessionId() const;

    std::time_t getCheckInTime() const;
};

#endif