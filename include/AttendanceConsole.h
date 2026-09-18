#ifndef ATTENDANCE_CONSOLE_H
#define ATTENDANCE_CONSOLE_H

#include "StudentHistoryStore.h"
#include <string>

class AttendanceConsole
{
private:
    StudentHistoryStore data;
    std::string dataDirectory;

    void showRecords(const std::string& studentId) const;
    void showCorrections(const std::string& studentId) const;
    void showSummary() const;
    void showStudents() const;
    bool showProfile(const std::string& id) const;

public:
    explicit AttendanceConsole(const std::string& directory);
    void run();
};

#endif