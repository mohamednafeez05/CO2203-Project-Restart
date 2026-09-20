# Student/history integration batch

This batch joins student profiles with attendance and correction history. It is
an integration viewer, not the final authenticated university application.

## Install

From the project root, extract the supplied update archive to a separate folder.
Copy its include, src, tests, scripts, examples and docs folders into the project,
allowing replacement of the listed existing files. Copy its .gitignore as well.
The archive contains only changed/new files, not a complete replacement project.
Do not delete the original project directories.

Replaced: include/AttendanceConsole.h, src/AttendanceConsole.cpp, src/main.cpp,
docs/DESIGN_CHANGE_LOG.md and .gitignore.
Added: include/StudentHistoryStore.h, src/StudentHistoryStore.cpp,
tests/StudentHistoryTest.cpp, scripts/Test.ps1, this guide and three example files.

## Build and automated checks (PowerShell, project root)

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\Test.ps1
```

The execution-policy setting applies only to this process. The script builds the
application and each of the three separate test programs with C++17 and -Wall.
It checks native exit codes, stops on failure, and runs tests in a new temporary
folder on every invocation. The folder is retained for inspection. Existing data
and backup files are not used. LegacySmokeTest remains a separate earlier demo.

Expected final output:

Application build and all three test suites passed.


## Interactive demonstration

Use a new folder name if data/student_history_demo already exists:

```powershell
New-Item -ItemType Directory data/student_history_demo
Copy-Item examples/student-history/*.txt data/student_history_demo/
.\mainApp.exe data/student_history_demo
```

Try 5 to list two students; 3 then S001 for a profile, two attendance entries and
one correction; 3 then S999 for Student not found; 4 for totals 2 students,
3 attendance entries, 1 correction, 2 students represented, 2 sessions represented.
Choose 0 to save all three files. Run the same command again and confirm the
same counts. Credentials are not displayed. Example credential fields are
obvious dummy values, not secure hashes or real accounts.

## Existing data and compatibility

The dataset now requires students.txt, attendance.txt and corrections.txt together.
A new folder with none starts empty. A partially populated folder stops startup.
An old attendance-only folder must be supplemented with a STUDENTS_V1 students.txt
containing the real matching student IDs and details. Do not copy the example
students over real student records. Work on a copy of an old dataset first.
Unknown student IDs in either history file cause rejection before any live
collection is changed. A correction need not match an existing attendance record:
it may describe missing attendance. A rejected load never writes or repairs files.

Temporary files stop startup; backups without originals also stop startup.
Recover a consistent set of files manually after inspection. SafeFile attempts
rollback on ordinary replacement errors; this is not atomic storage across
multiple files, does not guarantee power-loss recovery and assumes one app instance.

## Architecture

StudentHistoryStore owns three vectors. load builds temporary vectors, validates
cross-file student IDs, then swaps all three into place. save encodes all three
before sending them to SafeFile in one operation. The menu delegates file work
to the store. Const getters prevent the viewer from changing collections directly.
findStudent returns a borrowed pointer; successful reload invalidates it.
The viewer uses it only during immediate output and never retains it across reloads.


