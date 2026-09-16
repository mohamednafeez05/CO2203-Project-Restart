#ifndef PERSON_H
#define PERSON_H

#include <string>
#include <iostream>

class Person
{
private:
    std::string personId;
    std::string name;
    std::string passwordHash;

public:
    bool login(std::string password);
    void displayMenu();

    friend std::ostream& operator<<(std::ostream& os, Person& p);

    virtual ~Person();
};

#endif