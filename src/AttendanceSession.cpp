#include "AttendanceSession.h"

using namespace std;

// Initialises an empty, closed session.
AttendanceSession::AttendanceSession()
    : sessionId(""),
      timeSlot(nullptr),
      active(false),
      startTime(0),
      expiryTime(0),
      durationSeconds(defaultDurationSeconds)
{
}

AttendanceSession::AttendanceSession(
    const string& id,
    const TimeSlot* slot,
    int duration
)
    : sessionId(id),
      timeSlot(slot),
      active(false),
      startTime(0),
      expiryTime(0),
      durationSeconds(duration)
{
}

void AttendanceSession::open()
{
    active = true;

    startTime = time(nullptr);

    expiryTime = startTime + durationSeconds;
}

void AttendanceSession::close()
{
    active = false;
}

bool AttendanceSession::isExpired() const
{
    // A session that was never opened has not expired.
    if (startTime == 0)
    {
        return false;
    }

    return time(nullptr) >= expiryTime;
}

bool AttendanceSession::isOpen() const
{
    if (!active)
    {
        return false;
    }

    return !isExpired();
}

string AttendanceSession::getSessionId() const
{
    return sessionId;
}

const TimeSlot* AttendanceSession::getTimeSlot() const
{
    return timeSlot;
}

time_t AttendanceSession::getStartTime() const
{
    return startTime;
}

time_t AttendanceSession::getExpiryTime() const
{
    return expiryTime;
}

int AttendanceSession::getDurationSeconds() const
{
    return durationSeconds;
}
