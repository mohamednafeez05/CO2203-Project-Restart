#include "TimeSlot.h"

using namespace std;

// Initialises an empty slot.
TimeSlot::TimeSlot() : startTime(0), endTime(0)
{
}

TimeSlot::TimeSlot(const string& day,
                   int startTime,
                   int endTime,
                   const string& location)
    : day(day),
      startTime(startTime),
      endTime(endTime),
      location(location)
{
}

string TimeSlot::getDay() const
{
    return day;
}

int TimeSlot::getStartTime() const
{
    return startTime;
}

int TimeSlot::getEndTime() const
{
    return endTime;
}

string TimeSlot::getLocation() const
{
    return location;
}

// Different days never clash. On the same day, the slots clash when
// each one starts before the other one ends. A slot that ends exactly
// when another begins (e.g. 900-1000 and 1000-1100) does NOT clash.
bool TimeSlot::overlaps(const TimeSlot& other) const
{
    if (day != other.day)
    {
        return false;
    }

    return startTime < other.endTime &&
           other.startTime < endTime;
}

bool TimeSlot::operator==(const TimeSlot& other) const
{
    return overlaps(other);
}

bool TimeSlot::isSameSlot(const TimeSlot& other) const
{
    return day == other.day &&
           startTime == other.startTime &&
           endTime == other.endTime &&
           location == other.location;
}
