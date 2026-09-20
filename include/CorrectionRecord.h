#ifndef CORRECTION_RECORD_H
#define CORRECTION_RECORD_H

#include <string>
#include <ctime>

// Audit entry: a lecturer manually changed a student's attendance.
// Corrections are never deleted, so the history can always be reviewed.
class CorrectionRecord
{
public:
    enum Action
    {
        MARKED_PRESENT,   // lecturer added a missing attendance record
        REMOVED_PRESENT   // lecturer removed a wrong attendance record
    };

private:
    std::string studentId;
    std::string sessionId;
    std::string lecturerId;
    std::string reason;
    Action action;
    std::time_t correctionTime;

public:
    CorrectionRecord();

    // Stamps the correction with the current time.
    CorrectionRecord(
        const std::string& studentId,
        const std::string& sessionId,
        const std::string& lecturerId,
        const std::string& reason,
        Action action
    );

    std::string getStudentId() const;

    std::string getSessionId() const;

    std::string getLecturerId() const;

    std::string getReason() const;

    Action getAction() const;

    std::time_t getCorrectionTime() const;
};

#endif
