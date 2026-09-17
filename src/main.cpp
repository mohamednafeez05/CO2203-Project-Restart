#include <exception>
#include <iostream>
#include <string>
#include "Repository.h"
#include "Student.h"
#include "ConsoleInput.h"

using namespace std;

int main() {
    // Temporary repository test.
    Repository<int> marks("data/marks.txt");

    int firstMark = 75;
    int secondMark = 82;

    marks.add(firstMark);
    marks.add(secondMark);

    cout << "Marks stored: " << marks.getAll().size() << '\n';

    for (const int& mark : marks.getAll()) {
        cout << mark << '\n';
    }

    Repository<string> courses("data/courses.txt");

    string firstCourse = "CO2203";
    string secondCourse = "CO2204";

    courses.add(firstCourse);
    courses.add(secondCourse);

    cout << "Courses stored: " << courses.getAll().size() << '\n';

    for (const string& course : courses.getAll()) {
        cout << course << '\n';
    }

    // Checks student construction and inherited getters.
    Student student("S001", "Nafeez", "TEST_HASH_ONLY",
                    2, "Computer Engineering");

    cout << "Student: " << student.getPersonId()
         << " | " << student.getName()
         << " | Year " << student.getYearOfStudy()
         << " | " << student.getMajor() << '\n';

    cout << "Enrolled courses: "
         << student.getEnrolledCourses().size() << '\n';

    // Checks student saving and loading.
    try
    {
        Repository<Student> students("data/students_test.txt");
        students.add(student);
        // students.save();

        Repository<Student> restored("data/students_test.txt");

        Student existing("S999", "Existing Student", "TEST_HASH_ONLY",
                         1, "Computer Engineering");
        restored.add(existing);

    // Checks whether a failed load preserves existing data.
    try
        {
            restored.load();
        }
        catch (const std::exception& error)
        {
            cout << "Load rejected: " << error.what() << '\n';
        }

        cout << "Loaded students: "
             << restored.getAll().size() << '\n';

        for (const Student& loaded : restored.getAll())
        {
            cout << loaded.getPersonId() << " | "
                 << loaded.getName() << " | "
                 << loaded.getYearOfStudy() << " | "
                 << loaded.getMajor() << '\n';
        }
    }
    catch (const std::exception& error)
    {
        cerr << "Storage error: " << error.what() << '\n';
        return 1;
    }

    // Checks reusable console input validation.
    try
    {
        int choice = ConsoleInput::readInt(
            "Enter a test menu choice (1-4): ", 1, 4);

        std::string name = ConsoleInput::readText(
            "Enter a test name: ");

        cout << "Accepted choice: " << choice << '\n';
        cout << "Accepted name: " << name << '\n';
    }
    catch (const std::exception& error)
    {
        cerr << "Input error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}