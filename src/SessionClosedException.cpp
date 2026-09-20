#include "SessionClosedException.h"

const char* SessionClosedException::what() const noexcept
{
    return "Attendance session is closed";
}
