# Logic Foundry - CO2203 integration candidate

University Course Registration, Timetable and Attendance Management System.


## Windows build and tests

use y MSYS2 g++ on PATH. From this folder in PowerShell:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\Test.ps1
.\mainApp.exe .\examples\demo
```

The test runner compiles each shared source once and links eight independent suites. It uses a fresh temporary test directory; it does not change the demonstration dataset. Expected final line: `Application build and all eight test suites passed.`

For a source-only build with GNU make: `mingw32-make` (or `make` in MSYS2).

## Linux build and tests

```bash
make
bash scripts/Test.sh
./mainApp.exe examples/demo
```

`mainApp.exe` is the chosen target name on both platforms; build it locally. No executable is included in the submission archive.

## Demonstration credentials

| Role | ID | Password |
|---|---|---|
| Administrator | A001 | admin123 |
| Lecturer, owns demo courses | L001 | lecturer123 |
| Lecturer, owns no courses | L002 | lecturer123 |
| Student | S001 | student123 |
| Student | S002 | student123 |

These are artificial demonstration credentials. The inherited `passwordHash` field currently stores a directly compared credential

For a populated report/audit example, run `mainApp.exe examples/completed-demo` and use the same logins. It contains two closed sessions, original attendance and corrections from the integration test.

To reset, create a NEW directory:

```powershell
.\mainApp.exe --init-demo .\demo-fresh
.\mainApp.exe .\demo-fresh
```

Initialization refuses an existing directory. The main application expects `<folder>/system.txt`. Changes are saved after successful mutation and on logout. Failed saves retain pending in-memory changes; fix the path problem and retry saving before closing.

## File formats and migration

The main application's `SystemStorage` owns a single `UNIVERSITY_V1` snapshot containing all three user roles, all three course types, scores, slots, prerequisites, completions, enrolments, assignments, session times/rosters, immutable original records and append-only corrections. `END` detects truncation. `SafeFile` stages `.tmp`, backs up `.bak`, and replaces the snapshot.

Load rebuilds and validates a temporary object graph before swapping it into live state. Session times are preserved, but sessions restart CLOSED and codes are discarded. Open a new session after restart; this avoids reviving expired credentials.

The older `FileStorage`, `CourseStorage`, `StudentHistoryStore` and `AttendanceConsole` remain for historical compatibility tests. Their separate V1/V2/V3 datasets are NOT automatically migrated to the new role-aware snapshot. They reject new metadata they cannot preserve. Keep your existing data folders as backups; use the supplied demo for the integrated application.

## Key rules

- Student actions use the authenticated student; an ID cannot impersonate another student.
- Lecturer session, capture, correction and audit actions require current course ownership.
- A session snapshots its eligible roster at opening. Students enrolled later join the next session.
- Counts apply the latest correction without deleting original records. Reports use sessions whose opening roster includes the student; course percentages weight eligible student/session pairs. Opened sessions count immediately; zero eligible sessions display N/A.
- `TimeSlot::operator==` means overlap, not exact identity. Minutes are measured after midnight; weekdays normalize full names and three-letter abbreviations.
- Adding slots after enrolment or session history is blocked to preserve referenced slot addresses.
- Removing referenced users/courses is blocked rather than erasing historical relationships.
- Replay expects one student ID per nonblank line. Unknown/unregistered IDs and duplicates are rejected individually. A malformed line stops that replay, closes the session, and preserves earlier accepted events.
- Rotating codes expire after 60 seconds. After expiry, lecturer menu 5 -> Rotating code creates a new code while the session remains open. Use one running process: log out of the lecturer menu, sign in as student, and enter the code there.
- Course grade calculations demonstrate course-type polymorphism; these are course-level example scores, not a per-student gradebook.

## Contents

`include/`, `src/`: code; `tests/`, `scripts/`, `Makefile`: build/tests.
`examples/`: artificial sample data and replay files.
`docs/PROJECT_REPORT.pdf`: eight-page report with signature spaces.
`docs/FINAL_UML.pdf` and `docs/FINAL_UML.drawio`: readable diagram and editable source.
`docs/DESIGN_CHANGE_LOG.md`: final dated changes and baseline deviations.
`docs/TEST_RESULTS.txt`: actual local test output.
`docs/DEMO_GUIDE.md`: concise demo sequence.

