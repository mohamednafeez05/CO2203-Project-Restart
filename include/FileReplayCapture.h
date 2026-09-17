#ifndef FILE_REPLAY_CAPTURE_H
#define FILE_REPLAY_CAPTURE_H

#include <string>
#include <fstream>

#include "AttendanceCapture.h"
#include "AttendanceSession.h"

class FileReplayCapture :
    public AttendanceCapture
{
private:
    std::string fileName;

    std::ifstream inputFile;

    AttendanceSession* currentSession;

    bool active;

public:
    FileReplayCapture(
        std::string fileName
    );

    void beginSession(
        AttendanceSession& session
    );

    bool captureNext(
        std::string& studentId
    );

    void endSession();
};

#endif