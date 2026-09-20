#include "AttendanceRegister.h"

#include "Student.h"
#include "Course.h"
#include "Lecturer.h"

#include "SessionClosedException.h"
#include "NotEnrolledException.h"
#include "DuplicateAttendanceException.h"

#include <stdexcept>
#include <cctype>

// Students need at least 80% attendance to be eligible by default.
double AttendanceRegister::eligibilityThreshold = 80.0;

// ---------------------------------------------------------------------
// Small helper: true when the text is empty or only whitespace.
// ---------------------------------------------------------------------
static bool isBlank(const std::string& text)
{
    for (char ch : text)
    {
        if (!std::isspace(static_cast<unsigned char>(ch)))
        {
            return false;
        }
    }

    return true;
}

// ---------------------------------------------------------------------
// Construction and the Rule of Three
// ---------------------------------------------------------------------
AttendanceRegister::AttendanceRegister()
{
}

// The register owns its vectors by value, so every copy below is a
// full, independent (deep) copy. Nothing is shared between registers.
AttendanceRegister::~AttendanceRegister()
{
}

AttendanceRegister::AttendanceRegister(
    const AttendanceRegister& other)
    : sessions(other.sessions),
      records(other.records),
      corrections(other.corrections)
{
}

AttendanceRegister& AttendanceRegister::operator=(
    const AttendanceRegister& other)
{
    // Guard against self-assignment.
    if (this != &other)
    {
        sessions = other.sessions;
        records = other.records;
        corrections = other.corrections;
    }

    return *this;
}

// ---------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------
bool AttendanceRegister::alreadyMarked(
    const std::string& studentId,
    const std::string& sessionId) const
{
    for (const AttendanceRecord& record : records)
    {
        if (record.getStudentId() == studentId &&
            record.getSessionId() == sessionId)
        {
            return true;
        }
    }

    return false;
}

bool AttendanceRegister::sessionExists(
    const std::string& sessionId) const
{
    for (const AttendanceSession& session : sessions)
    {
        if (session.getSessionId() == sessionId)
        {
            return true;
        }
    }

    return false;
}

// ---------------------------------------------------------------------
// Sessions
// ---------------------------------------------------------------------
void AttendanceRegister::addSession(
    const AttendanceSession& session)
{
    if (!sessionExists(session.getSessionId()))
    {
        sessions.push_back(session);
    }
}

int AttendanceRegister::getSessionCount() const
{
    return static_cast<int>(sessions.size());
}

// ---------------------------------------------------------------------
// Taking attendance
// ---------------------------------------------------------------------
void AttendanceRegister::markPresent(
    Student& student,
    Course& course,
    AttendanceSession& session)
{
    if (!session.isOpen())
    {
        throw SessionClosedException();
    }

    if (!course.isStudentEnrolled(student))
    {
        throw NotEnrolledException();
    }

    const std::string studentId = student.getPersonId();

    if (alreadyMarked(studentId, session.getSessionId()))
    {
        throw DuplicateAttendanceException();
    }

    // Make sure this session counts towards the percentage.
    addSession(session);

    records.push_back(
        AttendanceRecord(studentId, session.getSessionId()));
}

// ---------------------------------------------------------------------
// Lecturer corrections
// ---------------------------------------------------------------------
void AttendanceRegister::addCorrection(
    Student& student,
    Course& course,
    const std::string& sessionId,
    Lecturer& lecturer,
    const std::string& reason)
{
    if (!lecturer.isAssignedTo(course))
    {
        throw std::logic_error(
            "Lecturer is not assigned to this course.");
    }

    if (isBlank(reason))
    {
        throw std::invalid_argument(
            "A correction needs a reason.");
    }

    if (!sessionExists(sessionId))
    {
        throw std::invalid_argument(
            "Unknown attendance session.");
    }

    if (!course.isStudentEnrolled(student))
    {
        throw NotEnrolledException();
    }

    const std::string studentId = student.getPersonId();

    if (alreadyMarked(studentId, sessionId))
    {
        throw DuplicateAttendanceException();
    }

    records.push_back(AttendanceRecord(studentId, sessionId));

    corrections.push_back(CorrectionRecord(
        studentId,
        sessionId,
        lecturer.getPersonId(),
        reason,
        CorrectionRecord::MARKED_PRESENT));
}

void AttendanceRegister::removeAttendance(
    Student& student,
    Course& course,
    const std::string& sessionId,
    Lecturer& lecturer,
    const std::string& reason)
{
    if (!lecturer.isAssignedTo(course))
    {
        throw std::logic_error(
            "Lecturer is not assigned to this course.");
    }

    if (isBlank(reason))
    {
        throw std::invalid_argument(
            "A correction needs a reason.");
    }

    const std::string studentId = student.getPersonId();

    for (auto it = records.begin(); it != records.end(); ++it)
    {
        if (it->getStudentId() == studentId &&
            it->getSessionId() == sessionId)
        {
            records.erase(it);

            corrections.push_back(CorrectionRecord(
                studentId,
                sessionId,
                lecturer.getPersonId(),
                reason,
                CorrectionRecord::REMOVED_PRESENT));

            return;
        }
    }

    throw std::invalid_argument(
        "No attendance record exists to remove.");
}

// ---------------------------------------------------------------------
// Reports
// ---------------------------------------------------------------------
int AttendanceRegister::getPresentCount(
    const std::string& studentId) const
{
    int count = 0;

    for (const AttendanceRecord& record : records)
    {
        if (record.getStudentId() == studentId)
        {
            count++;
        }
    }

    return count;
}

double AttendanceRegister::getAttendancePercentage(
    const std::string& studentId,
    int totalSessions) const
{
    if (totalSessions <= 0)
    {
        return 0.0;
    }

    const int present = getPresentCount(studentId);

    return (static_cast<double>(present) / totalSessions) * 100.0;
}

double AttendanceRegister::getPercentage(
    const Student& student) const
{
    return getAttendancePercentage(
        student.getPersonId(),
        getSessionCount());
}

double AttendanceRegister::getPercentage(
    const Student& student,
    const Course& course) const
{
    // A student who is not on the course has no attendance to report.
    if (!course.isStudentEnrolled(student))
    {
        return 0.0;
    }

    return getPercentage(student);
}

std::vector<Student*> AttendanceRegister::getEligibilityReport(
    const Course& course) const
{
    std::vector<Student*> eligible;

    for (Student* student : course.getEnrolledStudents())
    {
        if (student != nullptr &&
            getPercentage(*student) >= eligibilityThreshold)
        {
            eligible.push_back(student);
        }
    }

    return eligible;
}

double AttendanceRegister::getEligibilityThreshold()
{
    return eligibilityThreshold;
}

void AttendanceRegister::setEligibilityThreshold(double percent)
{
    if (percent < 0.0 || percent > 100.0)
    {
        throw std::invalid_argument(
            "Threshold must be between 0 and 100.");
    }

    eligibilityThreshold = percent;
}

const std::vector<AttendanceSession>&
AttendanceRegister::getSessions() const
{
    return sessions;
}

const std::vector<AttendanceRecord>&
AttendanceRegister::getRecords() const
{
    return records;
}

const std::vector<CorrectionRecord>&
AttendanceRegister::getCorrections() const
{
    return corrections;
}
