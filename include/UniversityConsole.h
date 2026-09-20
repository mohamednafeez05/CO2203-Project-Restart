#ifndef UNIVERSITY_CONSOLE_H
#define UNIVERSITY_CONSOLE_H
#include "UniversitySystem.h"
class Student;
class Lecturer;
class Administrator;
// Reads input and dispatches role dashboards through Person's virtual interface.
class UniversityConsole
{
    UniversitySystem system;
    std::string directory;
    void listCourses() const;
    void save();
public:
    explicit UniversityConsole(const std::string& directory);
    void run();
    void studentMenu(Student& student);
    void lecturerMenu(Lecturer& lecturer);
    void adminMenu(Administrator& administrator);
};
#endif
