#ifndef ATTENDANCE_REGISTER_H
#define ATTENDANCE_REGISTER_H

#include <vector>
#include <string>

#include "AttendanceRecord.h"
#include "CorrectionRecord.h"
#include "AttendanceSession.h"

class Student;
class Course;
class Lecturer;

// Keeps every attendance record for ONE course.
// (Composition: the Course owns its AttendanceRegister.)
//
// The register stores its own copies of the sessions, records and
// corrections. It follows the Rule of Three: because it manages
// collections it owns, it defines the destructor, copy constructor and
// copy assignment operator explicitly.
class AttendanceRegister
{
private:
    std::vector<AttendanceSession> sessions;

    std::vector<AttendanceRecord> records;

    std::vector<CorrectionRecord> corrections;

    // Minimum attendance percentage needed to sit the exam.
    static double eligibilityThreshold;

    bool alreadyMarked(
        const std::string& studentId,
        const std::string& sessionId
    ) const;

    bool sessionExists(
        const std::string& sessionId
    ) const;

public:
    AttendanceRegister();

    // Rule of Three.
    ~AttendanceRegister();
    AttendanceRegister(const AttendanceRegister& other);
    AttendanceRegister& operator=(const AttendanceRegister& other);

    // ---- Sessions -----------------------------------------------------

    // Remembers a session so it counts towards "sessions held".
    // Adding a session with an id that is already registered is ignored.
    void addSession(const AttendanceSession& session);

    int getSessionCount() const;

    // ---- Taking attendance -------------------------------------------

    // Records that the student attended the session.
    // Throws SessionClosedException, NotEnrolledException or
    // DuplicateAttendanceException.
    void markPresent(
        Student& student,
        Course& course,
        AttendanceSession& session
    );

    // ---- Lecturer corrections ----------------------------------------

    // Lecturer adds a missing attendance record (student was present but
    // was not captured). The correction is logged.
    // Throws NotEnrolledException, DuplicateAttendanceException,
    // std::invalid_argument (blank reason or unknown session) or
    // std::logic_error (lecturer is not assigned to the course).
    void addCorrection(
        Student& student,
        Course& course,
        const std::string& sessionId,
        Lecturer& lecturer,
        const std::string& reason
    );

    // Lecturer removes a wrong attendance record. The removal is logged.
    // Throws std::invalid_argument when there is no such record,
    // the reason is blank, or std::logic_error when not assigned.
    void removeAttendance(
        Student& student,
        Course& course,
        const std::string& sessionId,
        Lecturer& lecturer,
        const std::string& reason
    );

    // ---- Reports -----------------------------------------------------

    int getPresentCount(
        const std::string& studentId
    ) const;

    // Attendance percentage (0 to 100) over the sessions held so far.
    // Returns 0 when no sessions have been held.
    double getPercentage(
        const Student& student
    ) const;

    // Same as above, using the signature shown in the UML diagram.
    // The register already belongs to one course, so the course is only
    // used to make sure the student is enrolled in it.
    double getPercentage(
        const Student& student,
        const Course& course
    ) const;

    // Percentage using an explicit number of sessions (kept for callers
    // that track the total themselves).
    double getAttendancePercentage(
        const std::string& studentId,
        int totalSessions
    ) const;

    // Students of the course whose attendance meets the threshold.
    std::vector<Student*> getEligibilityReport(
        const Course& course
    ) const;

    static double getEligibilityThreshold();
    static void setEligibilityThreshold(double percent);

    const std::vector<AttendanceSession>&
    getSessions() const;

    const std::vector<AttendanceRecord>&
    getRecords() const;

    const std::vector<CorrectionRecord>&
    getCorrections() const;
};

#endif
