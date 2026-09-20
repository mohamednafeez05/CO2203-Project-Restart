#ifndef ATTENDANCE_EXCEPTION_H
#define ATTENDANCE_EXCEPTION_H

#include <exception>

// Base class for every error raised by the attendance subsystem.
// The menu/UI layer can catch this one type to handle all attendance
// problems, or catch a derived type to react to a specific problem.
class AttendanceException : public std::exception
{
public:
    const char* what() const noexcept override;
};

#endif
