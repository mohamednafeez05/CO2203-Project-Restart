#include "Course.h"
#include "Student.h"
#include "Lecturer.h"
#include <set>
#include <functional>
#include <cmath>
#include <stdexcept>
#include <algorithm>

bool Course::isFull()
{
    return capacity <= 0 ||
           enrolledStudents.size() >= static_cast<std::size_t>(capacity);
}

const std::vector<TimeSlot>& Course::getSlots() const
{
    return slots;
}

void Course::addStudent(Student& student)
{
    enrolledStudents.push_back(&student);
}

void Course::removeStudent(Student& student)
{
    auto it = std::find(
        enrolledStudents.begin(),
        enrolledStudents.end(),
        &student
    );

    if (it != enrolledStudents.end())
    {
        enrolledStudents.erase(it);
    }
}

bool Course::prerequisitesMet(Student& student)
{
    for (Course* prerequisite : prerequisites)
        if (!student.hasCompletedCourse(*prerequisite) ||
            !prerequisite->prerequisitesMet(student)) return false;
    return true;
}

std::ostream& operator<<(std::ostream& os, const Course& c)
{
    os << "Course Code: " << c.courseCode << std::endl;
    os << "Title: " << c.title << std::endl;

    return os;
}

Course::~Course()
{
}

std::string Course::getCourseCode() const
{
    return courseCode;
}

std::string Course::getTitle() const
{
    return title;
}

int Course::getCreditValue() const
{
    return creditValue;
}

int Course::getCapacity() const
{
    return capacity;
}

Lecturer* Course::getLecturer() const
{
    return lecturer;
}

const std::vector<Course*>& Course::getPrerequisites() const
{
    return prerequisites;
}

const std::vector<Student*>& Course::getEnrolledStudents() const
{
    return enrolledStudents;
}

// Checks the enrolled student references.
bool Course::isStudentEnrolled(const Student& student) const
{
    for (const Student* enrolled : enrolledStudents)
    {
        if (enrolled == &student)
        {
            return true;
        }
    }

    return false;
}

// Initialises and validates course details.
Course::Course(const std::string& code, const std::string& title,
               int credits, int capacity)
    : courseCode(code), title(title), creditValue(credits),
      capacity(capacity), lecturer(nullptr)
{
    if (code.empty() || title.empty() || credits <= 0 || capacity <= 0)
    {
        throw std::invalid_argument("Course details are invalid.");
    }
}

void Course::addTimeSlot(const TimeSlot& slot)
{
    if (!enrolledStudents.empty())
    {
        throw std::logic_error(
            "Cannot change slots while students are enrolled.");
    }

    if (slot.getDay().empty() || slot.getLocation().empty() ||
        slot.getStartTime() < 0 ||
        slot.getEndTime() <= slot.getStartTime())
    {
        throw std::invalid_argument("Course time slot is invalid.");
    }

    for (const TimeSlot& existing : slots)
    {
        if (existing == slot)
        {
            throw std::invalid_argument("Course time slots overlap.");

        }
    }

    slots.push_back(slot);
}
void Course::addPrerequisite(Course& course)
{
    std::set<const Course*> visited;
    std::function<bool(const Course*)> reaches = [&](const Course* item) {
        if (item == this) return true;
        if (!visited.insert(item).second) return false;
        for (const auto* next : item->getPrerequisites())
            if (reaches(next)) return true;
        return false;
    };
    if (reaches(&course)) throw std::invalid_argument("Prerequisite cycle rejected.");
    if (!enrolledStudents.empty()) throw std::logic_error("Drop enrolments before changing prerequisites.");
    if (std::find(prerequisites.begin(), prerequisites.end(), &course) == prerequisites.end())
        prerequisites.push_back(&course);
}
void Course::setLecturer(Lecturer& teacher)
{
    if (lecturer == &teacher) return;
    teacher.addAssignedCourse(*this);
    if (lecturer) lecturer->removeAssignedCourse(*this);
    lecturer = &teacher;
}
void Course::updateDetails(const std::string& name, int credits, int limit)
{
    if (name.empty() || credits <= 0 || limit <= 0 ||
        static_cast<std::size_t>(limit) < enrolledStudents.size())
        throw std::invalid_argument("Invalid course details or capacity below enrolment.");
    title = name; creditValue = credits; capacity = limit;
}
void Course::setScores(const std::vector<double>& values)
{
    for (double score : values)
        if (!std::isfinite(score) || score < 0 || score > 100)
            throw std::invalid_argument("Scores must be between 0 and 100.");
    assessmentScores = values;
}
const std::vector<double>& Course::getScores() const { return assessmentScores; }
