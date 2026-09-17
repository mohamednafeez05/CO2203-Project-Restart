#ifndef ATTENDANCE_CONSOLE_H
#define ATTENDANCE_CONSOLE_H

#include "AttendanceRegister.h"
#include <string>

class AttendanceConsole
{
private:
    AttendanceRegister attendance;
    std::string attendanceFile;
    std::string correctionFile;

    void showRecords(const std::string& studentId) const;
    void showCorrections(const std::string& studentId) const;
    void showSummary() const;

public:
    explicit AttendanceConsole(const std::string& directory);
    void run();
};

#endif