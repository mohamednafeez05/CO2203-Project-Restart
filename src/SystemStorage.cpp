#include "SystemStorage.h"
#include "UniversitySystem.h"
#include "Student.h"
#include "Lecturer.h"
#include "Administrator.h"
#include "LectureCourse.h"
#include "LabCourse.h"
#include "ProjectCourse.h"
#include "SafeFile.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <set>
#include <algorithm>
#include <stdexcept>

namespace
{
    std::string quoted(const std::string& text)
    {
        if (text.empty() || text.find_first_of("\r\n") != std::string::npos)
            throw std::invalid_argument("Empty or multiline saved text.");
        std::ostringstream out; out << std::quoted(text); return out.str();
    }
    void endRow(std::istringstream& row)
    {
        if (row.fail()) throw std::runtime_error("Malformed snapshot row.");
        row >> std::ws;
        if (!row.eof()) throw std::runtime_error("Unexpected snapshot fields.");
    }
    std::filesystem::path snapshot(const std::string& directory)
    {
        const std::filesystem::path folder(directory);
        if (std::filesystem::exists(folder) && !std::filesystem::is_directory(folder))
            throw std::runtime_error("Data path is not a directory.");
        const auto path = folder / "system.txt";
        if (std::filesystem::exists(path.string() + ".tmp"))
            throw std::runtime_error("Unfinished save: inspect system.txt.tmp and .bak before recovery.");
        if (std::filesystem::exists(path) && (!std::filesystem::is_regular_file(path) || std::filesystem::is_symlink(path)))
            throw std::runtime_error("Snapshot must be a regular, unlinked file.");
        return path;
    }
}

void SystemStorage::save(const UniversitySystem& sys, const std::string& directory)
{
    std::ostringstream out; out << "UNIVERSITY_V1\n";
    for (const auto& user : sys.users)
    {
        int role = 3, number = 1; std::string detail = "Administration";
        if (auto* s = dynamic_cast<Student*>(user.get())) { role = 1; number = s->getYearOfStudy(); detail = s->getMajor(); }
        else if (auto* l = dynamic_cast<Lecturer*>(user.get())) { role = 2; detail = l->getDepartment(); }
        out << "USER " << role << ' ' << quoted(user->getPersonId()) << ' ' << quoted(user->getName()) << ' ' << quoted(user->getPasswordHash()) << ' ' << number << ' ' << quoted(detail) << '\n';
    }
    for (const auto& c : sys.courses)
    {
        int type = dynamic_cast<LectureCourse*>(c.get()) ? 1 : dynamic_cast<LabCourse*>(c.get()) ? 2 : 3;
        out << "COURSE " << type << ' ' << quoted(c->getCourseCode()) << ' ' << quoted(c->getTitle()) << ' ' << c->getCreditValue() << ' ' << c->getCapacity() << '\n';
        for (const auto& slot : c->getSlots()) out << "SLOT " << quoted(c->getCourseCode()) << ' ' << quoted(slot.getDay()) << ' ' << slot.getStartTime() << ' ' << slot.getEndTime() << ' ' << quoted(slot.getLocation()) << '\n';
        if (c->getLecturer()) out << "ASSIGN " << quoted(c->getCourseCode()) << ' ' << quoted(c->getLecturer()->getPersonId()) << '\n';
        for (auto* pre : c->getPrerequisites()) out << "PREREQ " << quoted(c->getCourseCode()) << ' ' << quoted(pre->getCourseCode()) << '\n';
        out << "SCORES " << quoted(c->getCourseCode()) << ' ' << c->getScores().size();
        for (double score : c->getScores()) out << ' ' << std::setprecision(17) << score;
        out << '\n';
    }
    for (const auto& user : sys.users) if (auto* s = dynamic_cast<Student*>(user.get()))
    {
        for (auto* c : s->getCompletedCourses()) out << "COMPLETE " << quoted(s->getPersonId()) << ' ' << quoted(c->getCourseCode()) << '\n';
        for (auto* c : s->getEnrolledCourses()) out << "ENROL " << quoted(s->getPersonId()) << ' ' << quoted(c->getCourseCode()) << '\n';
    }
    for (const auto& item : sys.sessions)
    {
        const auto& s = item.second;
        out << "SESSION " << quoted(item.first) << ' ' << quoted(s.getCourseCode()) << ' ' << quoted(s.getLecturerId()) << ' ' << s.getSlotIndex() << ' ' << s.getDurationSeconds() << ' ' << s.getStartTime() << ' ' << s.getExpiryTime() << '\n';
        for (const auto& id : s.getRoster()) out << "ROSTER " << quoted(item.first) << ' ' << quoted(id) << '\n';
    }
    for (const auto& r : sys.attendance.getRecords()) out << "RECORD " << quoted(r.getStudentId()) << ' ' << quoted(r.getSessionId()) << ' ' << r.getCheckInTime() << ' ' << quoted(r.getStatus()) << ' ' << quoted(r.getMethod()) << '\n';
    for (const auto& r : sys.attendance.getCorrections()) out << "CORRECTION " << quoted(r.getStudentId()) << ' ' << quoted(r.getSessionId()) << ' ' << quoted(r.getLecturerId()) << ' ' << r.getCorrectionTime() << ' ' << static_cast<int>(r.getAction()) << ' ' << quoted(r.getReason()) << '\n';
    out << "END\n";
    const auto path = snapshot(directory);
    if (!std::filesystem::exists(path) && std::filesystem::exists(path.string() + ".bak"))
        throw std::runtime_error("Original snapshot missing; recover backup before saving.");
    std::filesystem::create_directories(directory);
    SafeFile::writeAll({{path.string(), out.str()}});
}

void SystemStorage::load(UniversitySystem& live, const std::string& directory)
{
    const auto path = snapshot(directory);
    std::ifstream input(path);
    if (!input) throw std::runtime_error("Cannot open system.txt. Use --init-demo with a new folder or restore a complete snapshot.");
    std::string line; std::getline(input, line);
    if (!line.empty() && line.back() == '\r') line.pop_back();
    if (line != "UNIVERSITY_V1") throw std::runtime_error("Invalid system snapshot header.");
    UniversitySystem staged;
    std::vector<std::string> rows;
    bool ended = false;
    while (std::getline(input, line))
    {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (ended) { if (!line.empty()) throw std::runtime_error("Data after snapshot END."); continue; }
        if (line == "END") { ended = true; continue; }
        std::istringstream row(line); std::string tag; row >> tag;
        if (tag == "USER")
        {
            int role = 0, year = 0; std::string id, name, pass, detail;
            row >> role >> std::quoted(id) >> std::quoted(name) >> std::quoted(pass) >> year >> std::quoted(detail); endRow(row);
            if (staged.findUser(id)) throw std::runtime_error("Duplicate user.");
            if (role == 1) staged.users.push_back(std::make_unique<Student>(id,name,pass,year,detail));
            else if (role == 2) staged.users.push_back(std::make_unique<Lecturer>(id,name,pass,detail));
            else if (role == 3) staged.users.push_back(std::make_unique<Administrator>(id,name,pass,1));
            else throw std::runtime_error("Unknown user role.");
        }
        else if (tag == "COURSE")
        {
            int type = 0, credits = 0, limit = 0; std::string code, title;
            row >> type >> std::quoted(code) >> std::quoted(title) >> credits >> limit; endRow(row);
            if (staged.findCourse(code)) throw std::runtime_error("Duplicate course.");
            if (type == 1) staged.courses.push_back(std::make_unique<LectureCourse>(code,title,credits,limit));
            else if (type == 2) staged.courses.push_back(std::make_unique<LabCourse>(code,title,credits,limit));
            else if (type == 3) staged.courses.push_back(std::make_unique<ProjectCourse>(code,title,credits,limit));
            else throw std::runtime_error("Unknown course type.");
        }
        else
        {
            const std::set<std::string> allowed = {"SLOT","ASSIGN","PREREQ","SCORES","COMPLETE","ENROL","SESSION","ROSTER","RECORD","CORRECTION"};
            if (!allowed.count(tag)) throw std::runtime_error("Unknown snapshot row.");
            rows.push_back(line);
        }
    }
    if (input.bad() || !ended) throw std::runtime_error("Truncated snapshot: END required.");
    bool admin = false; for (const auto& user : staged.users) if (dynamic_cast<Administrator*>(user.get())) admin = true;
    if (!admin) throw std::runtime_error("Snapshot needs an administrator.");
    std::set<std::string> unique;
    for (const char* phase : {"SLOT","ASSIGN","PREREQ","SCORES","COMPLETE","ENROL","SESSION","ROSTER","RECORD","CORRECTION"})
    for (const auto& text : rows)
    {
        std::istringstream row(text); std::string tag, a, b; row >> tag;
        if (tag != phase) continue;
        row >> std::quoted(a);
        if (tag == "SLOT")
        {
            std::string day, location; int start = 0, end = 0;
            row >> std::quoted(day) >> start >> end >> std::quoted(location); endRow(row);
            if (end > 1440) throw std::runtime_error("Slot exceeds one day.");
            staged.course(a).addTimeSlot(TimeSlot(day,start,end,location));
        }
        else if (tag == "SCORES")
        {
            int count = -1; row >> count;
            if (count < 0 || count > 10000) throw std::runtime_error("Invalid score count.");
            std::vector<double> scores; for (int i = 0; i < count; ++i) { double score = 0; row >> score; scores.push_back(score); }
            endRow(row); staged.course(a).setScores(scores);
            if (!unique.insert(tag + ":" + a).second) throw std::runtime_error("Duplicate scores.");
        }
        else if (tag == "SESSION")
        {
            std::string actor; int slot = -1, duration = 0; std::time_t start = 0, end = 0;
            row >> std::quoted(b) >> std::quoted(actor) >> slot >> duration >> start >> end; endRow(row);
            auto& c = staged.course(b);
            if (!dynamic_cast<Lecturer*>(staged.findUser(actor)) || a.empty() || staged.sessions.count(a) || slot < 0 || static_cast<std::size_t>(slot) >= c.getSlots().size() || duration < 1 || duration > 86400 || start <= 0 || end <= start || end - start != duration)
                throw std::runtime_error("Invalid session relationships or times.");
            AttendanceSession s(a,&c.getSlots()[slot],duration); s.configure(b,actor,slot,{}); s.restoreTimes(start,end); staged.sessions.emplace(a,s);
        }
        else if (tag == "RECORD")
        {
            std::time_t at = 0; std::string state, method;
            row >> std::quoted(b) >> at >> std::quoted(state) >> std::quoted(method); endRow(row);
            const auto& session = staged.sessions.at(b); staged.student(a);
            if (std::find(session.getRoster().begin(),session.getRoster().end(),a) == session.getRoster().end() || at < session.getStartTime() || at >= session.getExpiryTime()) throw std::runtime_error("Attendance outside session or roster.");
            if (!unique.insert(tag + ":" + a + ":" + b).second) throw std::runtime_error("Duplicate attendance.");
            staged.attendance.records.emplace_back(a,b,at,state,method);
        }
        else if (tag == "CORRECTION")
        {
            std::time_t at = 0; int action = 0; std::string actor, reason;
            row >> std::quoted(b) >> std::quoted(actor) >> at >> action >> std::quoted(reason); endRow(row);
            const auto& session = staged.sessions.at(b); staged.student(a);
            if (!dynamic_cast<Lecturer*>(staged.findUser(actor)) || std::find(session.getRoster().begin(),session.getRoster().end(),a) == session.getRoster().end() || at < session.getStartTime() || (action != 1 && action != 2)) throw std::runtime_error("Invalid audit relationships.");
            staged.attendance.corrections.emplace_back(a,b,actor,reason,static_cast<CorrectionRecord::Action>(action),at);
        }
        else
        {
            row >> std::quoted(b); endRow(row);
            const std::string key = tag + ":" + a + (tag == "ASSIGN" ? "" : ":" + b);
            if (!unique.insert(key).second) throw std::runtime_error("Duplicate relationship.");
            if (tag == "ASSIGN") { auto* l = dynamic_cast<Lecturer*>(staged.findUser(b)); if (!l) throw std::runtime_error("Unknown lecturer."); staged.course(a).setLecturer(*l); }
            else if (tag == "PREREQ") staged.course(a).addPrerequisite(staged.course(b));
            else if (tag == "COMPLETE") staged.student(a).markCourseCompleted(staged.course(b));
            else if (tag == "ENROL") { if (staged.course(b).getSlots().empty()) throw std::runtime_error("Enrolled course has no slots."); staged.student(a).enrol(staged.course(b)); }
            else if (tag == "ROSTER") { auto& s = staged.sessions.at(a); staged.student(b); auto ids = s.getRoster(); ids.push_back(b); s.configure(s.getCourseCode(),s.getLecturerId(),s.getSlotIndex(),ids); }
        }
    }
    // Pointees retain their addresses; the old graph is destroyed only after validation.
    live.captures.clear(); live.current = nullptr;
    live.users.swap(staged.users); live.courses.swap(staged.courses); live.sessions.swap(staged.sessions);
    live.attendance.records.swap(staged.attendance.records); live.attendance.corrections.swap(staged.attendance.corrections);
}
