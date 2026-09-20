#include "Timetable.h"

using namespace std;

// Creates an empty timetable.
Timetable::Timetable()
{
}

// Stores a copy of the slot.
void Timetable::addTimeSlot(const TimeSlot& slot)
{
    timeSlots.push_back(slot);
}

// Removes the first stored slot that is identical to the supplied one.
// Only one is removed so that two different courses which happen to use
// identical slots do not remove each other's entries.
void Timetable::removeTimeSlot(const TimeSlot& slot)
{
    for (auto it = timeSlots.begin(); it != timeSlots.end(); ++it)
    {
        if (it->isSameSlot(slot))
        {
            timeSlots.erase(it);
            return;
        }
    }
}

bool Timetable::hasClashWith(const TimeSlot& other) const
{
    for (const TimeSlot& slot : timeSlots)
    {
        if (slot == other)
        {
            return true;
        }
    }

    return false;
}

ostream& operator<<(ostream& os, const Timetable& timetable)
{
    os << "=== Weekly Timetable ===\n";

    if (timetable.timeSlots.empty())
    {
        os << "No courses scheduled.\n";
        return os;
    }

    for (const TimeSlot& slot : timetable.timeSlots)
    {
        os << slot.getDay()
           << " | "
           << slot.getStartTime()
           << " - "
           << slot.getEndTime()
           << " | "
           << slot.getLocation()
           << "\n";
    }

    return os;
}
