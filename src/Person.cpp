#include "Person.h"
#include <stdexcept>

Person::Person(const std::string& id,
               const std::string& name,
               const std::string& storedHash)
    : personId(id),
      name(name),
      passwordHash(storedHash)
{
    if (id.empty() || name.empty() || storedHash.empty())
    {
        throw std::invalid_argument(
            "Person details cannot be empty.");
    }
}

bool Person::login(std::string password)
{
    return password == passwordHash;
}

std::string Person::getId() const
{
    return personId;
}

std::string Person::getPersonId() const
{
    return personId;
}

std::string Person::getName() const
{
    return name;
}

std::string Person::getPasswordHash() const
{
    return passwordHash;
}

std::ostream& operator<<(std::ostream& os, Person& p)
{
    os << "Person ID: " << p.personId << std::endl;
    os << "Name: " << p.name << std::endl;

    return os;
}

Person::~Person()
{
}