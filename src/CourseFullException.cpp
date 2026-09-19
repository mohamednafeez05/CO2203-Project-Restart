#include "CourseFullException.h"

const char* CourseFullException::what() const noexcept
{
    return "Course is full";
}