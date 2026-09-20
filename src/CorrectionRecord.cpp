#include "CorrectionRecord.h"

CorrectionRecord::CorrectionRecord()
    : studentId(""),
      sessionId(""),
      lecturerId(""),
      reason(""),
      action(MARKED_PRESENT),
      correctionTime(0)
{
}

CorrectionRecord::CorrectionRecord(
    const std::string& studentId,
    const std::string& sessionId,
    const std::string& lecturerId,
    const std::string& reason,
    Action action)
    : studentId(studentId),
      sessionId(sessionId),
      lecturerId(lecturerId),
      reason(reason),
      action(action),
      correctionTime(std::time(nullptr))
{
}

std::string CorrectionRecord::getStudentId() const
{
    return studentId;
}

std::string CorrectionRecord::getSessionId() const
{
    return sessionId;
}

std::string CorrectionRecord::getLecturerId() const
{
    return lecturerId;
}

std::string CorrectionRecord::getReason() const
{
    return reason;
}

CorrectionRecord::Action CorrectionRecord::getAction() const
{
    return action;
}

std::time_t CorrectionRecord::getCorrectionTime() const
{
    return correctionTime;
}
