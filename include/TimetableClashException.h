#ifndef TIMETABLECLASHEXCEPTION_H
#define TIMETABLECLASHEXCEPTION_H

#include "EnrollmentException.h"

class TimetableClashException : public EnrollmentException
{
public:
    const char* what() const noexcept override;
};

#endif