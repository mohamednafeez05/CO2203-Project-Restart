#include "Timetable.h"

using namespace std;

void Timetable::addTimeSlot(const TimeSlot* slot)
{
    if (slot != nullptr)
    {
        timeSlots.push_back(slot);
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