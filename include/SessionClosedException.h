#ifndef SESSION_CLOSED_EXCEPTION_H
#define SESSION_CLOSED_EXCEPTION_H

#include "AttendanceException.h"

class SessionClosedException : public AttendanceException
{
public:
    const char* what() const noexcept override;
};

#endif
