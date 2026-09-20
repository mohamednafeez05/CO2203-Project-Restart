// Member 2 demonstration: scheduling + attendance, end to end.
// Build (from the project root):
//   g++ -std=c++17 -Iinclude demo/member2_demo.cpp $(ls src/*.cpp | grep -v main.cpp) -o member2_demo
#include <iostream>
#include <fstream>
#include <vector>

#include "Student.h"
#include "Lecturer.h"
#include "LectureCourse.h"
#include "AttendanceRegister.h"
#include "FileReplayCapture.h"
#include "RotatingCodeCapture.h"
#include "AttendanceException.h"
#include "TimetableClashException.h"

using namespace std;

static void heading(const string& t) { cout << "\n=== " << t << " ===\n"; }

int main()
{
    Lecturer lecturer("L001", "Dr. Silva", "hash", "Computer Engineering");
    Student ann("S001", "Ann", "hash", 2, "Computer Engineering");
    Student bob("S002", "Bob", "hash", 2, "Computer Engineering");
    Student cara("S003", "Cara", "hash", 2, "Computer Engineering");

    LectureCourse oop("CO2203", "Object-Oriented Programming", 3, 30);
    LectureCourse algo("CO2201", "Algorithm Design", 3, 30);
    oop.addTimeSlot(TimeSlot("Mon", 900, 1100, "Room A"));
    algo.addTimeSlot(TimeSlot("Mon", 1000, 1200, "Room B"));   // overlaps OOP
    oop.setLecturer(lecturer);

    heading("1. Timetable + clash detection");
    ann.enrol(oop);
    try { ann.enrol(algo); }
    catch (const TimetableClashException& e) { cout << "Rejected: " << e.what() << "\n"; }
    ann.viewTimetable();
    bob.enrol(oop);
    cara.enrol(oop);

    heading("2. Drop cleans the timetable");
    ann.drop(oop);
    ann.viewTimetable();
    ann.enrol(algo);                       // no longer clashes
    cout << "Ann can now take " << algo.getCourseCode() << "\n";
    ann.drop(algo);                        // switch back to OOP for the rest
    ann.enrol(oop);

    heading("3. Attendance with a rotating code");
    AttendanceRegister reg;
    AttendanceSession live = lecturer.openAttendanceSession(
        oop, TimeSlot("Mon", 900, 1100, "Room A"), 10);
    RotatingCodeCapture rotating;
    rotating.beginSession(live);
    cout << "Code on the lecturer's screen: " << rotating.getCurrentCode() << "\n";
    string code = rotating.getCurrentCode();
    cout << "Bob enters the WRONG code   -> " << (rotating.submitCode("0000") ? "accepted" : "rejected") << "\n";
    cout << "Bob enters the right code   -> " << (rotating.submitCode(code) ? "accepted" : "rejected") << "\n";
    reg.markPresent(bob, oop, live);
    cout << "Code after a check-in has changed: "
         << (rotating.getCurrentCode() != code ? "yes" : "no") << "\n";
    cout << "Reusing the old code        -> " << (rotating.submitCode(code) ? "accepted" : "rejected") << "\n";
    try { reg.markPresent(bob, oop, live); }
    catch (const AttendanceException& e) { cout << "Bob again: " << e.what() << "\n"; }
    rotating.endSession();
    try { reg.markPresent(cara, oop, live); }
    catch (const AttendanceException& e) { cout << "Cara after close: " << e.what() << "\n"; }

    heading("4. Attendance replayed from a file");
    { ofstream f("demo_roll.txt"); f << "S001\nS003\n"; }
    AttendanceSession second("CO2203-2", &oop.getSlots()[0], 600);
    FileReplayCapture file("demo_roll.txt");
    AttendanceCapture& capture = file;          // only the interface is used
    capture.beginSession(second);
    string id;
    vector<Student*> roster = { &ann, &bob, &cara };
    while (capture.captureNext(id))
        for (Student* s : roster)
            if (s->getPersonId() == id) reg.markPresent(*s, oop, second);
    capture.endSession();

    heading("5. Lecturer correction (audited)");
    reg.addCorrection(bob, oop, "CO2203-2", lecturer, "Bob was present but the file missed him");
    for (const CorrectionRecord& c : reg.getCorrections())
        cout << c.getStudentId() << " / " << c.getSessionId()
             << " by " << c.getLecturerId() << ": " << c.getReason() << "\n";

    heading("6. Percentages and eligibility");
    for (Student* s : roster)
        cout << s->getName() << ": " << reg.getPercentage(*s, oop) << "%\n";
    AttendanceRegister::setEligibilityThreshold(75.0);
    cout << "Eligible at " << AttendanceRegister::getEligibilityThreshold() << "%:";
    for (Student* s : reg.getEligibilityReport(oop)) cout << " " << s->getName();
    cout << "\n";
    return 0;
}
