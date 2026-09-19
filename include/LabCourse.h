#ifndef LABCOURSE_H
#define LABCOURSE_H

#include "Course.h"
#include <vector>

class LabCourse : public Course
{
private:
    int maxGroupSize;
    std::vector<double> labScores;

public:
    LabCourse(const std::string& code,
              const std::string& title,
              int credits,
              int capacity);

    double calculateFinalGrade() override;
};

#endif