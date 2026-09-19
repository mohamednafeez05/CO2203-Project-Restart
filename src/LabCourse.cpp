#include "LabCourse.h"

LabCourse::LabCourse(
    const std::string& code,
    const std::string& title,
    int credits,
    int capacity)
    : Course(code, title, credits, capacity),
      maxGroupSize(0)
{
}

double LabCourse::calculateFinalGrade()
{
    if (labScores.empty())
    {
        return 0.0;
    }

    double total = 0.0;

    for (double score : labScores)
    {
        total += score;
    }

    return total / labScores.size();
}