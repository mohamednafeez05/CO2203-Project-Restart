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
    // Initialises identity and the stored credential.
    Person(const std::string& id, const std::string& name,
        const std::string& storedHash);

    bool login(std::string password);

    // Requires each role to provide its menu.
    virtual void displayMenu() = 0;

    friend std::ostream& operator<<(std::ostream& os, Person& p);

    virtual ~Person();

    // Returns the person's identifier.
    std::string getPersonId() const;

    // Returns the person's name.
    std::string getName() const;

    // Returns the stored credential value for saving.
    std::string getPasswordHash() const;

};

#endif