#ifndef CORRECTION_RECORD_H
#define CORRECTION_RECORD_H

#include <string>
#include <ctime>

class CorrectionRecord
{
private:
    std::string studentId;
    std::string sessionId;
    std::string reason;
    std::time_t correctionTime;

public:
    CorrectionRecord();

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