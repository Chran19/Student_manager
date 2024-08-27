#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <iomanip>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

using namespace std;

// OpenGL and GLFW initialization
void initializeOpenGL(GLFWwindow*& window, int width, int height, const char* title) {
    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW" << endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    if (glewInit() != GLEW_OK) {
        cerr << "Failed to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

// StudentManager class (same as before)
class StudentManager {
    struct Student {
        string name;
        vector<int> grades;
    };

    map<int, Student> students;

    void saveToFile(const string& filename) const;
    void loadFromFile(const string& filename);

public:
    StudentManager() {
        loadFromFile("students_data.txt");
    }

    ~StudentManager() {
        saveToFile("students_data.txt");
    }

    void addStudent();
    void addGrade();
    void displayGrades() const;
    void calculateAverage() const;
    void editStudent();
    void deleteStudent();
};

// Save and load functions (same as before)
void StudentManager::saveToFile(const string& filename) const {
    ofstream file(filename);
    if (file.is_open()) {
        for (const auto& [id, student] : students) {
            file << id << ' ' << student.name << ' ';
            file << student.grades.size() << ' ';
            for (int grade : student.grades) {
                file << grade << ' ';
            }
            file << endl;
        }
        file.close();
    } else {
        cerr << "Error opening file for writing!" << endl;
    }
}

void StudentManager::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (file.is_open()) {
        int id, numGrades, grade;
        string name;
        while (file >> id >> name >> numGrades) {
            Student student;
            student.name = name;
            for (int i = 0; i < numGrades; ++i) {
                file >> grade;
                student.grades.push_back(grade);
            }
            students[id] = student;
        }
        file.close();
    } else {
        cerr << "Error opening file for reading!" << endl;
    }
}

// StudentManager functions (same as before)
void StudentManager::addStudent() {
    int id;
    string name;
    cout << "Enter student ID: ";
    cin >> id;
    cin.ignore(); // Ignore remaining newline
    cout << "Enter student name: ";
    getline(cin, name);

    if (students.find(id) != students.end()) {
        cout << "Student ID already exists!" << endl;
        return;
    }

    students[id] = {name, {}};
    cout << "Student added successfully." << endl;
}

void StudentManager::addGrade() {
    int id, grade;
    cout << "Enter student ID: ";
    cin >> id;
    cout << "Enter grade: ";
    cin >> grade;

    auto it = students.find(id);
    if (it == students.end()) {
        cout << "Student ID not found!" << endl;
        return;
    }

    it->second.grades.push_back(grade);
    cout << "Grade added successfully." << endl;
}

void StudentManager::displayGrades() const {
    if (students.empty()) {
        cout << "No student data available." << endl;
        return;
    }

    for (const auto& [id, student] : students) {
        cout << "ID: " << id << ", Name: " << student.name << ", Grades: ";
        for (int grade : student.grades) {
            cout << grade << ' ';
        }
        cout << endl;
    }
}

void StudentManager::calculateAverage() const {
    if (students.empty()) {
        cout << "No student data available." << endl;
        return;
    }

    for (const auto& [id, student] : students) {
        if (student.grades.empty()) {
            cout << "ID: " << id << ", Name: " << student.name << " has no grades." << endl;
            continue;
        }

        double sum = 0;
        for (int grade : student.grades) {
            sum += grade;
        }
        double average = sum / student.grades.size();
        cout << "ID: " << id << ", Name: " << student.name << ", Average Grade: " << fixed << setprecision(2) << average << endl;
    }
}

void StudentManager::editStudent() {
    int id;
    cout << "Enter student ID to edit: ";
    cin >> id;
    cin.ignore(); // Ignore remaining newline

    auto it = students.find(id);
    if (it == students.end()) {
        cout << "Student ID not found!" << endl;
        return;
    }

    string newName;
    cout << "Enter new name for student ID " << id << ": ";
    getline(cin, newName);

    it->second.name = newName;
    cout << "Student name updated successfully." << endl;
}

void StudentManager::deleteStudent() {
    int id;
    cout << "Enter student ID to delete: ";
    cin >> id;
    cin.ignore(); // Ignore remaining newline

    auto it = students.find(id);
    if (it == students.end()) {
        cout << "Student ID not found!" << endl;
        return;
    }

    students.erase(it);
    cout << "Student deleted successfully." << endl;
}

int main() {
    GLFWwindow* window;
    initializeOpenGL(window, 800, 600, "Student Manager");

    StudentManager manager;

    while (!glfwWindowShouldClose(window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Student Manager");

        static int studentID = 0;
        static char studentName[128] = "";
        static int grade = 0;

        ImGui::InputInt("Student ID", &studentID);
        ImGui::InputText("Student Name", studentName, IM_ARRAYSIZE(studentName));
        if (ImGui::Button("Add Student")) {
            manager.addStudent();
        }

        ImGui::InputInt("Grade", &grade);
        if (ImGui::Button("Add Grade")) {
            manager.addGrade();
        }

        if (ImGui::Button("Display Grades")) {
            manager.displayGrades();
        }

        if (ImGui::Button("Calculate Average")) {
            manager.calculateAverage();
        }

        if (ImGui::Button("Edit Student")) {
            manager.editStudent();
        }

        if (ImGui::Button("Delete Student")) {
            manager.deleteStudent();
        }

        ImGui::End();

        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
