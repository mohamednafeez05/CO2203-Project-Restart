#include "Person.h"
#include <stdexcept>

bool Person::login(std::string password)
{
    return password == passwordHash;
}


Person::~Person()
{
}

std::ostream& operator<<(std::ostream& os, const Person& p)
{
    os << "Person ID: " << p.personId << std::endl;
    os << "Name: " << p.name << std::endl;

    return os;
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

// Initialises and validates person details.
Person::Person(const std::string& id, const std::string& name,
               const std::string& storedHash)
    : personId(id), name(name), passwordHash(storedHash)
{
    if (id.empty() || name.empty() || storedHash.empty())
    {
        throw std::invalid_argument("Person details cannot be empty.");
    }
}

std::string Person::getId() const
{
    return getPersonId();
}
void Person::updateIdentity(const std::string& value, const std::string& credential)
{
    if (value.empty() || credential.empty()) throw std::invalid_argument("Empty name or credential.");
    name = value; passwordHash = credential;
}
