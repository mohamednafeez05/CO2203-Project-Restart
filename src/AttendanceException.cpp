#include "AttendanceException.h"

const char* AttendanceException::what() const noexcept
{
    return "Attendance exception";
}
