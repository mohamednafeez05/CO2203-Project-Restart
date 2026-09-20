#ifndef DUPLICATE_ATTENDANCE_EXCEPTION_H
#define DUPLICATE_ATTENDANCE_EXCEPTION_H

#include "AttendanceException.h"

class DuplicateAttendanceException : public AttendanceException
{
public:
    const char* what() const noexcept override;
};

#endif
