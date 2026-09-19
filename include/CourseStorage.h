#ifndef COURSE_STORAGE_H
#define COURSE_STORAGE_H

#include "Course.h"
#include "Student.h"
#include <memory>
#include <string>
#include <vector>

class CourseStorage
{
public:
    using Courses = std::vector<std::unique_ptr<Course>>;

    static void load(const std::string& filename,
                     std::vector<Student>& students,
                     Courses& courses);

    static std::string encode(const std::vector<Student>& students,
                              const Courses& courses);
};

#endif
