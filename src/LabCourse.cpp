#include "LabCourse.h"
LabCourse::LabCourse(const std::string& code, const std::string& title, int credits, int capacity)
    : Course(code, title, credits, capacity) {}
double LabCourse::calculateFinalGrade()
{
    double total = 0; for (double score : getScores()) total += score;
    return getScores().empty() ? 0.0 : total / getScores().size();
}
