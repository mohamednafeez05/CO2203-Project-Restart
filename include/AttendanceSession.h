#ifndef ATTENDANCE_SESSION_H
#define ATTENDANCE_SESSION_H

#include <string>
#include <ctime>
#include "TimeSlot.h"

class AttendanceSession
{
private:
    std::string sessionId;
    const TimeSlot* timeSlot;

    bool active;

    std::time_t startTime;
    std::time_t expiryTime;

    int durationSeconds;

public:
    static const int defaultDurationSeconds = 600;

    AttendanceSession(
        const std::string& id,
        const TimeSlot* slot,
        int duration = defaultDurationSeconds
    );

    void open();

    void close();

    bool isOpen() const;

    std::string getId() const;

    const TimeSlot* getTimeSlot() const;
};

#endif