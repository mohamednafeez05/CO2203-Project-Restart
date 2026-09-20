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

#include <stdexcept>
void AttendanceSession::configure(const std::string& course, const std::string& lecturer, int slot, const std::vector<std::string>& students)
{ courseCode = course; lecturerId = lecturer; slotIndex = slot; roster = students; }
void AttendanceSession::restoreTimes(std::time_t start, std::time_t end)
{
    if (start <= 0 || end <= start) throw std::invalid_argument("Invalid session times.");
    startTime = start; expiryTime = end; active = false;
}
std::string AttendanceSession::getCourseCode() const { return courseCode; }
std::string AttendanceSession::getLecturerId() const { return lecturerId; }
int AttendanceSession::getSlotIndex() const { return slotIndex; }
const std::vector<std::string>& AttendanceSession::getRoster() const { return roster; }
