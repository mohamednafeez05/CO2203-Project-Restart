#include "EnrollmentException.h"

const char* EnrollmentException::what() const noexcept
{
    return "Enrollment exception";
}