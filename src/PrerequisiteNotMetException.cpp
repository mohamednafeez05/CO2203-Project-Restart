#include "PrerequisiteNotMetException.h"

const char* PrerequisiteNotMetException::what() const noexcept
{
    return "Course prerequisite has not been met";
}