#include "CorrectionRecord.h"

CorrectionRecord::CorrectionRecord()
{
    studentId = "";
    sessionId = "";
    reason = "";
    correctionTime = 0;
}

CorrectionRecord::CorrectionRecord(
    std::string studentId,
    std::string sessionId,
    std::string reason)
{
    this->studentId = studentId;
    this->sessionId = sessionId;
    this->reason = reason;

    correctionTime = std::time(0);
}

std::string CorrectionRecord::getStudentId() const
{
    return studentId;
}

std::string CorrectionRecord::getSessionId() const
{
    return sessionId;
}

std::string CorrectionRecord::getReason() const
{
    return reason;
}

std::time_t CorrectionRecord::getCorrectionTime() const
{
    return correctionTime;
}