#include "Timetable.h"

using namespace std;

// Stores a reference to an existing slot.
void Timetable::addTimeSlot(const TimeSlot& slot)
{
    timeSlots.push_back(&slot);
}

// Creates an empty timetable.
Timetable::Timetable()
{
}

// Removes references to the supplied slot.
void Timetable::removeTimeSlot(const TimeSlot& slot)
{
    for (auto it = timeSlots.begin(); it != timeSlots.end(); )
    {
        if (*it == &slot)
        {
            it = timeSlots.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

bool Timetable::hasClashWith(
    const TimeSlot& other
) const
{
    for (const TimeSlot* slot : timeSlots)
    {
        if (*slot == other)
        {
            return true;
        }
    }

    return false;
}

ostream& operator<<(
    ostream& os,
    const Timetable& timetable
)
{
    os << "=== Weekly Timetable ===\n";

    if (timetable.timeSlots.empty())
    {
        os << "No courses scheduled.\n";
        return os;
    }

    for (const TimeSlot* slot : timetable.timeSlots)
    {
        os << slot->getDay()
           << " | "
           << slot->getStartTime()
           << " - "
           << slot->getEndTime()
           << " | "
           << slot->getLocation()
           << "\n";
    }

    return os;
}