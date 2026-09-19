#ifndef PREREQUISITENOTMETEXCEPTION_H
#define PREREQUISITENOTMETEXCEPTION_H

#include "EnrollmentException.h"

class PrerequisiteNotMetException : public EnrollmentException
{
public:
    const char* what() const noexcept override;
};

#endif