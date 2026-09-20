#ifndef LABCOURSE_H
#define LABCOURSE_H
#include "Course.h"
class LabCourse : public Course
{
public:
    LabCourse(const std::string& code, const std::string& title, int credits, int capacity);
    double calculateFinalGrade() override;
};
#endif
