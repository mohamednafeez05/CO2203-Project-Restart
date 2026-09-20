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





    void displayMenu() override;
    void displayMenu(UniversityConsole& console) override;

    int getAccessLevel() const;
};

#endif