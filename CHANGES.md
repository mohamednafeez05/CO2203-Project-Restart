# Member 2 – Change Log (Scheduling & Attendance)

Project: CO2203 University Course Registration, Timetable & Attendance System
Owner: Member 2 (Abdalla M.L, 24/ENG/002)

All changes below were verified by an automated test suite (99 checks) run
under AddressSanitizer + UBSan, and by compiling and running the full merged
project (Members 1, 2 and 3 together).

Status column: **Approved by** must be filled in by the team once agreed.

| # | File(s) | What changed | Why | Approved by |
|---|---------|--------------|-----|-------------|
| 1 | `Timetable.h/.cpp` | Timetable now stores **copies** of `TimeSlot`s instead of `const TimeSlot*` pointing into `Course::slots`. `removeTimeSlot` removes the first slot that is **identical** (day, times, location). | `Course::slots` is a `std::vector`; adding a slot to a course after students enrolled could move the vector and leave every student's timetable pointing at freed memory (heap-use-after-free, reproduced with ASan). Exact-match removal avoids deleting neighbours, because `operator==` means "overlaps". | |
| 2 | `TimeSlot.h/.cpp` | Added `isSameSlot()` (exact field match). `overlaps()` / `operator==` unchanged. | Needed by change 1. | |
| 3 | `AttendanceException.h/.cpp`, `SessionClosedException`, `NotEnrolledException`, `DuplicateAttendanceException` (new) | Added the exception hierarchy shown in the UML. | UML and D1 section 7 promise these typed exceptions. The old code threw `std::runtime_error`. | |
| 4 | `AttendanceRegister.h/.cpp` | `markPresent` throws the typed exceptions. Register now owns its `sessions`. Added `addSession`, `getSessionCount`, `getPercentage(student)`, `getPercentage(student, course)` (UML signature), `getEligibilityReport(course)`, static `eligibilityThreshold` (+ getter/setter). Explicit destructor, copy constructor, copy assignment (Rule of Three). | UML lists sessions, `getPercentage`, `getEligibilityReport` and static `eligibilityThreshold`. D1 assigns the Rule of Three to `AttendanceRegister`. Percentages no longer depend on the caller supplying the session total. | |
| 5 | `AttendanceRegister`, `CorrectionRecord.h/.cpp` | `addCorrection(student, course, sessionId, lecturer, reason)` now actually **changes attendance**, requires the lecturer to be assigned to the course and a non-blank reason, and is logged. New `removeAttendance(...)` for wrong records; also logged. `CorrectionRecord` now stores lecturer id and action. Nothing is ever deleted from the audit trail. | FR7.3 requires audited corrections. Previously a "correction" was only a note and changed nothing. | |
| 6 | `AttendanceSession.h/.cpp` | Added `isExpired()` and `getDurationSeconds()`. Constructor contract unchanged. | UML lists `isExpired()`. Member 1's `Lecturer::openAttendanceSession` still works (returns by value). | |
| 7 | `RotatingCodeCapture.h/.cpp` | Code now **expires** (`codeExpiry`) and is regenerated after each successful check-in. New non-blocking `submitCode(code)`. `captureNext` keeps asking after a wrong code and stops on `END`/closed input. Uses `<random>`. | A wrong code used to make `captureNext` return `false`, which a `while (captureNext(id))` loop reads as "session finished", so one typo ended attendance for the whole class. UML lists `codeExpiry` and `regenerateCode()`. | |
| 8 | `FileReplayCapture.h/.cpp` | Skips blank lines and trims spaces / Windows `\r`. Closes the file in the destructor. Copying disabled. | A trailing newline used to throw "Malformed record" and abort a valid replay. | |
| 9 | `AttendanceRecord`, `AttendanceCapture` | Cosmetic (const references, comments). Interface unchanged. | Consistency. | |

## Interface compatibility with Members 1 and 3

Unchanged and still valid:
`Timetable::addTimeSlot(const TimeSlot&)`, `removeTimeSlot(const TimeSlot&)`,
`hasClashWith(const TimeSlot&)`, `TimeSlot::overlaps`, `TimeSlot::operator==`,
`AttendanceSession(id, const TimeSlot*, seconds)`, `open()`, `close()`,
`isOpen()`, `AttendanceSession` is copyable, `AttendanceRegister::markPresent(Student&, Course&, AttendanceSession&)`,
`AttendanceCapture` (`beginSession`, `captureNext`, `endSession`).

Added, not breaking: everything listed as "added" above.

Changed signature (nobody else calls it yet): `AttendanceRegister::addCorrection`
(was `addCorrection(studentId, sessionId, reason)`).

## Things to update in the final UML

* `Timetable`: `weeklySlots : vector<TimeSlot>`, `addTimeSlot`, `removeTimeSlot`, `hasClashWith` (UML shows `addSlot` / `hasClash`).
* `TimeSlot`: add `isSameSlot()`.
* `AttendanceRegister`: add `addSession`, `getSessionCount`, `removeAttendance`, both `getPercentage` overloads, `markPresent`; `sessions` stored by value.
* `AttendanceSession`: `markPresent` and `setCapture` from the UML live on the register / are not used; add `getDurationSeconds`.
* `CorrectionRecord`: `lecturerId`, `action`.
* `RotatingCodeCapture`: add `submitCode`, `validateCode`, `isCodeExpired`.
* Capture `captureNext` returns `bool` and fills a `string&` (UML shows `string`).
* `AttendanceException` and its three subclasses as implemented.

## How to build

    g++ -std=c++17 -Iinclude src/*.cpp -o app            # whole project
    g++ -std=c++17 -Iinclude tests/member2_tests.cpp $(ls src/*.cpp | grep -v main.cpp) -o m2tests && ./m2tests
    g++ -std=c++17 -Iinclude demo/member2_demo.cpp  $(ls src/*.cpp | grep -v main.cpp) -o m2demo  && ./m2demo
