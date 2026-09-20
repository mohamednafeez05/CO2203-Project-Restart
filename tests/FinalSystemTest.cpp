#include "UniversitySystem.h"
#include "UniversityConsole.h"
#include "Student.h"
#include "Lecturer.h"
#include "ProjectCourse.h"
#include "LectureCourse.h"
#include <thread>
#include <chrono>
#include "FileReplayCapture.h"
#include "RotatingCodeCapture.h"
#include "CourseFullException.h"
#include "TimetableClashException.h"
#include "PrerequisiteNotMetException.h"
#include "SessionClosedException.h"
#include "DuplicateAttendanceException.h"
#include "NotEnrolledException.h"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <functional>
#include <iostream>

template<class E> void rejects(const std::function<void()>& action)
{
    bool failed = false;
    try { action(); } catch (const E&) { failed = true; }
    assert(failed);
}
int main()
{
    namespace fs = std::filesystem;
    Student chainStudent("CHAIN", "Chain test", "test", 1, "CE");
    LectureCourse base("BASE", "Base", 1, 5), middle("MID", "Middle", 1, 5), top("TOP", "Top", 1, 5);
    middle.addPrerequisite(base); top.addPrerequisite(middle);
    chainStudent.markCourseCompleted(middle);
    rejects<PrerequisiteNotMetException>([&] { chainStudent.enrol(top); });
    chainStudent.markCourseCompleted(base); chainStudent.enrol(top);
    AttendanceSession expires("EXPIRY", nullptr, 1); expires.open();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    assert(expires.isExpired() && !expires.isOpen());
    UniversitySystem system; system.initialiseDemo();
    assert(!system.login("S001","wrong"));
    assert(system.login("S001","student123"));
    rejects<std::logic_error>([&] { system.createCourse(1,"BAD","Bad",3,5); });
    system.enrol("C101"); system.enrol("C201");
    rejects<TimetableClashException>([&] { system.enrol("C301"); });
    system.login("S002","student123");
    rejects<CourseFullException>([&] { system.enrol("C201"); });
    system.login("S001","student123"); system.drop("C201");
    system.login("S002","student123");
    rejects<PrerequisiteNotMetException>([&] { system.enrol("C201"); });
    system.enrol("C101");
    system.login("A001","admin123");
    system.addPrerequisite("C301","C201");
    rejects<std::invalid_argument>([&] { system.addPrerequisite("C101","C301"); });
    system.createUser(1,"TEMP","Temporary","temp","CE",1);
    system.updateUser("TEMP","Updated","newpass"); system.removeUser("TEMP"); assert(!system.findUser("TEMP"));
    system.createCourse(3,"TEMP","Temporary",2,5); system.updateCourse("TEMP","Updated",3,8); system.removeCourse("TEMP");
    system.findCourse("C301")->setScores({50,100});
    assert(system.findCourse("C301")->calculateFinalGrade() == 80);
    system.login("L002","lecturer123");
    rejects<std::logic_error>([&] { system.openSession("BAD","C101",0,600); });
    system.login("L001","lecturer123"); system.openSession("MEET1","C101",0,600);
    system.useCapture("MEET1", std::make_unique<RotatingCodeCapture>());
    const auto token = system.sessionToken("MEET1");
    system.login("S001","student123");
    rejects<std::invalid_argument>([&] { system.checkIn("MEET1","wrong"); });
    system.checkIn("MEET1",token);
    rejects<DuplicateAttendanceException>([&] { system.checkIn("MEET1",token); });
    system.login("L001","lecturer123"); system.closeSession("MEET1");
    system.login("S002","student123");
    rejects<SessionClosedException>([&] { system.checkIn("MEET1",token); });
    system.login("L001","lecturer123");
    system.correct("MEET1","S001",false,"Wrong check-in");
    assert(system.getAttendance().getRecords().size() == 1);
    assert(!system.getAttendance().isPresent("S001","MEET1"));
    system.correct("MEET1","S001",true,"Verified presence");
    system.correct("MEET1","S002",true,"Verified missing capture");
    assert(system.getAttendance().getRecords().size() == 1);
    assert(system.getAttendance().getCorrections().size() == 3);
    system.login("L002","lecturer123");
    rejects<std::logic_error>([&] { system.correct("MEET1","S001",false,"Denied"); });
    system.login("L001","lecturer123"); system.openSession("MEET2","C101",0,600);
    { std::ofstream f("mixed-replay.txt"); f << "S001\nS999\nS001\nmalformed extra-field\n"; }
    FileReplayCapture replay("mixed-replay.txt"); std::ostringstream captureLog;
    rejects<std::runtime_error>([&] { system.replay("MEET2",replay,captureLog); });
    assert(captureLog.str().find("Recorded: S001") != std::string::npos);
    assert(captureLog.str().find("Rejected S999") != std::string::npos);
    assert(captureLog.str().find("Rejected S001") != std::string::npos);
    assert(!system.getSessions().at("MEET2").isOpen());
    const fs::path folder = "data/final_system_test";
    assert(!fs::exists(folder)); system.save(folder.string());
    UniversitySystem restored; restored.load(folder.string()); restored.login("A001","admin123");
    assert(restored.findCourse("C101")->getLecturer()->getPersonId() == "L001");
    assert(restored.findCourse("C201")->getPrerequisites()[0] == restored.findCourse("C101"));
    assert(dynamic_cast<Student*>(restored.findUser("S001"))->hasCompletedCourse(*restored.findCourse("C101")));
    assert(restored.findCourse("C301")->calculateFinalGrade() == 80);
    assert(restored.getAttendance().getRecords().size() == 2);
    assert(restored.getAttendance().getCorrections().size() == 3);
    assert(restored.getAttendance().getRecords()[0].getMethod() == "rotating-code");
    std::ostringstream report; restored.report(report,"C101");
    assert(report.str().find("75.000000%") != std::string::npos);
    std::ostringstream audit; restored.audit(audit,"C101");
    assert(audit.str().find("Wrong check-in") != std::string::npos);
    restored.save(folder.string());
    std::ifstream in(folder/"system.txt"); std::ostringstream bytes; bytes << in.rdbuf(); in.close();
    auto original = bytes.str();
    { std::ofstream bad(folder/"system.txt"); bad << "UNIVERSITY_V1\nUSER nonsense\nEND\n"; }
    rejects<std::runtime_error>([&] { restored.load(folder.string()); });
    assert(restored.getAttendance().getRecords().size() == 2);
    { std::ofstream fixed(folder/"system.txt"); fixed << original; }
    fs::remove(folder/"system.txt.bak"); fs::create_directory(folder/"system.txt.bak");
    rejects<std::runtime_error>([&] { restored.save(folder.string()); });
    fs::remove(folder/"system.txt.bak");
    std::ifstream preserved(folder/"system.txt"); std::ostringstream same; same << preserved.rdbuf(); assert(same.str() == original);
    preserved.close();
    std::istringstream commands("1\nS001\nstudent123\n6\n0\n1\nL001\nlecturer123\n7\n0\n1\nA001\nadmin123\n13\n0\n0\n");
    std::ostringstream consoleOutput;
    auto* oldIn = std::cin.rdbuf(commands.rdbuf()); auto* oldOut = std::cout.rdbuf(consoleOutput.rdbuf());
    try { UniversityConsole console(folder.string()); console.run(); }
    catch (...) { std::cin.rdbuf(oldIn); std::cout.rdbuf(oldOut); throw; }
    std::cin.rdbuf(oldIn); std::cout.rdbuf(oldOut);
    assert(consoleOutput.str().find("Student S001") != std::string::npos);
    assert(consoleOutput.str().find("Lecturer L001") != std::string::npos);
    assert(consoleOutput.str().find("Administrator") != std::string::npos);
    std::cout << "PASS: role menus, permissions, prerequisites, capacity, clashes, both captures, immutable corrections, percentages, restart and corrupt/save-failure paths.\n";
}
