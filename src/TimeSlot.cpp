#include <algorithm>
#include <cctype>
#include <stdexcept>
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
    std::string key = day;
    std::transform(key.begin(), key.end(), key.begin(), [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    const std::string days[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    bool found = false;
    for (const auto& canonical : days)
    {
        std::string lower = canonical;
        std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
        if (key == lower || key == lower.substr(0, 3)) { this->day = canonical; found = true; break; }
    }
    if (!found) throw std::invalid_argument("Use a weekday name or three-letter abbreviation.");
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