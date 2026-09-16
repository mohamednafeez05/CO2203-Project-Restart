#include <iostream>
#include <string>
#include "Repository.h"

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

    return 0;
}