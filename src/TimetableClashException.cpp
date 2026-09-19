#include "TimetableClashException.h"

const char* TimetableClashException::what() const noexcept
{
    return "Timetable clash detected";
}