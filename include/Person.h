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
    Person(const std::string& id,
           const std::string& name,
           const std::string& storedHash);

    bool login(std::string password);

    // Every derived role must provide its own menu.
    virtual void displayMenu() = 0;

    // Used by the attendance subsystem.
    std::string getId() const;

    // Used by storage/UI.
    std::string getPersonId() const;
    std::string getName() const;
    std::string getPasswordHash() const;

    friend std::ostream& operator<<(std::ostream& os, Person& p);

    virtual ~Person();
};

#endif