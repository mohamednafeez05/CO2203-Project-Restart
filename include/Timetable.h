#ifndef TIMETABLE_H
#define TIMETABLE_H

#include <vector>
#include <iostream>
#include "TimeSlot.h"

// A student's weekly timetable.
//
// The timetable keeps its own COPY of every slot it is given. It does not
// keep pointers into Course::slots, because a std::vector can move its
// elements when it grows, which would leave such pointers dangling.
// The Course still owns the "real" slots; the timetable only remembers
// when the student has to be somewhere.
class Timetable
{
private:
    std::vector<TimeSlot> timeSlots;

public:
    Timetable();

    void addTimeSlot(const TimeSlot& slot);

    // Removes the slot with exactly the same day, times and location.
    // Does nothing when the slot is not in the timetable.
    void removeTimeSlot(const TimeSlot& slot);

    bool hasClashWith(const TimeSlot& other) const;

    friend std::ostream& operator<<(
        std::ostream& os,
        const Timetable& timetable
    );
};

#endif
