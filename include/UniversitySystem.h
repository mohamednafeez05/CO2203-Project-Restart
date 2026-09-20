#ifndef UNIVERSITY_SYSTEM_H
#define UNIVERSITY_SYSTEM_H
#include "Person.h"
#include "Course.h"
#include "AttendanceRegister.h"
#include "AttendanceCapture.h"
#include <map>
#include <memory>
class Administrator;
class Lecturer;
class Student;
class SystemStorage;
// Owns the complete object graph and enforces permissions at its public boundary.
class UniversitySystem
{
    friend class SystemStorage;
private:
    std::vector<std::unique_ptr<Person>> users;
    std::vector<std::unique_ptr<Course>> courses;
    std::map<std::string, AttendanceSession> sessions;
    AttendanceRegister attendance;
    std::map<std::string, std::unique_ptr<AttendanceCapture>> captures;
    Person* current = nullptr;
    void requireAdmin() const;
    Lecturer& requireOwner(Course& course) const;
    Course& course(const std::string& code) const;
    Student& student(const std::string& id) const;
public:
    UniversitySystem() = default;
    UniversitySystem(const UniversitySystem&) = delete;
    UniversitySystem& operator=(const UniversitySystem&) = delete;
    Person* login(const std::string& id, const std::string& password);
    void logout();
    Person* getCurrent() const;
    Person* findUser(const std::string& id) const;
    Course* findCourse(const std::string& code) const;
    const std::vector<std::unique_ptr<Person>>& getUsers() const;
    const std::vector<std::unique_ptr<Course>>& getCourses() const;
    const std::map<std::string, AttendanceSession>& getSessions() const;
    const AttendanceRegister& getAttendance() const;
    void createUser(int role, const std::string& id, const std::string& name, const std::string& password, const std::string& detail, int year);
    void updateUser(const std::string& id, const std::string& name, const std::string& password);
    void removeUser(const std::string& id);
    void createCourse(int type, const std::string& code, const std::string& title, int credits, int capacity);
    void updateCourse(const std::string& code, const std::string& title, int credits, int capacity);
    void removeCourse(const std::string& code);
    void assignLecturer(const std::string& code, const std::string& id);
    void addSlot(const std::string& code, const TimeSlot& slot);
    void addPrerequisite(const std::string& code, const std::string& prerequisite);
    void completeCourse(const std::string& id, const std::string& code);
    void enrol(const std::string& code);
    void drop(const std::string& code);
    void openSession(const std::string& id, const std::string& code, int slot, int seconds);
    void closeSession(const std::string& id);
    void replay(const std::string& id, AttendanceCapture& source, std::ostream& output);
    void useCapture(const std::string& id, std::unique_ptr<AttendanceCapture> source);
    std::string sessionToken(const std::string& id) const;
    void checkIn(const std::string& id, const std::string& token);
    void correct(const std::string& session, const std::string& student, bool present, const std::string& reason);
    void report(std::ostream& output, const std::string& code = "") const;
    void audit(std::ostream& output, const std::string& code) const;
    void save(const std::string& directory) const;
    void load(const std::string& directory);
    void initialiseDemo();
};
#endif
