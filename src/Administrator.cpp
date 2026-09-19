#include "Administrator.h"
#include <stdexcept>

Administrator::Administrator(
    const std::string& id,
    const std::string& name,
    const std::string& storedHash,
    int accessLevel)
    : Person(id, name, storedHash),
      accessLevel(accessLevel)
{
    if (accessLevel < 0)
    {
        throw std::invalid_argument(
            "Access level cannot be negative.");
    }
}

void Administrator::displayMenu()
{
}

void Administrator::createUser(Person& user)
{
}

void Administrator::removeUser(std::string userId)
{
}

void Administrator::generateReport()
{
}

int Administrator::getAccessLevel() const
{
    return accessLevel;
}