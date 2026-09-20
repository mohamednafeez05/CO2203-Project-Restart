#ifndef STUDENT_HISTORY_STORE_H
#define STUDENT_HISTORY_STORE_H

#include "Student.h"
#include "AttendanceRecord.h"
#include "CorrectionRecord.h"
#include <string>
#include <vector>
#include "CourseStorage.h"
#include <map>

class AttendanceRegister;
// Owns the collections used by the history viewer; exposes read-only access.
class StudentHistoryStore
{
private:
    CourseStorage::Courses courses;
    std::map<std::string, std::string> sessionCourses;
    std::vector<Student> students;
    std::vector<AttendanceRecord> records;
    std::vector<CorrectionRecord> corrections;

    static void validateLinks(const std::vector<Student>& students,
                              const std::vector<AttendanceRecord>& records,
                              const std::vector<CorrectionRecord>& corrections);
public:
    void linkSession(const std::string& sessionId,
                    const std::string& courseCode);

    const std::map<std::string, std::string>& getSessionCourses() const;

    std::vector<const Course*> getCourses() const;
    StudentHistoryStore() = default;
    StudentHistoryStore(const StudentHistoryStore&) = delete;
    StudentHistoryStore& operator=(const StudentHistoryStore&) = delete;

    const Course* findCourse(const std::string& code) const;

    void importAttendance(const AttendanceRegister& batch);
    void enrol(const std::string& studentId,
               const std::string& courseCode);

    void drop(const std::string& studentId,
              const std::string& courseCode);

    void load(const std::string& directory);
    void save(const std::string& directory) const;
    const std::vector<Student>& getStudents() const;
    const std::vector<AttendanceRecord>& getRecords() const;
    const std::vector<CorrectionRecord>& getCorrections() const;
    // Returned pointers remain valid until the next successful load.
    const Student* findStudent(const std::string& id) const;
};

#endif
