#include "RotatingCodeCapture.h"

#include <iostream>
#include <cstdlib>

RotatingCodeCapture::RotatingCodeCapture()
    : currentSession(nullptr),
      currentCode(""),
      active(false)
{
}

void RotatingCodeCapture::beginSession(
    AttendanceSession& session)
{
    // End any previous capture session before starting a new one.
    if (active)
    {
        endSession();
    }

    currentSession = &session;

    if (!currentSession->isOpen())
    {
        currentSession->open();
    }

    active = true;
    generateCode();
}

void RotatingCodeCapture::generateCode()
{
    const int code = 1000 + std::rand() % 9000;
    currentCode = std::to_string(code);
}

std::string RotatingCodeCapture::getCurrentCode() const
{
    return currentCode;
}

bool RotatingCodeCapture::validateCode(
    std::string enteredCode) const
{
    return enteredCode == currentCode;
}

bool RotatingCodeCapture::captureNext(
    std::string& studentId)
{
    if (!active || currentSession == nullptr)
    {
        return false;
    }

    if (!currentSession->isOpen())
    {
        return false;
    }

    std::string enteredCode;

    std::cout << "Student ID: ";
    std::cin >> studentId;

    std::cout << "Attendance code: ";
    std::cin >> enteredCode;

    if (!validateCode(enteredCode))
    {
        return false;
    }

    // Rotate the code after a successful capture.
    generateCode();
    return true;
}

void RotatingCodeCapture::endSession()
{
    active = false;

    if (currentSession != nullptr)
    {
        currentSession->close();
    }

    currentSession = nullptr;
    currentCode.clear();
}
