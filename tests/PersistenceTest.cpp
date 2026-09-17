#include "AttendanceRegister.h"
#include "FileStorage.h"

#include <fstream>
#include <iostream>
#include <stdexcept>

// Stops the test when a required result is missing.
void check(bool condition, const std::string& message)
{
    if (!condition)
        throw std::runtime_error(message);
}

int main()
{
    try
    {
        const std::string attendanceFile =
            "data/persistence_test/attendance.txt";

        const std::string correctionFile =
            "data/persistence_test/corrections.txt";

        const std::string reason =
            "Lecturer confirmed \"present\" after review";

        std::vector<AttendanceRecord> attendance = {
            AttendanceRecord("S001", "SESSION01", 1700000000)
        };

        std::vector<CorrectionRecord> corrections = {
            CorrectionRecord("S001", "SESSION01", reason, 1700000100),
            CorrectionRecord("S001", "SESSION01",
                             "Follow-up note", 1700000200)
        };

        FileStorage::saveAll(attendance, attendanceFile);
        FileStorage::saveAll(corrections, correctionFile);

        AttendanceRegister original;
        original.load(attendanceFile, correctionFile);
        original.save(attendanceFile, correctionFile);

        AttendanceRegister restored;
        restored.load(attendanceFile, correctionFile);
        restored.load(attendanceFile, correctionFile);

        check(restored.getRecords().size() == 1 &&
              restored.getCorrections().size() == 2,
              "Wrong record counts after repeated load.");

        check(restored.getRecords()[0].getStudentId() == "S001" &&
              restored.getRecords()[0].getSessionId() == "SESSION01" &&
              restored.getRecords()[0].getCheckInTime() == 1700000000,
              "Attendance details changed.");

        check(restored.getCorrections()[0].getStudentId() == "S001" &&
              restored.getCorrections()[0].getSessionId() == "SESSION01" &&
              restored.getCorrections()[0].getReason() == reason &&
              restored.getCorrections()[0].getCorrectionTime() == 1700000100 &&
              restored.getCorrections()[1].getReason() == "Follow-up note" &&
              restored.getCorrections()[1].getCorrectionTime() == 1700000200,
              "Correction details or order changed.");

        std::cout
            << "PASS: round trip, timestamps, quoted reasons and repeated load.\n";

        // Makes the first file valid but different from memory.
        std::vector<AttendanceRecord> changed = {
            AttendanceRecord("S999", "SESSION99", 1700000300)
        };

        FileStorage::saveAll(changed, attendanceFile);

        const std::vector<std::string> invalidFiles = {
            "WRONG_HEADER\n",

            "CORRECTIONS_V1\n"
            "\"S001\" \"SESSION01\" \"Valid\" 1700000100\n"
            "BROKEN\n",

            "CORRECTIONS_V1\n"
            "\"S001\" \"SESSION01\" \"   \" 1700000100\n",

            "CORRECTIONS_V1\n"
            "\"S001\" \"SESSION01\" \"Reason\" -5\n"
        };

        for (const std::string& bad : invalidFiles)
        {
            std::ofstream output(correctionFile);
            output << bad;
            output.close();

            check(static_cast<bool>(output),
                  "Cannot write test fixture.");

            bool rejected = false;

            try
            {
                restored.load(attendanceFile, correctionFile);
            }
            catch (const std::runtime_error&)
            {
                rejected = true;
            }

            check(rejected, "Invalid correction file was accepted.");

            check(restored.getRecords().size() == 1 &&
                  restored.getRecords()[0].getStudentId() == "S001" &&
                  restored.getCorrections().size() == 2 &&
                  restored.getCorrections()[0].getReason() == reason,
                  "Failed loading changed the register.");
        }

        std::cout
            << "PASS: invalid files rejected; both collections preserved.\n";

        restored.save(attendanceFile, correctionFile);

        std::vector<AttendanceRecord> noAttendance;
        std::vector<CorrectionRecord> noCorrections;

        FileStorage::saveAll(noAttendance, attendanceFile);
        FileStorage::saveAll(noCorrections, correctionFile);

        restored.load(attendanceFile, correctionFile);

        check(restored.getRecords().empty() &&
              restored.getCorrections().empty(),
              "Empty collections did not load correctly.");

        std::cout << "PASS: empty collections.\n";
        std::cout << "All persistence checks passed.\n";
    }
    catch (const std::exception& error)
    {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }

    return 0;
}