#ifndef ENROLLMENTEXCEPTION_H
#define ENROLLMENTEXCEPTION_H

#include <exception>

class EnrollmentException : public std::exception
{
public:
    const char* what() const noexcept override;
};

#endif