#include "Person.h"

bool Person::login(std::string password)
{
    return password == passwordHash;
}

void Person::displayMenu()
{
    std::cout << "Person menu" << std::endl;
}

std::string Person::getId() const
{
    return personId;
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