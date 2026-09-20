#include "UniversityConsole.h"
#include "ConsoleInput.h"
#include "Student.h"
#include "Lecturer.h"
#include "Administrator.h"
#include "FileReplayCapture.h"
#include "RotatingCodeCapture.h"
#include <iostream>
#include <stdexcept>

namespace
{
    std::string text(const char* prompt) { return ConsoleInput::readText(prompt); }
    int number(const char* prompt, int max = 10000) { return ConsoleInput::readInt(prompt, 1, max); }
    void showError(const std::exception& error)
    {
        if (std::string(error.what()) == "Console input ended.") throw std::runtime_error(error.what());
        std::cout << "Action failed: " << error.what() << '\n';
    }
}
UniversityConsole::UniversityConsole(const std::string& path) : directory(path) { system.load(directory); }
void UniversityConsole::save() { system.save(directory); std::cout << "Saved.\n"; }
void UniversityConsole::listCourses() const
{
    for (const auto& c : system.getCourses())
    {
        std::cout << *c << "Seats: " << c->getEnrolledStudents().size() << '/' << c->getCapacity() << " | Grade example: " << c->calculateFinalGrade() << '\n';
        int index = 0;
        for (const auto& slot : c->getSlots()) std::cout << "  Slot " << index++ << ": " << slot.getDay() << ' ' << slot.getStartTime() << '-' << slot.getEndTime() << " minutes | " << slot.getLocation() << '\n';
        std::cout << "Prerequisites:"; for (const auto* p : c->getPrerequisites()) std::cout << ' ' << p->getCourseCode(); std::cout << '\n';
    }
}
void UniversityConsole::run()
{
    while (true)
    {
        std::cout << "\nLogic Foundry | University System\n1. Login\n0. Save and exit\n";
        if (ConsoleInput::readInt("Choice: ",0,1) == 0) { save(); return; }
        const auto id = text("User ID: "); const auto pass = text("Password: ");
        Person* user = system.login(id,pass);
        if (!user) { std::cout << "Invalid credentials.\n"; continue; }
        user->displayMenu(*this);
        system.logout();
    }
}
void UniversityConsole::studentMenu(Student& student)
{
    while (true)
    {
        std::cout << "\nStudent " << student.getPersonId() << "\n1. Profile\n2. Courses\n3. Enrol\n4. Drop\n5. Timetable\n6. Attendance report\n7. Check in with code\n0. Save and logout\n";
        try
        {
            int choice = ConsoleInput::readInt("Choice: ",0,7);
            if (choice == 0) { save(); return; }
            if (choice == 1) std::cout << student << '\n';
            if (choice == 2) listCourses();
            if (choice == 3) { system.enrol(text("Course code: ")); save(); }
            if (choice == 4) { system.drop(text("Course code: ")); save(); }
            if (choice == 5) std::cout << student.getTimetable();
            if (choice == 6) system.report(std::cout);
            if (choice == 7) { auto id = text("Session ID: "); auto token = text("Attendance code: "); system.checkIn(id,token); save(); }
        }
        catch (const std::exception& error) { showError(error); }
    }
}
void UniversityConsole::lecturerMenu(Lecturer& lecturer)
{
    while (true)
    {
        std::cout << "\nLecturer " << lecturer.getPersonId() << "\n1. My courses\n2. Enrolment list\n3. Open session\n4. Close session\n5. Select capture (file/code)\n6. Show code\n7. Attendance report\n8. Correct attendance\n9. Audit history\n0. Save and logout\n";
        try
        {
            int choice = ConsoleInput::readInt("Choice: ",0,9);
            if (choice == 0) { save(); return; }
            if (choice == 1) for (const auto* c : lecturer.getAssignedCourses()) std::cout << *c;
            if (choice == 2) { auto* c = system.findCourse(text("Course code: ")); if (!c) throw std::invalid_argument("Unknown course."); lecturer.viewEnrolmentList(*c); }
            if (choice == 3)
            {
                auto code = text("Course code: "); auto id = text("Unique session ID: ");
                int slot = ConsoleInput::readInt("Slot index (first is 0): ",0,1000); int duration = number("Duration in seconds: ",86400);
                system.openSession(id,code,slot,duration); save();
            }
            if (choice == 4) { system.closeSession(text("Session ID: ")); save(); }
            if (choice == 5)
            {
                auto id = text("Session ID: "); int mechanism = number("1. File replay  2. Rotating code: ",2);
                if (mechanism == 1)
                {
                    FileReplayCapture replay(text("Replay file: "));
                    try { system.replay(id,replay,std::cout); }
                    catch (...) { save(); throw; }
                    save();
                }
                else { system.useCapture(id,std::make_unique<RotatingCodeCapture>()); std::cout << "Code: " << system.sessionToken(id) << '\n'; }
            }
            if (choice == 6) std::cout << "Code: " << system.sessionToken(text("Session ID: ")) << '\n';
            if (choice == 7) system.report(std::cout);
            if (choice == 8)
            {
                auto id = text("Session ID: "); auto student = text("Student ID: ");
                bool present = ConsoleInput::readInt("0. Absent  1. Present: ",0,1) == 1;
                auto reason = text("Reason: "); system.correct(id,student,present,reason); save();
            }
            if (choice == 9) system.audit(std::cout,text("Course code: "));
        }
        catch (const std::exception& error) { showError(error); }
    }
}
void UniversityConsole::adminMenu(Administrator&)
{
    while (true)
    {
        std::cout << "\nAdministrator\n1. List users\n2. Create user\n3. Update name/password\n4. Remove user\n5. List courses\n6. Create course\n7. Edit course details\n8. Remove course\n9. Assign lecturer\n10. Add slot\n11. Add prerequisite\n12. Record course completion\n13. Report\n14. Set course grading example scores\n0. Save and logout\n";
        try
        {
            int choice = ConsoleInput::readInt("Choice: ",0,14);
            if (choice == 0) { save(); return; }
            if (choice == 1) for (const auto& p : system.getUsers()) std::cout << *p;
            if (choice == 2)
            {
                int role = number("1. Student  2. Lecturer  3. Administrator: ",3);
                auto id = text("ID: "); auto name = text("Name: "); auto pass = text("Password: ");
                auto detail = text("Major/department (Administration for admin): "); int year = number("Year (1 for staff): ",10);
                system.createUser(role,id,name,pass,detail,year); save();
            }
            if (choice == 3) { auto id = text("ID: "); auto name = text("New name: "); auto pass = text("New password: "); system.updateUser(id,name,pass); save(); }
            if (choice == 4) { system.removeUser(text("ID: ")); save(); }
            if (choice == 5) listCourses();
            if (choice == 6 || choice == 7)
            {
                int type = choice == 6 ? number("1. Lecture  2. Lab  3. Project: ",3) : 0;
                auto code = text("Course code: "); auto title = text("Title: "); int credits = number("Credits: ",30); int capacity = number("Capacity: ");
                if (choice == 6) system.createCourse(type,code,title,credits,capacity);
                else system.updateCourse(code,title,credits,capacity);
                save();
            }
            if (choice == 8) { system.removeCourse(text("Course code: ")); save(); }
            if (choice == 9) { auto code = text("Course code: "); auto id = text("Lecturer ID: "); system.assignLecturer(code,id); save(); }
            if (choice == 10)
            {
                auto code = text("Course code: "); auto day = text("Day (e.g. Monday, use consistent spelling): ");
                int start = ConsoleInput::readInt("Start in minutes after midnight: ",0,1439); int end = number("End in minutes after midnight: ",1440); auto room = text("Location: ");
                system.addSlot(code,TimeSlot(day,start,end,room)); save();
            }
            if (choice == 11) { auto code = text("Course code: "); auto pre = text("Prerequisite code: "); system.addPrerequisite(code,pre); save(); }
            if (choice == 12) { auto id = text("Student ID: "); auto code = text("Completed course code: "); system.completeCourse(id,code); save(); }
            if (choice == 13) system.report(std::cout);
            if (choice == 14)
            {
                auto* c = system.findCourse(text("Course code: ")); if (!c) throw std::invalid_argument("Unknown course.");
                int count = number("Number of scores (project: last is final assessment): ",100);
                std::vector<double> scores; for (int i = 0; i < count; ++i) scores.push_back(ConsoleInput::readInt("Score 0-100: ",0,100));
                c->setScores(scores); std::cout << "Calculated: " << c->calculateFinalGrade() << '\n'; save();
            }
        }
        catch (const std::exception& error) { showError(error); }
    }
}
