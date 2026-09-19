#ifndef PROJECTCOURSE_H
#define PROJECTCOURSE_H

#include "Course.h"

class Milestone;

class ProjectCourse : public Course
{
private:
    std::vector<Milestone> milestones;

public:
    double calculateFinalGrade() override;
};

#endif