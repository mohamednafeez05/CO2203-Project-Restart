#ifndef FILE_REPLAY_CAPTURE_H
#define FILE_REPLAY_CAPTURE_H

#include <string>
#include <fstream>

#include "AttendanceCapture.h"
#include "AttendanceSession.h"

// Replays attendance from a text file: one student ID per line.
// Useful for demonstrations and for bulk-loading attendance.
class FileReplayCapture :
    public AttendanceCapture
{
private:
    std::string fileName;

    std::ifstream inputFile;

    AttendanceSession* currentSession;

    bool active;

    // An ifstream cannot be copied, so neither can this class.
    FileReplayCapture(const FileReplayCapture&);
    FileReplayCapture& operator=(const FileReplayCapture&);

public:
    FileReplayCapture(
        const std::string& fileName
    );

    ~FileReplayCapture();

    // Throws std::runtime_error when the file cannot be opened.
    void beginSession(
        AttendanceSession& session
    );

    // Skips blank lines. Throws std::runtime_error only if the file
    // cannot be read. Trims spaces around the ID.
    bool captureNext(
        std::string& studentId
    );

    void endSession();
};

#endif
