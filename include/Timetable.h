#ifndef TIMETABLE_H
#define TIMETABLE_H

#include <vector>
#include <iostream>
#include "TimeSlot.h"

class Timetable
{
private:
    std::vector<const TimeSlot*> timeSlots;

public:
    Timetable() = default;

    void addTimeSlot(const TimeSlot* slot);

    bool hasClashWith(const TimeSlot& other) const;

    friend std::ostream& operator<<(
        std::ostream& os,
        const Timetable& timetable
    );
};

#endif