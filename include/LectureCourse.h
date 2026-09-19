#ifndef LECTURECOURSE_H
#define LECTURECOURSE_H

#include "Course.h"

class LectureCourse : public Course
{
private:
    double examScore;

public:
    LectureCourse(const std::string& code,
                  const std::string& title,
                  int credits,
                  int capacity);

    double calculateFinalGrade() override;
};

#endif