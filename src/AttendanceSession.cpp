#include "AttendanceSession.h"

using namespace std;

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

    expiryTime =
        startTime + durationSeconds;
}

void AttendanceSession::close()
{
    active = false;
}

bool AttendanceSession::isOpen() const
{
    if (!active)
    {
        return false;
    }

    if (time(nullptr) >= expiryTime)
    {
        return false;
    }

    return true;
}

string AttendanceSession::getId() const
{
    return sessionId;
}

const TimeSlot* AttendanceSession::getTimeSlot() const
{
    return timeSlot;
}