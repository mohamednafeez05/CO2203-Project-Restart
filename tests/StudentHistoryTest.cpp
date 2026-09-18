#include "StudentHistoryStore.h"
#include "AttendanceConsole.h"
#include "FileStorage.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace fs = std::filesystem;
void check(bool ok, const std::string& message)
{
    if (!ok) throw std::runtime_error(message);
}
void put(const fs::path& path, const std::string& text)
{
    std::ofstream file(path);
    file << text;
    file.close();
    check(bool(file), "Fixture write failed.");
}
std::string read(const fs::path& path)
{
    std::ifstream file(path);
    check(bool(file), "Fixture read failed.");
    return std::string(std::istreambuf_iterator<char>(file), {});
}
void fixture(const fs::path& folder)
{
    fs::create_directories(folder);
    put(folder / "students.txt", "STUDENTS_V1\n\"S001\" \"Nafeez\" \"test-secret\" 2 \"Computer Engineering\"\n\"S002\" \"Demo\" \"test-secret\" 1 \"Engineering\"\n");
    put(folder / "attendance.txt", "ATTENDANCE_V1\n\"S001\" \"SESSION01\" 1700000000\n\"S002\" \"SESSION01\" 1700000060\n\"S001\" \"SESSION02\" 1700086400\n");
    put(folder / "corrections.txt", "CORRECTIONS_V1\n\"S001\" \"SESSION01\" \"Reviewed\" 1700000100\n");
}
template<class Action> void rejects(Action action)
{
    bool rejected = false;
    try { action(); } catch (const std::exception&) { rejected = true; }
    check(rejected, "Invalid operation unexpectedly succeeded.");
}
// Restore console streams even when the action throws.
struct ConsoleStreams
{
    std::istringstream input;
    std::ostringstream output;
    std::streambuf* oldInput;
    std::streambuf* oldOutput;
    std::streambuf* oldError;
    explicit ConsoleStreams(const std::string& text) : input(text),
        oldInput(std::cin.rdbuf(input.rdbuf())),
        oldOutput(std::cout.rdbuf(output.rdbuf())),
        oldError(std::cerr.rdbuf(output.rdbuf())) {}
    ~ConsoleStreams()
    {
        std::cin.rdbuf(oldInput);
        std::cout.rdbuf(oldOutput);
        std::cerr.rdbuf(oldError);
    }
    ConsoleStreams(const ConsoleStreams&) = delete;
    ConsoleStreams& operator=(const ConsoleStreams&) = delete;
};
int main()
{
    try
    {
        const fs::path folder("data/student_history_test");
        check(!fs::exists(folder), "Use a fresh test working directory.");
        fixture(folder);
        StudentHistoryStore store;
        store.load(folder.string());
        store.load(folder.string());
        check(store.getStudents().size() == 2 && store.getRecords().size() == 3 &&
              store.getCorrections().size() == 1, "Repeated load duplicated data.");
        check(store.findStudent("S001")->getName() == "Nafeez" &&
              !store.findStudent("S999"), "Student lookup failed.");
        store.save(folder.string());
        for (const char* name : {"students.txt", "attendance.txt", "corrections.txt"})
            check(read(folder / name) == read(folder / (std::string(name) + ".bak")), "Backup mismatch.");
        StudentHistoryStore restarted;
        restarted.load(folder.string());
        check(restarted.getRecords()[0].getCheckInTime() == 1700000000 &&
              restarted.getCorrections()[0].getReason() == "Reviewed", "Restart changed data.");
        std::cout << "PASS: linked load, repeated load, coordinated save and restart.\n";

        for (const char* name : {"students.txt", "attendance.txt", "corrections.txt"})
        {
            fixture(folder);
            put(folder / name, "BROKEN\n");
            rejects([&] { store.load(folder.string()); });
            check(store.getStudents().size() == 2 && store.getRecords().size() == 3 &&
                  store.getCorrections().size() == 1, "Failed load changed collections.");
            check(read(folder / name) == "BROKEN\n", "Load wrote to disk.");
        }
        fixture(folder);
        put(folder / "students.txt", "STUDENTS_V1\n\"S001\" \"Changed\" \"secret\" 1 \"Math\"\n");
        rejects([&] { store.load(folder.string()); });
        check(store.findStudent("S001")->getName() == "Nafeez", "Unknown attendance ID replaced students.");
        fixture(folder);
        put(folder / "corrections.txt", "CORRECTIONS_V1\n\"S999\" \"SESSION01\" \"Reviewed\" 1700000100\n");
        rejects([&] { store.load(folder.string()); });
        std::cout << "PASS: corrupt files and unknown student references preserve live data.\n";

        for (const char* name : {"students.txt", "attendance.txt", "corrections.txt"})
        {
            fixture(folder);
            fs::remove(folder / name);
            rejects([&] { store.load(folder.string()); });
        }
        fixture(folder);
        for (const char* name : {"students.txt.tmp", "attendance.txt.tmp", "corrections.txt.tmp"})
        {
            put(folder / name, "unfinished");
            rejects([&] { store.load(folder.string()); });
            rejects([&] { store.save(folder.string()); });
            check(read(folder / name) == "unfinished", "Existing temp file changed.");
            fs::remove(folder / name);
        }
        const std::string originalStudents = read(folder / "students.txt");
        const std::string originalAttendance = read(folder / "attendance.txt");
        const std::string originalCorrections = read(folder / "corrections.txt");
        fs::remove(folder / "corrections.txt.bak");
        fs::create_directory(folder / "corrections.txt.bak");
        rejects([&] { store.save(folder.string()); });
        check(read(folder / "students.txt") == originalStudents &&
              read(folder / "attendance.txt") == originalAttendance &&
              read(folder / "corrections.txt") == originalCorrections,
              "Rejected coordinated save changed originals.");
        fs::remove(folder / "corrections.txt.bak");
        std::cout << "PASS: failed save preserves all three originals.\n";
        const fs::path backupOnly = folder / "backup_only";
        fs::create_directories(backupOnly);
        put(backupOnly / "students.txt.bak", "recover me");
        rejects([&] { store.load(backupOnly.string()); });
        const fs::path empty = folder / "new_install";
        StudentHistoryStore fresh;
        fresh.load(empty.string());
        fresh.save(empty.string());
        restarted.load(empty.string());
        check(restarted.getStudents().empty() && restarted.getRecords().empty(), "Fresh data not empty.");
        std::cout << "PASS: missing files, unfinished saves, backup-only folders and fresh startup.\n";

        fixture(folder);
        std::string transcript;
        {
            ConsoleStreams io("abc\n99\n1.5\n5\n3\n  \nS001\n3\nS999\n4\n0\n");
            AttendanceConsole app(folder.string());
            app.run();
            transcript = io.output.str();
        }
        for (const char* expected : {"Enter a whole number", "Input cannot be empty",
             "Registered students: 2", "Student: S001 | Nafeez", "Student not found: S999",
             "Attendance entries: 3", "Correction entries: 1", "data saved."})
            check(transcript.find(expected) != std::string::npos, std::string("Missing console output: ") + expected);
        check(transcript.find("test-secret") == std::string::npos, "Console exposed credential field.");
        const std::string before = read(folder / "attendance.txt");
        {
            ConsoleStreams io("");
            AttendanceConsole app(folder.string());
            rejects([&] { app.run(); });
        }
        check(read(folder / "attendance.txt") == before, "EOF changed saved history.");
        std::cout << "PASS: console profiles, search, summary, invalid input and EOF.\n";
        std::cout << "All student-history checks passed.\n";
    }
    catch (const std::exception& error)
    {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}
