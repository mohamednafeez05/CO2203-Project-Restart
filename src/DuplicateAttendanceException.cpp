#include "DuplicateAttendanceException.h"

const char* DuplicateAttendanceException::what() const noexcept
{
    return "Attendance has already been recorded for this session";
}
