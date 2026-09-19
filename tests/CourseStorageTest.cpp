#include "StudentHistoryStore.h"
#include "FileStorage.h"
#include "SafeFile.h"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

std::string readText(const std::filesystem::path& path)
{
    std::ifstream input(path);
    std::ostringstream text;
    text << input.rdbuf();
    return text.str();
}

int main()
{
    namespace fs = std::filesystem;

    const fs::path folder = "data/course_storage_test";
    assert(!fs::exists(folder));
    fs::create_directories(folder);

    std::vector<Student> students;
    students.emplace_back(
        "S001", "Nafeez", "TEST_ONLY", 2, "Engineering");

    SafeFile::writeAll({
        {(folder / "students.txt").string(), FileStorage::encode(students)},
        {(folder / "attendance.txt").string(), "ATTENDANCE_V1\n"},
        {(folder / "corrections.txt").string(), "CORRECTIONS_V1\n"}
    });

    const std::string catalogue =
        "COURSE_ENROLMENTS_V1\n"
        "COURSE \"LECTURE\" \"CO2203\" \"Object Oriented Programming\" 3 30\n"
        "SLOT \"CO2203\" \"Monday\" 540 600 \"Room A\"\n"
        "COURSE \"LAB\" \"CO2204\" \"Programming Lab\" 1 20\n"
        "SLOT \"CO2204\" \"Monday\" 600 660 \"Lab B\"\n";

    const auto path = folder / "course_enrolments.txt";

    SafeFile::writeAll({
        {path.string(), catalogue + "ENROL \"S001\" \"CO2203\"\n"}
    });

    StudentHistoryStore store;
    store.load(folder.string());

    assert(store.findStudent("S001")->getEnrolledCourses().size() == 1);
    assert(store.findCourse("CO2203")->isStudentEnrolled(
        *store.findStudent("S001")));

    store.enrol("S001", "CO2204");
    store.save(folder.string());

    StudentHistoryStore restarted;
    restarted.load(folder.string());

    assert(restarted.findStudent("S001")->getEnrolledCourses().size() == 2);
    assert(restarted.findCourse("CO2204")->getSlots().front().getLocation()
           == "Lab B");

    restarted.load(folder.string());
    assert(restarted.findStudent("S001")->getEnrolledCourses().size() == 2);

    restarted.drop("S001", "CO2203");
    restarted.save(folder.string());
    store.load(folder.string());

    assert(store.findStudent("S001")->getEnrolledCourses().size() == 1);
    assert(store.findCourse("CO2203")->getEnrolledStudents().empty());

    std::cout
        << "PASS: course types, slots, enrolments, dropping and restart.\n";

    const std::string good = readText(path);
    const Student* before = store.findStudent("S001");

    for (const std::string& bad : {
        catalogue + "ENROL \"S999\" \"CO2203\"\n",
        catalogue + "ENROL \"S001\" \"MISSING\"\n",
        catalogue + "ENROL \"S001\" \"CO2203\"\nENROL \"S001\" \"CO2203\"\n",
        catalogue + "COURSE \"PROJECT\" \"P1\" \"Project\" 3 20\n",
        catalogue + "SLOT \"CO2203\" \"Monday\" 550 590 \"Room C\"\n"})
    {
        SafeFile::writeAll({{path.string(), bad}});
        bool rejected = false;

        try
        {
            store.load(folder.string());
        }
        catch (const std::exception&)
        {
            rejected = true;
        }

        assert(rejected);
        assert(store.findStudent("S001") == before);
        assert(before->getEnrolledCourses().size() == 1);
    }

    SafeFile::writeAll({{path.string(), good}});

    std::cout
        << "PASS: invalid links, duplicates and course rows preserve live data.\n";

    const std::string names[] = {
        "students.txt", "attendance.txt", "corrections.txt",
        "course_enrolments.txt", "dataset.txt"
    };

    std::vector<std::string> originals;

    for (const auto& name : names)
        originals.push_back(readText(folder / name));

    fs::remove(folder / "students.txt.bak");
    fs::create_directory(folder / "students.txt.bak");

    bool rejected = false;

    try
    {
        store.save(folder.string());
    }
    catch (const std::exception&)
    {
        rejected = true;
    }

    assert(rejected);

    for (unsigned i = 0; i < 5; ++i)
        assert(readText(folder / names[i]) == originals[i]);

    fs::remove(folder / "students.txt.bak");
    fs::remove(path.string() + ".bak");
    fs::rename(path, path.string() + ".missing");

    rejected = false;

    try
    {
        store.load(folder.string());
    }
    catch (const std::exception&)
    {
        rejected = true;
    }

    assert(rejected);
    assert(store.findStudent("S001") == before);

    fs::rename(path.string() + ".missing", path);
    std::ofstream(path.string() + ".tmp") << "unfinished";

    rejected = false;

    try
    {
        store.load(folder.string());
    }
    catch (const std::exception&)
    {
        rejected = true;
    }

    assert(rejected);
    assert(store.findStudent("S001") == before);

    std::cout
        << "PASS: failed save, missing course file and unfinished save.\n";
}