#include "FileReplayCapture.h"

#include <stdexcept>

FileReplayCapture::FileReplayCapture(
    std::string fileName)
{
    this->fileName = fileName;

    currentSession = 0;

    active = false;
}

void FileReplayCapture::beginSession(
    AttendanceSession& session)
{
    currentSession = &session;

    if (currentSession->isOpen() == false)
    {
        currentSession->open();
    }

    inputFile.open(fileName.c_str());

    if (inputFile.is_open() == false)
    {
        throw std::runtime_error(
            "Cannot open attendance replay file."
        );
    }

    active = true;
}

bool FileReplayCapture::captureNext(
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

    if (std::getline(inputFile, studentId))
    {
        if (studentId == "")
        {
            throw std::runtime_error(
                "Malformed attendance record."
            );
        }

        return true;
    }

    return false;
}

void FileReplayCapture::endSession()
{
    if (inputFile.is_open())
    {
        inputFile.close();
    }

    active = false;

    currentSession = 0;
}