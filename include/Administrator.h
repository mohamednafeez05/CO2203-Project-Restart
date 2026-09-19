#ifndef ADMINISTRATOR_H
#define ADMINISTRATOR_H

#include "Person.h"

class Administrator : public Person
{
private:
    int accessLevel;

public:
    Administrator(const std::string& id,
                  const std::string& name,
                  const std::string& storedHash,
                  int accessLevel);

    void createUser(Person& user);
    void removeUser(std::string userId);
    void generateReport();

    void displayMenu() override;

    int getAccessLevel() const;
};

#endif