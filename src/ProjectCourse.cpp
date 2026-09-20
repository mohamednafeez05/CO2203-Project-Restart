#include "ProjectCourse.h"
ProjectCourse::ProjectCourse(const std::string& code, const std::string& title, int credits, int capacity)
    : Course(code, title, credits, capacity) {}
double ProjectCourse::calculateFinalGrade()
{
    const auto& scores = getScores();
    if (scores.empty()) return 0;
    if (scores.size() == 1) return scores[0];
    double total = 0;
    for (std::size_t i = 0; i + 1 < scores.size(); ++i) total += scores[i];
    return 0.4 * total / (scores.size() - 1) + 0.6 * scores.back();
}
