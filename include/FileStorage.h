#ifndef FILE_STORAGE_H
#define FILE_STORAGE_H

#include <string>
#include <vector>
#include "Student.h"
#include "AttendanceRecord.h"
#include "CorrectionRecord.h"

class FileStorage
{
public:
    // Saves student details to a text file.
    static void saveAll(const std::vector<Student>& students,
                        const std::string& filename);

    // Loads students after validating the whole file.
    static void loadAll(std::vector<Student>& students,
                        const std::string& filename);

    // Saves attendance records.
    static void saveAll(const std::vector<AttendanceRecord>& records,
                        const std::string& filename);

    // Restores attendance records after validation.
    static void loadAll(std::vector<AttendanceRecord>& records,
                        const std::string& filename);

    // Saves the correction history.
    static void saveAll(const std::vector<CorrectionRecord>& corrections,
                        const std::string& filename);

    // Restores the correction history after validation.
    static void loadAll(std::vector<CorrectionRecord>& corrections,
                        const std::string& filename);

    // Validates objects and converts them into file text.
    static std::string encode(const std::vector<Student>& students);

    static std::string encode(
        const std::vector<AttendanceRecord>& records);

    static std::string encode(
        const std::vector<CorrectionRecord>& corrections);
    };

#endif