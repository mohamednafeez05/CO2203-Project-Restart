#ifndef FILE_STORAGE_H
#define FILE_STORAGE_H

#include <string>
#include <vector>
#include "Student.h"

class FileStorage
{
public:
    // Saves student details to a text file.
    static void saveAll(const std::vector<Student>& students,
                        const std::string& filename);

    // Loads students after validating the whole file.
    static void loadAll(std::vector<Student>& students,
                        const std::string& filename);
};

#endif