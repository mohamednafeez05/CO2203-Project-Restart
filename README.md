# 🎓 University Course Registration, Timetable & Attendance Management System

A console-based university management system developed in **C++** as part of the **CO2203 – Object Oriented Programming** module at the **University of Sri Jayewardenepura, Faculty of Engineering**.

The project demonstrates the practical application of Object-Oriented Programming concepts through course registration, timetable management, attendance tracking, and persistent data storage.

---

## 🚀 Key Features

### 👨‍🎓 Student
- Login to the system
- Enrol in courses
- Drop courses
- View personal timetable
- Validate prerequisites
- Check course capacity
- Prevent timetable clashes
- Participate in attendance sessions

### 👨‍🏫 Lecturer
- Login using lecturer account
- View assigned courses
- View enrolled students
- Open and close attendance sessions
- Record attendance
- Use attendance capture mechanisms

### 🛠️ Administrator
- Manage user accounts
- Manage course offerings
- Create and update courses
- Assign lecturers
- Manage course capacities and prerequisites
- Generate reports

---

## 🕒 Timetable Management

The system maintains weekly course time slots and detects timetable clashes before allowing students to enrol.

Clash detection is implemented using operator overloading as part of the OOP design.

---

## ✅ Attendance Management

The attendance subsystem supports session-based attendance tracking.

The project uses an abstract `AttendanceCapture` interface with multiple implementations:

- `FileReplayCapture`
- `RotatingCodeCapture`

The active attendance capture method can be selected at runtime using polymorphism.

---

## 💾 Data Persistence

System data is saved and restored between program executions using file handling.

Persistence is handled through:

- `Repository<T>`
- `FileStorage`

This keeps file operations separate from the main application logic.

---

## 🧠 Object-Oriented Programming Concepts Used

- Encapsulation
- Inheritance
- Polymorphism
- Abstract classes
- Virtual functions
- Virtual destructors
- Composition
- Aggregation
- Operator overloading
- Function overriding
- Exception handling
- Custom exception classes
- Templates
- STL containers
- File handling
- Const-correctness
- Static members

---

## 🏗️ Main Class Hierarchies

### Person Hierarchy

```text
Person
├── Student
├── Lecturer
└── Administrator
```

Role-specific behaviour is implemented using virtual functions and runtime polymorphism.



### Course Hierarchy
Course

```text
├── LectureCourse
├── LabCourse
└── ProjectCourse
```

Different course types share common functionality while also implementing specialised behaviour.

### Attendance Capture Hierarchy
AttendanceCapture

```text
├── FileReplayCapture
└── RotatingCodeCapture
```

This allows different attendance capture mechanisms to be used through the same abstract interface.

## 🧩 Core Components

Key components include:

- `Person`, `Student`, `Lecturer`, `Administrator`
- `Course`, `LectureCourse`, `LabCourse`, `ProjectCourse`
- `TimeSlot` and `Timetable`
- `AttendanceSession`, `AttendanceRecord`, `AttendanceRegister`
- `AttendanceCapture`
- `Repository<T>`
- `FileStorage`

---

## 🗂️ System Architecture

The system was designed using UML before implementation.

```text
User Interface
      │
      ▼
Domain Model
      │
      ├── Person Hierarchy
      ├── Course Hierarchy
      ├── Enrolment
      ├── Timetable
      └── Attendance
      │
      ▼
Persistence Layer
      │
      ├── Repository<T>
      └── FileStorage
      │
      ▼
Data Files
```

---

## 🛠️ Technologies Used

- **C++**
- **C++17**
- Object-Oriented Programming
- STL
- File Handling
- UML
- Git
- GitHub
- VS Code
- MinGW / `g++`

---

## 👥 Team

Developed as a **3-member university group project** by **Logic Foundry**.

| Member | Main Responsibility |
|---|---|
| B.G.C.V. Wickramawardana | Domain Model |
| M.L. Abdalla | Scheduling & Attendance Engine |
| M.N.M. Nafeez | Persistence, UI & System Integration |

---

## 👨‍💻 My Contribution

My primary responsibility was the **Persistence and UI layer**, together with system integration.

My work included:

- Working with `Repository<T>`
- File storage and persistence integration
- Connecting system modules through the menu/UI
- Integrating components developed by team members
- Git and GitHub repository management
- Testing and debugging integrated functionality
- Supporting final system integration and demonstration

---

## ▶️ Running the Project

### Requirements

You need:

- A **C++17-compatible compiler**
- `g++`
- MinGW on Windows
- Git

### Clone the Repository

```bash
git clone https://github.com/mohamednafeez05/CO2203-Project-Restart.git
```

### Enter the Project Directory

```bash
cd CO2203-Project-Restart
```

### Compile the Project

On Windows with MinGW:

```bash
mingw32-make
```

Then run the generated executable.

> Build and execution commands may vary depending on the operating system and compiler setup.

---

## 📚 What I Learned

This project provided practical experience in:

- Designing larger object-oriented applications
- Translating UML designs into C++ implementations
- Designing class relationships
- Applying inheritance and polymorphism
- Working with abstract interfaces
- Managing persistent data
- Integrating independently developed modules
- Debugging cross-module issues
- Using Git and GitHub collaboratively
- Working as part of a software development team

---

## 🎯 Academic Context

**Module:** CO2203 – Object Oriented Programming  
**Language:** C++  
**University:** University of Sri Jayewardenepura  
**Faculty:** Faculty of Engineering  
**Department:** Computer Engineering

---

## 📌 Project Status

- ✅ Design completed
- ✅ Implementation completed
- ✅ Integration completed
- ✅ Testing completed
- ✅ Final demonstration completed

---

## 🔐 Demo Accounts

The project supports Student, Lecturer, and Administrator login roles.

For security and privacy, real credentials are not included in this public repository.
Use the provided sample data or create test accounts locally for demonstration.
