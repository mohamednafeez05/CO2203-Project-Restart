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

// Restores the original correction time.
CorrectionRecord::CorrectionRecord(std::string studentId,
                                   std::string sessionId,
                                   std::string reason,
                                   std::time_t recordedTime)
    : studentId(studentId),
      sessionId(sessionId),
      reason(reason),
      correctionTime(recordedTime)
{
}
#include <stdexcept>
CorrectionRecord::CorrectionRecord(const std::string& student, const std::string& session, const std::string& actor, const std::string& text, Action change, std::time_t at)
    : lecturerId(actor), action(change), studentId(student), sessionId(session), reason(text), correctionTime(at)
{
    if (actor.empty() || text.find_first_not_of(" \t\r\n") == std::string::npos || at <= 0 || (change != MARKED_PRESENT && change != REMOVED_PRESENT))
        throw std::invalid_argument("Invalid correction.");
}
std::string CorrectionRecord::getLecturerId() const { return lecturerId; }
CorrectionRecord::Action CorrectionRecord::getAction() const { return action; }
