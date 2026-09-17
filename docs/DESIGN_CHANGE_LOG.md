# Design Change Log

## Repository constructor
- Change: Added Repository(const std::string& file).
- Reason: Initialise the existing filename attribute when creating a repository.
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