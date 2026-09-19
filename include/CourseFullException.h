#ifndef COURSEFULLEXCEPTION_H
#define COURSEFULLEXCEPTION_H

#include "EnrollmentException.h"

class CourseFullException : public EnrollmentException
{
public:
    const char* what() const noexcept override;
};

#endif