#ifndef TIMESLOT_H
#define TIMESLOT_H

#include <string>

class TimeSlot
{
private:
    std::string day;
    int startTime;
    int endTime;
    std::string location;

public:
    TimeSlot();

    // Initialises the slot details.
    TimeSlot(const std::string& day,
             int startTime,
             int endTime,
             const std::string& location);

    // True when both slots are on the same day and their times overlap.
    bool overlaps(const TimeSlot& other) const;

    // Clash detection: two slots are "equal" when they overlap.
    bool operator==(const TimeSlot& other) const;

    // True when every field is identical (used to find a slot to remove).
    bool isSameSlot(const TimeSlot& other) const;

    std::string getDay() const;
    int getStartTime() const;
    int getEndTime() const;
    std::string getLocation() const;
};

#endif
