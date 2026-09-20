# Design Change Log

### Student and history storage integration (2026-09-18)
- Added StudentHistoryStore to own and load three collections together.
- Attendance and correction student IDs must match loaded students.
- Failed loads preserve all previous in-memory collections.
- Saving submits all three encoded files to SafeFile in one attempt.
- AttendanceConsole now lists students and joins profiles with history.
- Exposes only const collection access; returned student pointers expire on successful reload.
- This store covers student profiles and history, not course/enrolment ownership.
- Added isolated integration tests, test runner and sample data.
- Proposed by: Member 1.
- Approved by: Member 2 and 3

## Repository constructor
- Change: Added Repository(const std::string& file).
- Reason: Initialise the existing filename attribute when creating a repository.
- Proposed by: Member 3.
- Approved by: Member 2 and 1

### Attendance history console

- Added AttendanceConsole to load, display, filter and save history.
- Added an optional command-line data-directory argument for isolated tests.
- Startup stops if one data file is missing or either file is invalid.
- Moved the earlier main demonstrations into tests/LegacySmokeTest.cpp.
- Authentication and role-menu integration remain pending.
- Proposed by: Member 2
- Approved by: Member 3 and 1

### Safer file saving

- Added SafeFile to stage writes and retain previous-file backups.
- Split FileStorage encoding from disk writing.
- AttendanceRegister submits both collections in one save attempt.
- Replacement errors trigger an attempt to restore previous files.
- AttendanceConsole rejects leftover temporary files on startup.
- Scope: one running application; no power-loss durability guarantee.
- Proposed by: Member 3.
- Approved by: Member 2 and 1


### Attendance persistence integration

- Added a timestamp-restoring constructor to CorrectionRecord.
- Added FileStorage overloads for correction collections.
- Added AttendanceRegister save/load methods for both collections.
- Loading validates both files before replacing either collection.
- Reason: preserve the audit history across program restarts.
- Proposed by: Member 3.
- Approved by: Member 2 and 1
