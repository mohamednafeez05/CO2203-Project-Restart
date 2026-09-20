#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Student.h"
#include "Lecturer.h"
#include "LectureCourse.h"
#include "LabCourse.h"
#include "AttendanceRegister.h"
#include "AttendanceCapture.h"
#include "FileReplayCapture.h"
#include "RotatingCodeCapture.h"
#include "AttendanceException.h"
#include "SessionClosedException.h"
#include "NotEnrolledException.h"
#include "DuplicateAttendanceException.h"
#include "TimetableClashException.h"
#include "CourseFullException.h"
#include "PrerequisiteNotMetException.h"

using namespace std;

static int passed = 0, failed = 0;
#define CHECK(cond, name) do { if (cond) { ++passed; } else { ++failed; cout << "  FAIL: " << name << "  (line " << __LINE__ << ")\n"; } } while (0)

template <typename Ex, typename F>
static bool throwsType(F f) { try { f(); } catch (const Ex&) { return true; } catch (...) { return false; } return false; }

static void testTimeSlot()
{
    cout << "[TimeSlot]\n";
    TimeSlot a("Mon", 900, 1100, "R1");
    CHECK(a.overlaps(TimeSlot("Mon", 1000, 1200, "R2")), "partial overlap");
    CHECK(a.overlaps(TimeSlot("Mon", 800, 1200, "R2")), "containing overlap");
    CHECK(a.overlaps(TimeSlot("Mon", 930, 1000, "R2")), "contained overlap");
    CHECK(!a.overlaps(TimeSlot("Mon", 1100, 1200, "R2")), "back-to-back does NOT clash");
    CHECK(!a.overlaps(TimeSlot("Mon", 700, 900, "R2")), "back-to-back before does NOT clash");
    CHECK(!a.overlaps(TimeSlot("Tue", 900, 1100, "R1")), "different day never clashes");
    CHECK(a == TimeSlot("Mon", 1000, 1200, "R2"), "operator== means overlap");
    CHECK(!(a == TimeSlot("Tue", 900, 1100, "R1")), "operator== false on other day");
    CHECK(a.isSameSlot(TimeSlot("Mon", 900, 1100, "R1")), "isSameSlot true");
    CHECK(!a.isSameSlot(TimeSlot("Mon", 900, 1100, "R9")), "isSameSlot false on location");
}

static void testTimetable()
{
    cout << "[Timetable]\n";
    Timetable t;
    TimeSlot a("Mon", 900, 1100, "R1"), b("Mon", 1100, 1200, "R1"), c("Mon", 1000, 1130, "R2");
    t.addTimeSlot(a);
    CHECK(t.hasClashWith(c), "clash detected");
    CHECK(!t.hasClashWith(b), "no clash back-to-back");
    t.addTimeSlot(b);
    t.removeTimeSlot(a);
    CHECK(!t.hasClashWith(TimeSlot("Mon", 900, 1000, "X")), "slot a removed");
    CHECK(t.hasClashWith(TimeSlot("Mon", 1100, 1150, "X")), "slot b still there");
    // Removing a slot that is not stored must do nothing.
    t.removeTimeSlot(TimeSlot("Fri", 1, 2, "Z"));
    CHECK(t.hasClashWith(TimeSlot("Mon", 1100, 1150, "X")), "removing unknown slot changes nothing");
    // removing by an OVERLAPPING-but-different slot must not delete anything
    t.removeTimeSlot(TimeSlot("Mon", 1100, 1300, "R1"));
    CHECK(t.hasClashWith(TimeSlot("Mon", 1100, 1150, "X")), "removal is exact-match, not overlap");
    ostringstream os; os << t;
    CHECK(os.str().find("Mon") != string::npos, "operator<< prints slots");
    Timetable empty; ostringstream e; e << empty;
    CHECK(e.str().find("No courses scheduled") != string::npos, "operator<< empty message");
}

static void testEnrolAndDangling()
{
    cout << "[Enrol / dangling-pointer regression]\n";
    Student s("S1", "Ann", "h", 2, "CE");
    LectureCourse c1("CO1", "A", 3, 10);
    c1.addTimeSlot(TimeSlot("Tue", 900, 1000, "R1"));
    s.enrol(c1);
    // Force many reallocations of Course::slots AFTER the student enrolled.
    for (int i = 0; i < 200; ++i)
        c1.addTimeSlot(TimeSlot("Wed", 800 + i, 801 + i, "R"));
    ostringstream os;
    s.viewTimetable();  // must not touch freed memory (ASan would abort)
    CHECK(true, "viewTimetable after slots vector reallocated");
    // and dropping must still clean the timetable
    LectureCourse other("CO2", "B", 3, 10);
    other.addTimeSlot(TimeSlot("Tue", 930, 1030, "R2"));
    CHECK(throwsType<TimetableClashException>([&]{ s.enrol(other); }), "clash rejected while enrolled");
    s.drop(c1);
    CHECK(!throwsType<TimetableClashException>([&]{ s.enrol(other); }), "after drop the clashing course can be taken");
    CHECK(other.isStudentEnrolled(s), "enrolled after drop");
}

static void testEnrolRules()
{
    cout << "[Enrol rules]\n";
    Student s1("S1", "A", "h", 1, "CE"), s2("S2", "B", "h", 1, "CE");
    LectureCourse full("F1", "Full", 3, 1);
    full.addTimeSlot(TimeSlot("Mon", 900, 1000, "R"));
    s1.enrol(full);
    CHECK(throwsType<CourseFullException>([&]{ s2.enrol(full); }), "full course rejected");

    LectureCourse base("B1", "Base", 3, 5), mid("M1", "Mid", 3, 5), top("T1", "Top", 3, 5);
    mid.addPrerequisite(base);
    top.addPrerequisite(mid);
    Student s3("S3", "C", "h", 1, "CE");
    CHECK(throwsType<PrerequisiteNotMetException>([&]{ s3.enrol(mid); }), "missing prereq rejected");
    s3.markCourseCompleted(mid);   // has mid, but NOT base -> chain broken
    CHECK(throwsType<PrerequisiteNotMetException>([&]{ s3.enrol(top); }), "prerequisite CHAIN enforced");
    s3.markCourseCompleted(base);
    CHECK(!throwsType<PrerequisiteNotMetException>([&]{ s3.enrol(top); }), "chain satisfied -> allowed");
}

static void testRegister()
{
    cout << "[AttendanceRegister]\n";
    Student s("S1", "Ann", "h", 2, "CE"), other("S2", "Bob", "h", 2, "CE");
    Lecturer lec("L1", "Dr", "h", "CS"), stranger("L2", "Dr2", "h", "CS");
    LectureCourse c("CO1", "A", 3, 10);
    c.addTimeSlot(TimeSlot("Mon", 900, 1000, "R"));
    c.setLecturer(lec);
    s.enrol(c);

    AttendanceRegister reg;
    AttendanceSession s1("SES1", &c.getSlots()[0], 600); s1.open();
    AttendanceSession s2("SES2", &c.getSlots()[0], 600); s2.open();
    AttendanceSession s3("SES3", &c.getSlots()[0], 600); s3.open();
    AttendanceSession s4("SES4", &c.getSlots()[0], 600); s4.open();

    reg.markPresent(s, c, s1);
    CHECK(reg.getPresentCount("S1") == 1, "present counted");
    CHECK(throwsType<DuplicateAttendanceException>([&]{ reg.markPresent(s, c, s1); }), "duplicate rejected (typed)");
    CHECK(throwsType<AttendanceException>([&]{ reg.markPresent(s, c, s1); }), "duplicate is an AttendanceException");
    CHECK(throwsType<NotEnrolledException>([&]{ reg.markPresent(other, c, s1); }), "not enrolled rejected (typed)");
    AttendanceSession closed("X", &c.getSlots()[0], 600);   // never opened
    CHECK(throwsType<SessionClosedException>([&]{ reg.markPresent(s, c, closed); }), "unopened session rejected (typed)");
    s2.close();
    CHECK(throwsType<SessionClosedException>([&]{ reg.markPresent(s, c, s2); }), "closed session rejected (typed)");

    // Sessions only count once someone attended; register the others explicitly.
    reg.addSession(s2); reg.addSession(s3); reg.addSession(s4);
    reg.addSession(s1);   // duplicate id ignored
    CHECK(reg.getSessionCount() == 4, "4 distinct sessions, duplicate id ignored");
    CHECK(reg.getPercentage(s) == 25.0, "1 of 4 = 25%");
    CHECK(reg.getPercentage(s, c) == 25.0, "UML-signature overload agrees");
    CHECK(reg.getPercentage(other, c) == 0.0, "non-enrolled student = 0%");
    CHECK(reg.getAttendancePercentage("S1", 0) == 0.0, "zero sessions -> 0% not divide-by-zero");

    // corrections
    CHECK(throwsType<invalid_argument>([&]{ reg.addCorrection(s, c, "SES3", lec, "   "); }), "blank reason rejected");
    CHECK(throwsType<logic_error>([&]{ reg.addCorrection(s, c, "SES3", stranger, "late"); }), "unassigned lecturer rejected");
    CHECK(throwsType<invalid_argument>([&]{ reg.addCorrection(s, c, "NOPE", lec, "late"); }), "unknown session rejected");
    CHECK(throwsType<DuplicateAttendanceException>([&]{ reg.addCorrection(s, c, "SES1", lec, "again"); }), "cannot correct an already-present record");
    reg.addCorrection(s, c, "SES3", lec, "Arrived late, was present");
    CHECK(reg.getPresentCount("S1") == 2, "correction adds attendance");
    CHECK(reg.getCorrections().size() == 1, "correction logged");
    CHECK(reg.getCorrections()[0].getLecturerId() == "L1", "correction records who did it");
    CHECK(reg.getCorrections()[0].getAction() == CorrectionRecord::MARKED_PRESENT, "action recorded");
    CHECK(reg.getPercentage(s) == 50.0, "percentage updated after correction");

    reg.removeAttendance(s, c, "SES3", lec, "Entered by mistake");
    CHECK(reg.getPresentCount("S1") == 1, "removal reduces attendance");
    CHECK(reg.getCorrections().size() == 2, "removal is also logged (nothing erased from audit)");
    CHECK(reg.getCorrections()[1].getAction() == CorrectionRecord::REMOVED_PRESENT, "removal action recorded");
    CHECK(throwsType<invalid_argument>([&]{ reg.removeAttendance(s, c, "SES3", lec, "again"); }), "removing a missing record rejected");

    // eligibility
    AttendanceRegister::setEligibilityThreshold(20.0);
    CHECK(reg.getEligibilityReport(c).size() == 1, "25% >= 20% threshold -> eligible");
    AttendanceRegister::setEligibilityThreshold(80.0);
    CHECK(reg.getEligibilityReport(c).empty(), "25% < 80% threshold -> not eligible");
    CHECK(throwsType<invalid_argument>([]{ AttendanceRegister::setEligibilityThreshold(150); }), "bad threshold rejected");
    CHECK(AttendanceRegister::getEligibilityThreshold() == 80.0, "threshold unchanged after bad set");

    // Rule of Three: copies must be independent
    AttendanceRegister copy(reg);
    CHECK(copy.getRecords().size() == reg.getRecords().size(), "copy ctor copies records");
    copy.addSession(AttendanceSession("EXTRA", &c.getSlots()[0], 60));
    CHECK(copy.getSessionCount() == 5 && reg.getSessionCount() == 4, "copy is independent of original");
    AttendanceRegister assigned;
    assigned = reg;
    CHECK(assigned.getSessionCount() == 4, "copy assignment copies sessions");
    assigned = assigned;   // self-assignment must be safe
    CHECK(assigned.getSessionCount() == 4, "self-assignment safe");
}

static void testSession()
{
    cout << "[AttendanceSession]\n";
    AttendanceSession blank;
    CHECK(!blank.isOpen(), "default session closed");
    CHECK(!blank.isExpired(), "never-opened session is not 'expired'");
    AttendanceSession s("S", nullptr, 600);
    CHECK(!s.isOpen(), "new session starts closed");
    s.open();
    CHECK(s.isOpen(), "opens");
    s.close();
    CHECK(!s.isOpen(), "closes");
    AttendanceSession quick("Q", nullptr, -1);   // already-expired duration
    quick.open();
    CHECK(!quick.isOpen() && quick.isExpired(), "expired session is not open");
    AttendanceSession copy = s;                   // must be copyable (Lecturer returns by value)
    CHECK(copy.getSessionId() == "S", "copyable / return by value");
}

static void testFileReplay()
{
    cout << "[FileReplayCapture]\n";
    { ofstream f("replay_ok.txt", ios::binary); f << "S1\r\n\r\n  S2  \r\nS3\n\n"; }
    AttendanceSession ses("R1", nullptr, 600);
    FileReplayCapture cap("replay_ok.txt");
    cap.beginSession(ses);
    CHECK(ses.isOpen(), "begin opens the session");
    vector<string> got; string id;
    while (cap.captureNext(id)) got.push_back(id);
    CHECK(got.size() == 3 && got[0] == "S1" && got[1] == "S2" && got[2] == "S3", "blank lines skipped, CR and spaces trimmed");
    cap.endSession();
    CHECK(!ses.isOpen(), "end closes the session");
    CHECK(!cap.captureNext(id), "no capture after end");
    cap.beginSession(ses);   // reusable
    got.clear(); while (cap.captureNext(id)) got.push_back(id);
    CHECK(got.size() == 3, "can replay a second time");
    cap.endSession();

    FileReplayCapture missing("no_such_file.txt");
    AttendanceSession s2("R2", nullptr, 600);
    CHECK(throwsType<runtime_error>([&]{ missing.beginSession(s2); }), "missing file throws");
    CHECK(!missing.captureNext(id), "failed begin leaves capture inactive");
    { AttendanceCapture* base = new FileReplayCapture("replay_ok.txt"); delete base; CHECK(true, "polymorphic delete ok"); }
}

static void testRotating()
{
    cout << "[RotatingCodeCapture]\n";
    AttendanceSession ses("C1", nullptr, 600);
    RotatingCodeCapture cap;
    CHECK(!cap.submitCode("1234"), "submit before begin fails");
    cap.beginSession(ses);
    CHECK(ses.isOpen(), "begin opens the session");
    string code = cap.getCurrentCode();
    CHECK(code.size() == 4, "4-digit code");
    CHECK(cap.validateCode(code), "current code valid");
    CHECK(!cap.validateCode("0000x"), "wrong code invalid");
    CHECK(!cap.submitCode("wrong"), "wrong code rejected");
    CHECK(cap.getCurrentCode() == code, "wrong attempt does not rotate the code");
    CHECK(cap.submitCode(code), "right code accepted");
    CHECK(cap.getCurrentCode() != "" , "code exists after use");
    bool rotated = false;
    for (int i = 0; i < 20 && !rotated; ++i) { string c2 = cap.getCurrentCode(); cap.submitCode(c2); rotated = (cap.getCurrentCode() != c2); }
    CHECK(rotated, "code rotates after successful use");
    cap.endSession();
    CHECK(!ses.isOpen(), "end closes the session");
    CHECK(!cap.submitCode(code), "no submit after end");

    // expiry
    AttendanceSession s2("C2", nullptr, 600);
    RotatingCodeCapture shortLived(1);
    shortLived.beginSession(s2);
    string c = shortLived.getCurrentCode();
    CHECK(shortLived.validateCode(c), "fresh code valid");
    // simulate passage of time by waiting
    struct timespec ts = {2, 0}; nanosleep(&ts, nullptr);
    CHECK(shortLived.isCodeExpired(), "code expires");
    CHECK(!shortLived.validateCode(c), "expired code rejected");
    CHECK(!shortLived.submitCode(c), "expired code cannot be submitted");
    shortLived.regenerateCode();
    CHECK(shortLived.validateCode(shortLived.getCurrentCode()), "regenerate restores validity");
    shortLived.endSession();

    // captureNext (console) with piped input: wrong code, then right code, then END
    {
        AttendanceSession s3("C3", nullptr, 600);
        RotatingCodeCapture k; k.beginSession(s3);
        string good = k.getCurrentCode();
        istringstream in("S1 0000\nS2 " + good + "\n");
        streambuf* old = cin.rdbuf(in.rdbuf());
        ostringstream sink; streambuf* oldOut = cout.rdbuf(sink.rdbuf());
        string id; bool r = k.captureNext(id);
        cout.rdbuf(oldOut); cin.rdbuf(old);
        CHECK(r && id == "S2", "wrong code retried, then right code captured");
        CHECK(sink.str().find("Incorrect code") != string::npos, "student told the code was wrong");
        istringstream in2("END\n");
        old = cin.rdbuf(in2.rdbuf());
        oldOut = cout.rdbuf(sink.rdbuf());
        bool r2 = k.captureNext(id);
        cout.rdbuf(oldOut); cin.rdbuf(old);
        CHECK(!r2, "END finishes capture");
        istringstream in3("");
        old = cin.rdbuf(in3.rdbuf());
        oldOut = cout.rdbuf(sink.rdbuf());
        bool r3 = k.captureNext(id);
        cout.rdbuf(oldOut); cin.rdbuf(old);
        CHECK(!r3, "closed input ends capture without hanging");
    }
}

static void testPolymorphicRegisterLoop()
{
    cout << "[Dependency inversion: capture -> register]\n";
    Student a("S1", "A", "h", 1, "CE"), b("S2", "B", "h", 1, "CE"), z("S9", "Z", "h", 1, "CE");
    LectureCourse c("CO1", "X", 3, 10);
    c.addTimeSlot(TimeSlot("Mon", 900, 1000, "R"));
    a.enrol(c); b.enrol(c);
    vector<Student*> roster = { &a, &b, &z };

    { ofstream f("roll.txt"); f << "S1\nS2\nS9\nGHOST\n"; }
    AttendanceRegister reg;
    AttendanceSession ses("SES", &c.getSlots()[0], 600);
    FileReplayCapture file("roll.txt");
    AttendanceCapture& cap = file;         // register loop only sees the base class
    cap.beginSession(ses);
    string id; int ok = 0, notEnrolled = 0, unknown = 0;
    while (cap.captureNext(id))
    {
        Student* who = nullptr;
        for (Student* s : roster) if (s->getPersonId() == id) who = s;
        if (!who) { ++unknown; continue; }
        try { reg.markPresent(*who, c, ses); ++ok; }
        catch (const NotEnrolledException&) { ++notEnrolled; }
    }
    cap.endSession();
    CHECK(ok == 2, "two enrolled students marked via base-class reference");
    CHECK(notEnrolled == 1, "non-enrolled student rejected in the loop");
    CHECK(unknown == 1, "unknown id handled by caller");
    CHECK(reg.getSessionCount() == 1, "session counted once");
    CHECK(reg.getPercentage(a) == 100.0, "100% after the only session attended");
}

int main()
{
    cout << unitbuf;
    testTimeSlot();
    testTimetable();
    testEnrolAndDangling();
    testEnrolRules();
    testSession();
    testRegister();
    testFileReplay();
    testRotating();
    testPolymorphicRegisterLoop();
    cout << "\n=== " << passed << " passed, " << failed << " failed ===\n";
    return failed == 0 ? 0 : 1;
}
