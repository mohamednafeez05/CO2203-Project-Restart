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
    TimeSlot(const std::string& day,
             int startTime,
             int endTime,
             const std::string& location);

    std::string getDay() const;
    int getStartTime() const;
    int getEndTime() const;
    std::string getLocation() const;

    bool overlaps(const TimeSlot& other) const;

    bool operator==(const TimeSlot& other) const;
};

#endif