#include "Person.h"

bool Person::login(std::string password)
{
    return password == passwordHash;
}

void Person::displayMenu()
{
    std::cout << "Person menu" << std::endl;
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