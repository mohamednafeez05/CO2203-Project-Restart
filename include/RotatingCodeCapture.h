#ifndef ROTATING_CODE_CAPTURE_H
#define ROTATING_CODE_CAPTURE_H

#include <string>

#include "AttendanceCapture.h"
#include "AttendanceSession.h"

class RotatingCodeCapture :
    public AttendanceCapture
{
private:
    AttendanceSession* currentSession;

    std::string currentCode;

    bool active;

public:
    RotatingCodeCapture();

    void beginSession(
        AttendanceSession& session
    );

    bool captureNext(
        std::string& studentId
    );

    void endSession();

    void generateCode();

    std::string getCurrentCode() const;

    bool validateCode(
        std::string enteredCode
    ) const;
};

#endif