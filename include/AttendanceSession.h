#ifndef ATTENDANCE_SESSION_H
#define ATTENDANCE_SESSION_H

#include <string>
#include <ctime>
#include <vector>
#include "TimeSlot.h"

// One specific class meeting during which attendance can be taken.
class AttendanceSession
{
private:
    std::string courseCode, lecturerId;
    int slotIndex = 0;
    std::vector<std::string> roster;
    std::string sessionId;
    const TimeSlot* timeSlot;

    bool active;

    std::time_t startTime;
    std::time_t expiryTime;

    int durationSeconds;

public:
    void configure(const std::string& course, const std::string& lecturer, int slot, const std::vector<std::string>& students);
    void restoreTimes(std::time_t start, std::time_t end);
    std::string getCourseCode() const;
    std::string getLecturerId() const;
    int getSlotIndex() const;
    const std::vector<std::string>& getRoster() const;

    static const int defaultDurationSeconds = 600;

    // Creates an empty, closed session.
    AttendanceSession();

    // Creates a closed session. Call open() to start the clock.
    AttendanceSession(
        const std::string& sessionId,
        const TimeSlot* timeSlot,
        int durationSeconds = defaultDurationSeconds
    );

    void open();

    void close();

    // True only while the session was opened, not closed, and not expired.
    bool isOpen() const;

    // True when the session was opened and its time has run out.
    bool isExpired() const;

    std::string getSessionId() const;

    const TimeSlot* getTimeSlot() const;

    std::time_t getStartTime() const;

    std::time_t getExpiryTime() const;

    int getDurationSeconds() const;
};

#endif
