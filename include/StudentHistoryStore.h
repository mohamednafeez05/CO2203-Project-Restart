#ifndef STUDENT_HISTORY_STORE_H
#define STUDENT_HISTORY_STORE_H

#include "Student.h"
#include "AttendanceRecord.h"
#include "CorrectionRecord.h"
#include <string>
#include <vector>

// Owns the collections used by the history viewer; exposes read-only access.
class StudentHistoryStore
{
private:
    std::vector<Student> students;
    std::vector<AttendanceRecord> records;
    std::vector<CorrectionRecord> corrections;

    static void validateLinks(const std::vector<Student>& students,
                              const std::vector<AttendanceRecord>& records,
                              const std::vector<CorrectionRecord>& corrections);
public:
    void load(const std::string& directory);
    void save(const std::string& directory) const;
    const std::vector<Student>& getStudents() const;
    const std::vector<AttendanceRecord>& getRecords() const;
    const std::vector<CorrectionRecord>& getCorrections() const;
    // Returned pointers remain valid until the next successful load.
    const Student* findStudent(const std::string& id) const;
};

#endif
