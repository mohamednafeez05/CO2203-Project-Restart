#ifndef NOT_ENROLLED_EXCEPTION_H
#define NOT_ENROLLED_EXCEPTION_H

#include "AttendanceException.h"

class NotEnrolledException : public AttendanceException
{
public:
    const char* what() const noexcept override;
};

#endif
