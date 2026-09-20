#include "Lecturer.h"
#include "AttendanceCapture.h"
#include "SessionClosedException.h"
#include "NotEnrolledException.h"
#include "DuplicateAttendanceException.h"
#include <algorithm>
#include <set>
#include "AttendanceRegister.h"
#include "Student.h"
#include "Course.h"
#include <stdexcept>
#include "FileStorage.h"
#include "SafeFile.h"

AttendanceRegister::AttendanceRegister()
{
}

bool AttendanceRegister::alreadyMarked(
    std::string studentId,
    std::string sessionId) const
{
    for (std::size_t i = 0; i < records.size(); i++)
    {
        if (records[i].getStudentId() == studentId &&
            records[i].getSessionId() == sessionId)
        {
            return true;
        }
    }

    return false;
}

void AttendanceRegister::markPresent(Student& student, Course& course, AttendanceSession& session)
{ markCaptured(student, course, session, "legacy"); }

void AttendanceRegister::markCaptured(Student& student, Course& course, AttendanceSession& session, const std::string& method)
{
    if (!session.isOpen()) throw SessionClosedException();
    if (!course.isStudentEnrolled(student)) throw NotEnrolledException();
    bool matches = false;
    for (const TimeSlot& slot : course.getSlots()) if (&slot == session.getTimeSlot()) matches = true;
    if (!matches) throw std::invalid_argument("Session slot does not belong to this course.");
    if (alreadyMarked(student.getPersonId(), session.getSessionId()) || isPresent(student.getPersonId(), session.getSessionId()))
        throw DuplicateAttendanceException();
    const auto now = std::time(nullptr);
    records.emplace_back(student.getPersonId(), session.getSessionId(), now,
                         now - session.getStartTime() > 300 ? "late" : "present", method);
}

bool AttendanceRegister::isPresent(const std::string& studentId, const std::string& sessionId) const
{
    bool present = alreadyMarked(studentId, sessionId);
    for (const auto& note : corrections)
        if (note.getStudentId() == studentId && note.getSessionId() == sessionId && note.getAction() != CorrectionRecord::NOTE_ONLY)
            present = note.getAction() == CorrectionRecord::MARKED_PRESENT;
    return present;
}

void AttendanceRegister::correct(Student& student, Course& course, AttendanceSession& session, Lecturer& actor, const std::string& reason, bool present)
{
    if (course.getLecturer() != &actor) throw std::logic_error("Lecturer does not own this course.");
    if (session.getCourseCode() != course.getCourseCode()) throw std::logic_error("Wrong session course.");
    const auto& roster = session.getRoster();
    if (std::find(roster.begin(), roster.end(), student.getPersonId()) == roster.end()) throw NotEnrolledException();
    if (isPresent(student.getPersonId(), session.getSessionId()) == present)
        throw std::logic_error("Correction would not change attendance.");
    corrections.emplace_back(student.getPersonId(), session.getSessionId(), actor.getPersonId(), reason,
        present ? CorrectionRecord::MARKED_PRESENT : CorrectionRecord::REMOVED_PRESENT);
}

// The capture loop depends only on the abstract interface.
void AttendanceRegister::capture(AttendanceCapture& source, Course& course, AttendanceSession& session, const std::string& method, std::ostream& output)
{
    if (!session.isOpen()) throw SessionClosedException();
    source.beginSession(session);
    try
    {
        std::string id;
        while (source.captureNext(id))
        {
            Student* student = nullptr;
            for (auto* item : course.getEnrolledStudents()) if (item->getPersonId() == id) student = item;
            try
            {
                if (!student) throw NotEnrolledException();
                const auto& roster = session.getRoster();
                if (!roster.empty() && std::find(roster.begin(), roster.end(), id) == roster.end()) throw NotEnrolledException();
                markCaptured(*student, course, session, method);
                output << "Recorded: " << id << '\n';
            }
            catch (const std::exception& error) { output << "Rejected " << id << ": " << error.what() << '\n'; }
        }
    }
    catch (...) { source.endSession(); throw; }
    source.endSession();
}

void AttendanceRegister::addCorrection(
    std::string studentId,
    std::string sessionId,
    std::string reason)
{
    CorrectionRecord correction(
        studentId,
        sessionId,
        reason
    );

    corrections.push_back(correction);
}

int AttendanceRegister::getPresentCount(
    std::string studentId) const
{
    std::set<std::string> ids;
    for (const auto& record : records) if (record.getStudentId() == studentId) ids.insert(record.getSessionId());
    for (const auto& note : corrections) if (note.getStudentId() == studentId) ids.insert(note.getSessionId());
    int count = 0;
    for (const auto& id : ids) if (isPresent(studentId, id)) ++count;
    return count;
}

double AttendanceRegister::getAttendancePercentage(
    std::string studentId,
    int totalSessions) const
{
    if (totalSessions == 0)
    {
        return 0.0;
    }

    int present =
        getPresentCount(studentId);

    return
        ((double)present / totalSessions)
        * 100.0;
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

// Prepares both collections before starting the save.
void AttendanceRegister::save(
    const std::string& attendanceFile,
    const std::string& correctionFile) const
{
    if (attendanceFile == correctionFile)
    {
        throw std::invalid_argument(
            "Use different attendance and correction files.");
    }

    SafeFile::writeAll({
        {attendanceFile, FileStorage::encode(records)},
        {correctionFile, FileStorage::encode(corrections)}
    });
}

// Replaces both collections only after both files pass.
void AttendanceRegister::load(const std::string& attendanceFile,
                              const std::string& correctionFile)
{
    if (attendanceFile == correctionFile)
    {
        throw std::invalid_argument(
            "Use different attendance and correction files.");
    }

    std::vector<AttendanceRecord> loadedRecords;
    std::vector<CorrectionRecord> loadedCorrections;

    FileStorage::loadAll(loadedRecords, attendanceFile);
    FileStorage::loadAll(loadedCorrections, correctionFile);

    records.swap(loadedRecords);
    corrections.swap(loadedCorrections);
}