#ifndef PROJECTCOURSE_H
#define PROJECTCOURSE_H
#include "Course.h"
class ProjectCourse : public Course
{
public:
    ProjectCourse(const std::string& code, const std::string& title, int credits, int capacity);
    double calculateFinalGrade() override;
};
#endif
