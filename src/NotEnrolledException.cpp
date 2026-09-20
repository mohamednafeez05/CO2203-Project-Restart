#include "NotEnrolledException.h"

const char* NotEnrolledException::what() const noexcept
{
    return "Student is not enrolled in this course";
}
