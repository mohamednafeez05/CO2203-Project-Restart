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

    TimeSlot(std::string day,
             int startTime,
             int endTime,
             std::string location);

    bool overlaps(const TimeSlot& other) const;

    bool operator==(const TimeSlot& other) const;

    std::string getDay() const;
    int getStartTime() const;
    int getEndTime() const;
    std::string getLocation() const;
};

#endif