// payroll.h - Simple header for payroll system

#include <stdio.h>   // For FILE, printf, scanf, etc.
#include <stdlib.h>  // For malloc, free, etc.
#include <string.h>  // For strcpy, strcmp, etc.

// Data Structures
struct User {
    int id;
    char username[50];
    char password[50];
    int isManager; // 1 for Manager, 0 for Employee
};

struct Employee {
    int id;
    char name[100];
    char position[100];
    float salary;
    int hoursWorked;
};

// File names (defined in one .c file)
extern const char *USERS_FILE;
extern const char *EMPLOYEES_FILE;
extern const char *LOG_FILE;

// Function declarations

// From auth.c
void registerUser();
int loginUser(struct User *currentUser);
void exportUsersToTxt();

// From employee.c
int getNextAvailableEmpId();
void addEmployee();
void viewEmployees();
void searchEmployee();
void updateEmployee();
void deleteEmployee();
void generateSalarySlip();
void logActivity(const char *message);
float calculateSalary(float baseSalary, int hoursWorked); // Optional helper
