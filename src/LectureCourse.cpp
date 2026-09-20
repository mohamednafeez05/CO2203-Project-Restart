#include "LectureCourse.h"
LectureCourse::LectureCourse(const std::string& code, const std::string& title, int credits, int capacity)
    : Course(code, title, credits, capacity) {}
double LectureCourse::calculateFinalGrade()
{
    return getScores().empty() ? 0.0 : getScores().back();
}
