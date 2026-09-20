#include "FileReplayCapture.h"

#include <stdexcept>
#include <cctype>

// Removes spaces, tabs and carriage returns from both ends of the text.
// (A file saved on Windows leaves a '\r' at the end of each line.)
static std::string trim(const std::string& text)
{
    std::size_t first = 0;

    while (first < text.size() &&
           std::isspace(static_cast<unsigned char>(text[first])))
    {
        first++;
    }

    std::size_t last = text.size();

    while (last > first &&
           std::isspace(static_cast<unsigned char>(text[last - 1])))
    {
        last--;
    }

    return text.substr(first, last - first);
}

FileReplayCapture::FileReplayCapture(
    const std::string& fileName)
    : fileName(fileName),
      currentSession(nullptr),
      active(false)
{
}

FileReplayCapture::~FileReplayCapture()
{
    // Make sure the file is closed even if the caller forgot.
    if (inputFile.is_open())
    {
        inputFile.close();
    }
}

void FileReplayCapture::beginSession(
    AttendanceSession& session)
{
    // Finish any previous replay before starting a new one.
    if (active || inputFile.is_open())
    {
        endSession();
    }

    inputFile.clear();
    inputFile.open(fileName.c_str());

    if (!inputFile.is_open())
    {
        throw std::runtime_error(
            "Cannot open attendance replay file.");
    }

    currentSession = &session;

    if (!currentSession->isOpen())
    {
        currentSession->open();
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

    std::string line;

    // Skip blank lines so a trailing newline is not an error.
    while (std::getline(inputFile, line))
    {
        line = trim(line);

        if (!line.empty())
        {
            studentId = line;
            return true;
        }
    }

    // getline failed: either a normal end of file or a real read error.
    if (inputFile.bad())
    {
        throw std::runtime_error(
            "Failed to read attendance replay file.");
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

    if (currentSession != nullptr)
    {
        currentSession->close();
    }

    currentSession = nullptr;
}
