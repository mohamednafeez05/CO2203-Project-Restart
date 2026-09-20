#ifndef LECTURECOURSE_H
#define LECTURECOURSE_H
#include "Course.h"
class LectureCourse : public Course
{
public:
    LectureCourse(const std::string& code, const std::string& title, int credits, int capacity);
    double calculateFinalGrade() override;
};
#endif
