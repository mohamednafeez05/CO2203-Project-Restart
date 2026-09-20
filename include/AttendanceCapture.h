#ifndef ATTENDANCE_CAPTURE_H
#define ATTENDANCE_CAPTURE_H

#include <string>

class AttendanceSession;

class AttendanceCapture
{
public:
    virtual bool acceptToken(const std::string&) { return false; }
    virtual std::string token() const { return ""; }
    virtual void beginSession(
        AttendanceSession& session
    ) = 0;

    virtual bool captureNext(
        std::string& studentId
    ) = 0;

    virtual void endSession() = 0;

    virtual ~AttendanceCapture()
    {
    }
};

#endif