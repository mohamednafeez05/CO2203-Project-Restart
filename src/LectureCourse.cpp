#include "LectureCourse.h"

LectureCourse::LectureCourse(
    const std::string& code,
    const std::string& title,
    int credits,
    int capacity)
    : Course(code, title, credits, capacity),
      examScore(0.0)
{
}

double LectureCourse::calculateFinalGrade()
{
    return examScore;
}