#include "RotatingCodeCapture.h"

#include <iostream>
#include <cstdlib>
#include <ctime>

RotatingCodeCapture::RotatingCodeCapture()
{
    currentSession = 0;
    currentCode = "";
    active = false;
}

void RotatingCodeCapture::beginSession(
    AttendanceSession& session)
{
    currentSession = &session;

    if (currentSession->isOpen() == false)
    {
        currentSession->open();
    }

    active = true;

    generateCode();
}

void RotatingCodeCapture::generateCode()
{
    int code;

    code = 1000 + rand() % 9000;

    currentCode =
        std::to_string(code);
}

std::string
RotatingCodeCapture::getCurrentCode() const
{
    return currentCode;
}

bool RotatingCodeCapture::validateCode(
    std::string enteredCode) const
{
    if (enteredCode == currentCode)
    {
        return true;
    }

    return false;
}

bool RotatingCodeCapture::captureNext(
    std::string& studentId)
{
    if (active == false)
    {
        return false;
    }

    if (currentSession == 0)
    {
        return false;
    }

    if (currentSession->isOpen() == false)
    {
        return false;
    }

    std::string enteredCode;

    std::cout << "Student ID: ";
    std::cin >> studentId;

    std::cout << "Attendance code: ";
    std::cin >> enteredCode;

    if (validateCode(enteredCode))
    {
        generateCode();

        return true;
    }

    return false;
}

void RotatingCodeCapture::endSession()
{
    active = false;

    if (currentSession != 0)
    {
        currentSession->close();
    }

    currentSession = 0;
}