#include "Person.h"

bool Person::login(std::string password)
{
    return password == passwordHash;
}


Person::~Person()
{
}

std::ostream& operator<<(std::ostream& os, Person& p)
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