#ifndef CORRECTION_RECORD_H
#define CORRECTION_RECORD_H

#include <string>
#include <ctime>

class CorrectionRecord
{
public:
    enum Action { NOTE_ONLY, MARKED_PRESENT, REMOVED_PRESENT };
private:
    std::string lecturerId;
    Action action = NOTE_ONLY;
    std::string studentId;
    std::string sessionId;
    std::string reason;
    std::time_t correctionTime;

public:
    CorrectionRecord();
    CorrectionRecord(const std::string& student, const std::string& session, const std::string& actor, const std::string& reason, Action action, std::time_t at = std::time(nullptr));
    std::string getLecturerId() const;
    Action getAction() const;

    CorrectionRecord(
        std::string studentId,
        std::string sessionId,
        std::string reason
    );
    // Restores a correction with its original timestamp.
    CorrectionRecord(std::string studentId,
                    std::string sessionId,
                    std::string reason,
                    std::time_t recordedTime);

    std::string getStudentId() const;

    std::string getSessionId() const;

    std::string getReason() const;

    std::time_t getCorrectionTime() const;
};

#endif