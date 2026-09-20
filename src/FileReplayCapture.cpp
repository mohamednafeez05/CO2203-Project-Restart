#include "FileReplayCapture.h"

#include <stdexcept>
#include <cctype>

FileReplayCapture::FileReplayCapture(
    std::string fileName)
    : fileName(fileName),
      currentSession(nullptr),
      active(false)
{
}

void FileReplayCapture::beginSession(
    AttendanceSession& session)
{
    // Make sure a previous replay is fully closed before starting again.
    if (active || inputFile.is_open())
    {
        endSession();
    }

    currentSession = &session;

    if (!currentSession->isOpen())
    {
        currentSession->open();
    }

    inputFile.open(fileName.c_str());

    if (!inputFile.is_open())
    {
        currentSession = nullptr;
        throw std::runtime_error(
            "Cannot open attendance replay file."
        );
    }

    active = true;
}

bool FileReplayCapture::captureNext(
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

    if (!std::getline(inputFile, studentId))
    {
        return false;
    }

    // A replay record must contain a non-whitespace student ID.
    bool hasNonWhitespace = false;
    for (char ch : studentId)
    {
        if (!std::isspace(static_cast<unsigned char>(ch)))
        {
            hasNonWhitespace = true;
            break;
        }
    }

    if (!hasNonWhitespace)
    {
        throw std::runtime_error(
            "Malformed attendance record."
        );
    }

    return true;
}

void FileReplayCapture::endSession()
{
    if (inputFile.is_open())
    {
        inputFile.close();
    }

    active = false;

    if (currentSession != nullptr)
    {
        currentSession->close();
    }

    currentSession = nullptr;
}
