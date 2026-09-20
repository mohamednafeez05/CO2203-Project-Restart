#ifndef ATTENDANCE_CAPTURE_H
#define ATTENDANCE_CAPTURE_H

#include <string>

class AttendanceSession;

// Abstract "way of collecting attendance".
// AttendanceRegister and the menus only know this interface, so a new
// capture method can be added without changing them (dependency inversion).
class AttendanceCapture
{
public:
    // Starts collecting for the session (opens it if necessary).
    virtual void beginSession(
        AttendanceSession& session
    ) = 0;

    // Fetches the next student who checked in.
    // Returns true and fills studentId when a student was captured.
    // Returns false when there is nobody left / the session is over.
    virtual bool captureNext(
        std::string& studentId
    ) = 0;

    // Stops collecting and closes the session.
    virtual void endSession() = 0;

    virtual ~AttendanceCapture()
    {
    }
};

#endif
