#include "UniversitySystem.h"
#include "SystemStorage.h"
#include "Administrator.h"
#include "Lecturer.h"
#include "Student.h"
#include "LectureCourse.h"
#include "LabCourse.h"
#include "ProjectCourse.h"
#include "ObjectLookup.h"
#include "SessionClosedException.h"
#include "NotEnrolledException.h"
#include "DuplicateAttendanceException.h"
#include <algorithm>
#include <stdexcept>
#include <iomanip>

Person* UniversitySystem::findUser(const std::string& id) const
{ return findObject(users, id, [](const Person& p) { return p.getPersonId(); }); }
Course* UniversitySystem::findCourse(const std::string& code) const
{ return findObject(courses, code, [](const Course& c) { return c.getCourseCode(); }); }
Course& UniversitySystem::course(const std::string& code) const
{ auto* c = findCourse(code); if (!c) throw std::invalid_argument("Unknown course."); return *c; }
Student& UniversitySystem::student(const std::string& id) const
{ auto* s = dynamic_cast<Student*>(findUser(id)); if (!s) throw std::invalid_argument("Unknown student."); return *s; }
Person* UniversitySystem::login(const std::string& id, const std::string& password)
{ current = nullptr; auto* user = findUser(id); if (user && user->login(password)) current = user; return current; }
void UniversitySystem::logout() { current = nullptr; }
Person* UniversitySystem::getCurrent() const { return current; }
void UniversitySystem::requireAdmin() const
{ if (!dynamic_cast<Administrator*>(current)) throw std::logic_error("Administrator login required."); }
Lecturer& UniversitySystem::requireOwner(Course& c) const
{ auto* l = dynamic_cast<Lecturer*>(current); if (!l || c.getLecturer() != l) throw std::logic_error("Only the assigned lecturer may perform this action."); return *l; }
const std::vector<std::unique_ptr<Person>>& UniversitySystem::getUsers() const { requireAdmin(); return users; }
const std::vector<std::unique_ptr<Course>>& UniversitySystem::getCourses() const { return courses; }
const std::map<std::string, AttendanceSession>& UniversitySystem::getSessions() const { return sessions; }
const AttendanceRegister& UniversitySystem::getAttendance() const { return attendance; }

void UniversitySystem::createUser(int role, const std::string& id, const std::string& name, const std::string& password, const std::string& detail, int year)
{
    requireAdmin();
    if (findUser(id)) throw std::invalid_argument("Duplicate user ID.");
    if (role == 1) users.push_back(std::make_unique<Student>(id, name, password, year, detail));
    else if (role == 2) users.push_back(std::make_unique<Lecturer>(id, name, password, detail));
    else if (role == 3) users.push_back(std::make_unique<Administrator>(id, name, password, 1));
    else throw std::invalid_argument("Unknown role.");
}
void UniversitySystem::updateUser(const std::string& id, const std::string& name, const std::string& password)
{ requireAdmin(); auto* p = findUser(id); if (!p) throw std::invalid_argument("Unknown user."); p->updateIdentity(name, password); }
void UniversitySystem::removeUser(const std::string& id)
{
    requireAdmin(); auto* user = findUser(id);
    if (!user || user == current) throw std::logic_error("Unknown user or cannot remove current administrator.");
    for (const auto& c : courses)
    {
        if (c->getLecturer() == user) throw std::logic_error("Reassign lecturer's courses first.");
        for (auto* s : c->getEnrolledStudents()) if (s == user) throw std::logic_error("Drop student's courses first.");
    }
    for (const auto& item : sessions)
        if (item.second.getLecturerId() == id || std::find(item.second.getRoster().begin(), item.second.getRoster().end(), id) != item.second.getRoster().end())
            throw std::logic_error("User has attendance history; removal is blocked.");
    for (const auto& note : attendance.getCorrections()) if (note.getLecturerId() == id) throw std::logic_error("User has audit history.");
    users.erase(std::remove_if(users.begin(), users.end(), [&](const auto& p) { return p.get() == user; }), users.end());
}
void UniversitySystem::createCourse(int type, const std::string& code, const std::string& title, int credits, int limit)
{
    requireAdmin(); if (findCourse(code)) throw std::invalid_argument("Duplicate course code.");
    if (type == 1) courses.push_back(std::make_unique<LectureCourse>(code, title, credits, limit));
    else if (type == 2) courses.push_back(std::make_unique<LabCourse>(code, title, credits, limit));
    else if (type == 3) courses.push_back(std::make_unique<ProjectCourse>(code, title, credits, limit));
    else throw std::invalid_argument("Unknown course type.");
}
void UniversitySystem::updateCourse(const std::string& code, const std::string& title, int credits, int limit)
{ requireAdmin(); course(code).updateDetails(title, credits, limit); }
void UniversitySystem::removeCourse(const std::string& code)
{
    requireAdmin(); auto& c = course(code);
    if (!c.getEnrolledStudents().empty()) throw std::logic_error("Course has enrolments.");
    for (const auto& item : sessions) if (item.second.getCourseCode() == code) throw std::logic_error("Course has attendance history.");
    for (const auto& item : courses) for (auto* pre : item->getPrerequisites()) if (pre == &c) throw std::logic_error("Course is a prerequisite.");
    for (const auto& user : users) if (auto* s = dynamic_cast<Student*>(user.get())) if (s->hasCompletedCourse(c)) throw std::logic_error("Course has completion history.");
    if (c.getLecturer()) c.getLecturer()->removeAssignedCourse(c);
    courses.erase(std::remove_if(courses.begin(), courses.end(), [&](const auto& item) { return item.get() == &c; }), courses.end());
}
void UniversitySystem::assignLecturer(const std::string& code, const std::string& id)
{
    requireAdmin(); auto* l = dynamic_cast<Lecturer*>(findUser(id)); if (!l) throw std::invalid_argument("Unknown lecturer.");
    for (const auto& item : sessions) if (item.second.getCourseCode() == code && item.second.isOpen()) throw std::logic_error("Close course sessions before reassignment.");
    course(code).setLecturer(*l);
}
void UniversitySystem::addSlot(const std::string& code, const TimeSlot& slot)
{
    requireAdmin();
    if (slot.getEndTime() > 1440) throw std::invalid_argument("Use minutes after midnight, at most 1440.");
    for (const auto& item : sessions) if (item.second.getCourseCode() == code) throw std::logic_error("Cannot change slots with session history.");
    course(code).addTimeSlot(slot);
}
void UniversitySystem::addPrerequisite(const std::string& code, const std::string& prerequisite)
{ requireAdmin(); course(code).addPrerequisite(course(prerequisite)); }
void UniversitySystem::completeCourse(const std::string& id, const std::string& code)
{ requireAdmin(); student(id).markCourseCompleted(course(code)); }
void UniversitySystem::enrol(const std::string& code)
{
    auto* s = dynamic_cast<Student*>(current); if (!s) throw std::logic_error("Student login required.");
    if (course(code).getSlots().empty()) throw std::logic_error("Course has no scheduled slots.");
    s->enrol(course(code));
}
void UniversitySystem::drop(const std::string& code)
{ auto* s = dynamic_cast<Student*>(current); if (!s) throw std::logic_error("Student login required."); s->drop(course(code)); }
void UniversitySystem::openSession(const std::string& id, const std::string& code, int slot, int seconds)
{
    auto& c = course(code); auto& l = requireOwner(c);
    if (id.empty() || sessions.count(id) || slot < 0 || static_cast<std::size_t>(slot) >= c.getSlots().size() || seconds < 1 || seconds > 86400)
        throw std::invalid_argument("Invalid session ID, slot or duration.");
    std::vector<std::string> roster; for (auto* s : c.getEnrolledStudents()) roster.push_back(s->getPersonId());
    AttendanceSession meeting(id, &c.getSlots()[slot], seconds);
    meeting.configure(code, l.getPersonId(), slot, roster); meeting.open(); sessions.emplace(id, meeting);
}
void UniversitySystem::closeSession(const std::string& id)
{ auto& s = sessions.at(id); requireOwner(course(s.getCourseCode())); if (captures.count(id)) captures.erase(id); s.close(); }
void UniversitySystem::replay(const std::string& id, AttendanceCapture& source, std::ostream& output)
{ auto& s = sessions.at(id); auto& c = course(s.getCourseCode()); requireOwner(c); captures.erase(id); attendance.capture(source, c, s, "file-replay", output); }
void UniversitySystem::useCapture(const std::string& id, std::unique_ptr<AttendanceCapture> source)
{ auto& s = sessions.at(id); requireOwner(course(s.getCourseCode())); if (!s.isOpen()) throw SessionClosedException(); source->beginSession(s); captures[id] = std::move(source); }
std::string UniversitySystem::sessionToken(const std::string& id) const
{ const auto& s = sessions.at(id); requireOwner(course(s.getCourseCode())); if (!s.isOpen()) throw SessionClosedException(); return captures.at(id)->token(); }
void UniversitySystem::checkIn(const std::string& id, const std::string& token)
{
    auto* student = dynamic_cast<Student*>(current); if (!student) throw std::logic_error("Student login required.");
    auto& s = sessions.at(id); auto& c = course(s.getCourseCode());
    if (!s.isOpen()) throw SessionClosedException();
    if (!c.isStudentEnrolled(*student) || std::find(s.getRoster().begin(), s.getRoster().end(), student->getPersonId()) == s.getRoster().end()) throw NotEnrolledException();
    for (const auto& r : attendance.getRecords()) if (r.getSessionId() == id && r.getStudentId() == student->getPersonId()) throw DuplicateAttendanceException();
    if (attendance.isPresent(student->getPersonId(), id)) throw DuplicateAttendanceException();
    if (!captures.count(id) || !captures.at(id)->acceptToken(token)) throw std::invalid_argument("Wrong or expired code; ask lecturer to refresh it.");
    attendance.markCaptured(*student, c, s, "rotating-code");
}
void UniversitySystem::correct(const std::string& meeting, const std::string& id, bool present, const std::string& reason)
{ auto& s = sessions.at(meeting); auto& c = course(s.getCourseCode()); auto& l = requireOwner(c); attendance.correct(student(id), c, s, l, reason, present); }
void UniversitySystem::report(std::ostream& out, const std::string& filter) const
{
    if (!current) throw std::logic_error("Login required.");
    auto* self = dynamic_cast<Student*>(current); auto* teacher = dynamic_cast<Lecturer*>(current);
    for (const auto& c : courses)
    {
        if ((!filter.empty() && filter != c->getCourseCode()) || (teacher && c->getLecturer() != teacher)) continue;
        int present = 0, possible = 0;
        for (const auto& user : users) if (auto* s = dynamic_cast<Student*>(user.get()))
        {
            if (self && s != self) continue;
            int attended = 0, total = 0;
            for (const auto& item : sessions)
            {
                const auto& session = item.second;
                if (session.getCourseCode() != c->getCourseCode() || std::find(session.getRoster().begin(), session.getRoster().end(), s->getPersonId()) == session.getRoster().end()) continue;
                ++total; if (attendance.isPresent(s->getPersonId(), item.first)) ++attended;
            }
            if (total || c->isStudentEnrolled(*s)) out << c->getCourseCode() << " | " << s->getPersonId() << " | " << attended << '/' << total << " | " << (total ? std::to_string(100.0 * attended / total) + "%" : "N/A") << '\n';
            present += attended; possible += total;
        }
        if (!self) out << "Course " << c->getCourseCode() << ": enrolled " << c->getEnrolledStudents().size() << '/' << c->getCapacity() << ", attendance " << (possible ? std::to_string(100.0 * present / possible) + "%" : "N/A") << '\n';
    }
}
void UniversitySystem::audit(std::ostream& out, const std::string& code) const
{
    auto& c = course(code); if (!dynamic_cast<Administrator*>(current)) requireOwner(c);
    for (const auto& r : attendance.getRecords()) if (sessions.at(r.getSessionId()).getCourseCode() == code)
        out << "ORIGINAL " << r.getStudentId() << ' ' << r.getSessionId() << ' ' << r.getCheckInTime() << ' ' << r.getStatus() << ' ' << r.getMethod() << '\n';
    for (const auto& r : attendance.getCorrections()) if (sessions.at(r.getSessionId()).getCourseCode() == code)
        out << "CORRECTION " << r.getStudentId() << ' ' << r.getSessionId() << ' ' << r.getLecturerId() << ' ' << r.getCorrectionTime() << ' ' << (r.getAction() == CorrectionRecord::MARKED_PRESENT ? "present" : "absent") << ' ' << r.getReason() << '\n';
}
void UniversitySystem::save(const std::string& dir) const { SystemStorage::save(*this, dir); }
void UniversitySystem::load(const std::string& dir) { SystemStorage::load(*this, dir); }
void UniversitySystem::initialiseDemo()
{
    if (!users.empty()) throw std::logic_error("Demo requires an empty system.");
    users.push_back(std::make_unique<Administrator>("A001", "Demo Administrator", "admin123", 1)); current = users.back().get();
    createUser(2,"L001","Demo Lecturer","lecturer123","Computing",1);
    createUser(2,"L002","Other Lecturer","lecturer123","Computing",1);
    createUser(1,"S001","Nafeez","student123","Computer Engineering",2);
    createUser(1,"S002","Demo Student","student123","Computer Engineering",2);
    createCourse(1,"C101","Programming Fundamentals",3,30);
    createCourse(2,"C201","Programming Lab",1,1);
    createCourse(3,"C301","Software Project",3,20);
    addSlot("C101",TimeSlot("Monday",540,600,"Room A"));
    addSlot("C201",TimeSlot("Tuesday",600,660,"Lab B"));
    addSlot("C301",TimeSlot("Monday",570,630,"Room C"));
    for (const char* code : {"C101","C201","C301"}) assignLecturer(code,"L001");
    addPrerequisite("C201","C101"); completeCourse("S001","C101");
    logout();
}
