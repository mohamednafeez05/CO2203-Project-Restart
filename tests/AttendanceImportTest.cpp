#include "StudentHistoryStore.h"
#include "AttendanceRegister.h"
#include "FileStorage.h"
#include "SafeFile.h"
#include <cassert>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include "AttendanceConsole.h"
#include <sstream>

int main()
{
    namespace fs = std::filesystem;
    const fs::path folder = "data/attendance_import_test";
    assert(!fs::exists(folder));
    fs::create_directories(folder);

    std::vector<Student> students;
    students.emplace_back(
        "S001", "Nafeez", "TEST_ONLY", 2, "Engineering");

    SafeFile::writeAll({
        {(folder / "students.txt").string(),
         FileStorage::encode(students)},
        {(folder / "attendance.txt").string(), "ATTENDANCE_V1\n"},
        {(folder / "corrections.txt").string(), "CORRECTIONS_V1\n"}
    });

    StudentHistoryStore store;
    store.load(folder.string());

    const auto attendance =
        (folder / "batch_attendance.txt").string();
    const auto corrections =
        (folder / "batch_corrections.txt").string();

    auto makeBatch = [&](const std::string& id,
                         const std::string& session,
                         const std::string& noteId)
    {
        FileStorage::saveAll(std::vector<AttendanceRecord>{
            AttendanceRecord(id, session, 1700000000)
        }, attendance);

        FileStorage::saveAll(std::vector<CorrectionRecord>{
            CorrectionRecord(
                noteId, session, "Reviewed entry", 1700000001)
        }, corrections);

        AttendanceRegister batch;
        batch.load(attendance, corrections);
        return batch;
    };

    auto valid = makeBatch("S001", "SESSION1", "S001");
    store.importAttendance(valid);
    assert(store.getRecords().size() == 1);
    assert(store.getCorrections().size() == 1);

    auto expectRejected = [&](const AttendanceRegister& batch)
    {
        bool rejected = false;

        try
        {
            store.importAttendance(batch);
        }
        catch (const std::runtime_error&)
        {
            rejected = true;
        }

        assert(rejected);
        assert(store.getRecords().size() == 1);
        assert(store.getCorrections().size() == 1);
    };

    expectRejected(valid);
    expectRejected(makeBatch("UNKNOWN", "SESSION2", "S001"));
    expectRejected(makeBatch("S001", "SESSION2", "UNKNOWN"));

    AttendanceRegister repeatedNote;
    repeatedNote.addCorrection(
        "S001", "SESSION3", "Reviewed entry");
    store.importAttendance(repeatedNote);

    bool duplicateNote = false;

    try
    {
        store.importAttendance(repeatedNote);
    }
    catch (const std::runtime_error&)
    {
        duplicateNote = true;
    }

    assert(duplicateNote);
    assert(store.getCorrections().size() == 2);

    store.save(folder.string());

    StudentHistoryStore restored;
    restored.load(folder.string());

    assert(restored.getRecords().size() == 1);
    assert(restored.getCorrections().size() == 2);
    assert(restored.getRecords()[0].getCheckInTime() == 1700000000);
    assert(restored.getCorrections()[0].getCorrectionTime() == 1700000001);
    assert(restored.getCorrections()[0].getReason() == "Reviewed entry");

    makeBatch("S001", "SESSION4", "S001");

    const std::string command =
        "10\n" + attendance + "\n" + corrections + "\n";

    std::istringstream input(command + command + "0\n");
    std::ostringstream output;

    auto* previousInput = std::cin.rdbuf(input.rdbuf());
    auto* previousOutput = std::cout.rdbuf(output.rdbuf());

    try
    {
        AttendanceConsole console(folder.string());
        console.run();
    }
    catch (...)
    {
        std::cin.rdbuf(previousInput);
        std::cout.rdbuf(previousOutput);
        throw;
    }

    std::cin.rdbuf(previousInput);
    std::cout.rdbuf(previousOutput);

    assert(output.str().find(
        "Imported 1 attendance entries and 1 correction notes.") !=
        std::string::npos);

    assert(output.str().find("Import failed:") != std::string::npos);

    restored.load(folder.string());
    assert(restored.getRecords().size() == 2);
    assert(restored.getCorrections().size() == 3);

    std::cout
        << "PASS: attendance transfer, rejected batches and restart.\n";
}