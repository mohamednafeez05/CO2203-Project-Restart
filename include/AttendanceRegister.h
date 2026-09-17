#ifndef ATTENDANCE_REGISTER_H
#define ATTENDANCE_REGISTER_H

#include <vector>
#include <string>

#include "AttendanceRecord.h"
#include "CorrectionRecord.h"
#include "AttendanceSession.h"

class Student;
class Course;

class AttendanceRegister
{
private:
    std::vector<AttendanceRecord> records;

    std::vector<CorrectionRecord> corrections;

    bool alreadyMarked(
        std::string studentId,
        std::string sessionId
    ) const;

public:
    AttendanceRegister();

    void markPresent(
        Student& student,
        Course& course,
        AttendanceSession& session
    );

    void addCorrection(
        std::string studentId,
        std::string sessionId,
        std::string reason
    );

    int getPresentCount(
        std::string studentId
    ) const;

    double getAttendancePercentage(
        std::string studentId,
        int totalSessions
    ) const;

    const std::vector<AttendanceRecord>&
    getRecords() const;

    const std::vector<CorrectionRecord>&
    getCorrections() const;
};

#endif