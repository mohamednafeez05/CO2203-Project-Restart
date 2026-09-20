#ifndef ROTATING_CODE_CAPTURE_H
#define ROTATING_CODE_CAPTURE_H

#include <string>
#include <ctime>

#include "AttendanceCapture.h"
#include "AttendanceSession.h"

// Interactive capture (Option A in the design rationale).
// The lecturer's screen shows a short random code. A student proves they
// are in the room by typing their ID and the code currently on screen.
// The code changes after every successful check-in and also expires after
// a short time, so a code cannot be passed around to absent friends.
class RotatingCodeCapture :
    public AttendanceCapture
{
private:
    AttendanceSession* currentSession;

    std::string currentCode;

    std::time_t codeExpiry;

    int codeLifetimeSeconds;

    bool active;

    // Holds a pointer to a session, so copying would be unsafe.
    RotatingCodeCapture(const RotatingCodeCapture&);
    RotatingCodeCapture& operator=(const RotatingCodeCapture&);

public:
    static const int defaultCodeLifetimeSeconds = 60;

    RotatingCodeCapture(
        int codeLifetimeSeconds = defaultCodeLifetimeSeconds
    );

    void beginSession(
        AttendanceSession& session
    );

    // Asks the student for their ID and the code (console).
    // Keeps asking after a wrong code. Returns false only when the
    // session has ended or the input stream is closed.
    bool captureNext(
        std::string& studentId
    );

    void endSession();

    // Creates a new random 4-digit code and restarts its expiry timer.
    void regenerateCode();

    std::string getCurrentCode() const;

    // True when the code is correct AND has not expired.
    bool validateCode(
        const std::string& enteredCode
    ) const;

    bool isCodeExpired() const;

    // Checks one check-in attempt without using the console.
    // Returns true (and rotates the code) when the code is valid,
    // false when the session is not open or the code is wrong/expired.
    bool submitCode(
        const std::string& enteredCode
    );
};

#endif
