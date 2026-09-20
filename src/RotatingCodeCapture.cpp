#include "RotatingCodeCapture.h"

#include <iostream>
#include <cstdlib>
#include <random>

// A random 4-digit code, 1000 to 9999.
static int randomFourDigitCode()
{
    static std::mt19937 engine(std::random_device{}());
    std::uniform_int_distribution<int> dist(1000, 9999);
    return dist(engine);
}

RotatingCodeCapture::RotatingCodeCapture(int lifetime)
    : currentSession(nullptr),
      currentCode(""),
      codeExpiry(0),
      codeLifetimeSeconds(lifetime > 0 ? lifetime
                                       : defaultCodeLifetimeSeconds),
      active(false)
{
}

void RotatingCodeCapture::beginSession(
    AttendanceSession& session)
{
    // End any previous capture before starting a new one.
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
    regenerateCode();
}

void RotatingCodeCapture::regenerateCode()
{
    currentCode = std::to_string(randomFourDigitCode());
    codeExpiry = std::time(nullptr) + codeLifetimeSeconds;
}

std::string RotatingCodeCapture::getCurrentCode() const
{
    return currentCode;
}

bool RotatingCodeCapture::isCodeExpired() const
{
    return std::time(nullptr) >= codeExpiry;
}

bool RotatingCodeCapture::validateCode(
    const std::string& enteredCode) const
{
    if (currentCode.empty() || isCodeExpired())
    {
        return false;
    }

    return enteredCode == currentCode;
}

bool RotatingCodeCapture::submitCode(
    const std::string& enteredCode)
{
    if (!active ||
        currentSession == nullptr ||
        !currentSession->isOpen())
    {
        return false;
    }

    if (!validateCode(enteredCode))
    {
        return false;
    }

    // A used code is thrown away, so it cannot be shared.
    regenerateCode();
    return true;
}

bool RotatingCodeCapture::captureNext(
    std::string& studentId)
{
    while (active &&
           currentSession != nullptr &&
           currentSession->isOpen())
    {
        std::string enteredCode;

        std::cout << "Student ID (or END to finish): ";

        if (!(std::cin >> studentId))
        {
            return false;   // input closed
        }

        if (studentId == "END")
        {
            return false;
        }

        std::cout << "Attendance code: ";

        if (!(std::cin >> enteredCode))
        {
            return false;   // input closed
        }

        if (submitCode(enteredCode))
        {
            return true;
        }

        // Wrong or expired code: tell the student and ask again,
        // instead of silently ending the whole session.
        if (isCodeExpired())
        {
            regenerateCode();
            std::cout << "That code expired. "
                         "Ask the lecturer for the new code.\n";
        }
        else
        {
            std::cout << "Incorrect code. Try again.\n";
        }
    }

    return false;
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
    codeExpiry = 0;
}
