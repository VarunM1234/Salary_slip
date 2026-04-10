#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- File Names ---
const char *USERS_FILE = "users.txt";
const char *EMPLOYEES_FILE = "employees.txt";
const char *LOG_FILE = "activity.log";

// --- Structures ---
struct User {
    int id;
    char username[50];
    char password[50];
    int isManager; // 1 for Manager, 0 for Employee
};

struct Employee {
    int id;
    char name[100];
    char position[100]; // Designation
    float salary;
    int hoursWorked;
};

// --- Function Prototypes ---
void logActivity(const char *message);
void registerUser();
int loginUser(struct User *currentUser);
void exportUsersToTxt();
int getNextAvailableEmpId();
void addEmployee();
void viewEmployees();
void searchEmployee(); // Will be restricted to managers in main
void updateEmployee();
void deleteEmployee();
void generateSalarySlip();

// --- Helper to get integer input safely ---
int getIntegerInput() {
    char buffer[100];
    int value;
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        if (sscanf(buffer, "%d", &value) == 1) {
            return value;
        }
    }
    printf("Invalid input. Please enter a number.\n");
    return -1; // Indicate invalid input
}

// --- Helper to get float input safely ---
float getFloatInput() {
    char buffer[100];
    float value;
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        if (sscanf(buffer, "%f", &value) == 1) {
            return value;
        }
    }
    printf("Invalid input. Please enter a number.\n");
    return -1.0; // Indicate invalid input
}

// --- Helper to get string input safely ---
void getStringInput(char *buffer, int max_length) {
    if (fgets(buffer, max_length, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        } else {
            // Check if there was no newline -> means the line was truncated
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
    } else {
        buffer[0] = '\0';
    }
}

// --- Function Implementations ---

void logActivity(const char *message) {
    FILE *fp = fopen(LOG_FILE, "a");
    if (fp != NULL) {
        fprintf(fp, "[ACTIVITY] %s\n", message);
        fclose(fp);
    } else {
        printf("Warning: Could not write to log file.\n");
    }
}

void registerUser() {
    struct User newUser;
    FILE *fp = fopen(USERS_FILE, "a+");
    if (fp == NULL) {
        printf("Error opening users file for registration.\n");
        return;
    }

    // Find the highest existing ID to generate a new one
    int highestId = 0;
    int id, isManager;
    char uname[50], pwd[50];
    rewind(fp); // Go to the beginning to read existing users
    while (fscanf(fp, "%d %49s %49s %d", &id, uname, pwd, &isManager) == 4) {
        if (id > highestId) {
            highestId = id;
        }
    }

    newUser.id = highestId + 1;
    printf("Enter username: ");
    getStringInput(newUser.username, sizeof(newUser.username)); // Simple input, consider fgets for spaces
    printf("Enter password: ");
    getStringInput(newUser.password, sizeof(newUser.password));

    int choice;
    printf("Role? (1 = Manager, 2 = Employee): ");
    choice = getIntegerInput();
    if (choice != 1 && choice != 2) {
        printf("Invalid role selection.\n");
        fclose(fp);
        return;
    }
    newUser.isManager = (choice == 1) ? 1 : 0;

    fprintf(fp, "%d %s %s %d\n", newUser.id, newUser.username, newUser.password, newUser.isManager);
    fclose(fp);

    printf("Registered User ID %d successfully!\n", newUser.id);
    logActivity("New user registered");
}

int loginUser(struct User *currentUser) {
    char username[50], password[50];
    FILE *fp = fopen(USERS_FILE, "r");
    if (fp == NULL) {
        printf("No users found. Please register first.\n");
        return 0; // Login failed
    }

    int attempts = 0;
    const int max_attempts = 3;
    while (attempts < max_attempts) {
        printf("Attempt %d/%d\n", attempts + 1, max_attempts);
        printf("Username: ");
        getStringInput(username, sizeof(username));
        printf("Password: ");
        getStringInput(password, sizeof(password));

        int id, isManager;
        char fileUsername[50], filePassword[50];
        rewind(fp); // Rewind to check from the beginning each time
        while (fscanf(fp, "%d %49s %49s %d", &id, fileUsername, filePassword, &isManager) == 4) {
            if (strcmp(fileUsername, username) == 0 && strcmp(filePassword, password) == 0) {
                currentUser->id = id;
                strcpy(currentUser->username, fileUsername);
                strcpy(currentUser->password, filePassword);
                currentUser->isManager = isManager;
                fclose(fp);
                printf("Login successful! Welcome, %s (ID: %d, %s).\n",
                       currentUser->username, currentUser->id,
                       currentUser->isManager ? "Manager" : "Employee");
                logActivity("User logged in");
                return 1; // Login successful
            }
        }
        printf("Login failed.\n");
        attempts++;
    }

    fclose(fp);
    printf("Too many failed attempts. Login blocked.\n");
    logActivity("Multiple failed login attempts");
    return 0; // Login failed after retries
}

void exportUsersToTxt() {
    FILE *fp_in = fopen(USERS_FILE, "r");
    FILE *fp_out = fopen("users_export.txt", "w");
    if (fp_in == NULL) {
        if(fp_out) fclose(fp_out);
        printf("No user data found to export.\n");
        return;
    }
    if (fp_out == NULL) {
        fclose(fp_in);
        printf("Could not create export file 'users_export.txt'.\n");
        return;
    }

    fprintf(fp_out, "User Export Report\n");
    fprintf(fp_out, "==================\n");
    fprintf(fp_out, "%-5s %-20s %-20s %-10s\n", "ID", "Username", "Password", "Role");
    fprintf(fp_out, "----------------------------------------\n");

    int id, isManager;
    char username[50], password[50];
    while (fscanf(fp_in, "%d %49s %49s %d", &id, username, password, &isManager) == 4) {
        fprintf(fp_out, "%-5d %-20s %-20s %-10s\n",
                id, username, password,
                isManager ? "Manager" : "Employee");
    }

    fclose(fp_in);
    fclose(fp_out);
    printf("Users exported to 'users_export.txt'.\n");
    logActivity("Users exported to text file");
}

int getNextAvailableEmpId() {
    FILE *fp = fopen(EMPLOYEES_FILE, "r");
    if (fp == NULL) {
        return 1; // Start from 1 if file doesn't exist
    }

    int maxId = 0;
    int id, hours;
    float salary;
    char name[100], position[100];
    while (fscanf(fp, "%d %99s %99s %f %d", &id, name, position, &salary, &hours) == 5) {
        if (id > maxId) {
            maxId = id;
        }
    }
    fclose(fp);
    return maxId + 1; // Return the next sequential ID
}

void addEmployee() {
    struct Employee emp;
    emp.id = getNextAvailableEmpId(); // Use sequential ID

    printf("Enter employee name: ");
    getStringInput(emp.name, sizeof(emp.name)); // Consider fgets for spaces
    printf("Enter position/designation: ");
    getStringInput(emp.position, sizeof(emp.position));
    printf("Enter base salary: ");
    emp.salary = getFloatInput();
    if (emp.salary < 0) { printf("Invalid salary.\n"); return; }
    printf("Enter hours worked: ");
    emp.hoursWorked = getIntegerInput();
    if (emp.hoursWorked < 0) { printf("Invalid hours.\n"); return; }

    FILE *fp = fopen(EMPLOYEES_FILE, "a");
    if (fp != NULL) {
        fprintf(fp, "%d %s %s %.2f %d\n", emp.id, emp.name, emp.position, emp.salary, emp.hoursWorked);
        fclose(fp);
        printf("Employee (ID: %d, Name: %s) added successfully!\n", emp.id, emp.name);
        logActivity("Employee added");
    } else {
        printf("Error opening employees file for writing.\n");
    }
}

void viewEmployees() {
    FILE *fp = fopen(EMPLOYEES_FILE, "r");
    if (fp == NULL) {
        printf("No employees found.\n");
        return;
    }

    printf("\n--- All Employees ---\n");
    // Adjust column widths as needed
    printf("%-5s %-5s %-20s %-20s %-12s %-8s\n", "S.No", "ID", "Name", "Position", "Salary", "Hours");
    printf("--------------------------------------------------------------------------\n");

    int id, hours, serial_no = 1;
    float salary;
    char name[100], position[100];
    while (fscanf(fp, "%d %99s %99s %f %d", &id, name, position, &salary, &hours) == 5) {
        printf("%-5d %-5d %-20s %-20s %-12.2f %-8d\n",
               serial_no++, id, name, position, salary, hours);
    }
    printf("--------------------------------------------------------------------------\n");
    fclose(fp);
}

// This function is accessible to all, but main() will control who calls it
void searchEmployee() {
    int searchId;
    printf("Enter Employee ID to search: ");
    searchId = getIntegerInput();
    if (searchId <= 0) { printf("Invalid ID.\n"); return; }

    FILE *fp = fopen(EMPLOYEES_FILE, "r");
    if (fp == NULL) {
        printf("No employees found.\n");
        return;
    }

    int id, hours;
    float salary;
    char name[100], position[100];
    while (fscanf(fp, "%d %99s %99s %f %d", &id, name, position, &salary, &hours) == 5) {
        if (id == searchId) {
            printf("\nFound Employee:\n");
            printf("ID: %d, Name: %s, Position: %s, Salary: %.2f, Hours: %d\n",
                   id, name, position, salary, hours);
            fclose(fp);
            return;
        }
    }

    printf("Employee with ID %d not found.\n", searchId);
    fclose(fp);
}

void updateEmployee() {
    int updateId;
    printf("Enter Employee ID to update: ");
    updateId = getIntegerInput();
    if (updateId <= 0) { printf("Invalid ID.\n"); return; }

    FILE *fp_read = fopen(EMPLOYEES_FILE, "r");
    FILE *fp_write = fopen("temp_update.txt", "w");
    if (fp_read == NULL || fp_write == NULL) {
        printf("Error opening files for update.\n");
        if (fp_read) fclose(fp_read);
        if (fp_write) fclose(fp_write);
        return;
    }

    int found = 0;
    int id, hours;
    float salary;
    char name[100], position[100];
    while (fscanf(fp_read, "%d %99s %99s %f %d", &id, name, position, &salary, &hours) == 5) {
        if (id == updateId && !found) {
            found = 1;
            printf("Updating employee ID %d: %s\n", id, name);
            printf("Enter new name (current: %s): ", name);
            getStringInput(name, sizeof(name));
            printf("Enter new position (current: %s): ", position);
            getStringInput(position, sizeof(position));
            printf("Enter new base salary (current: %.2f): ", salary);
            salary = getFloatInput();
            if (salary < 0) { printf("Invalid salary.\n"); fclose(fp_read); fclose(fp_write); remove("temp_update.txt"); return; }
            printf("Enter new hours worked (current: %d): ", hours);
            hours = getIntegerInput();
            if (hours < 0) { printf("Invalid hours.\n"); fclose(fp_read); fclose(fp_write); remove("temp_update.txt"); return; }
        }
        fprintf(fp_write, "%d %s %s %.2f %d\n", id, name, position, salary, hours);
    }

    fclose(fp_read);
    fclose(fp_write);

    if (found) {
        remove(EMPLOYEES_FILE);
        if (rename("temp_update.txt", EMPLOYEES_FILE) == 0) {
            printf("Employee updated successfully!\n");
            logActivity("Employee updated");
        } else {
             printf("Error renaming temporary file. Update might have failed.\n");
        }
    } else {
        remove("temp_update.txt"); // Delete temp file if not used
        printf("Employee with ID %d not found.\n", updateId);
    }
}

void deleteEmployee() {
    int deleteId;
    printf("Enter Employee ID to delete: ");
    deleteId = getIntegerInput();
    if (deleteId <= 0) { printf("Invalid ID.\n"); return; }

    FILE *fp_read = fopen(EMPLOYEES_FILE, "r");
    FILE *fp_write = fopen("temp_delete.txt", "w");
    if (fp_read == NULL || fp_write == NULL) {
        printf("Error opening files for deletion.\n");
        if (fp_read) fclose(fp_read);
        if (fp_write) fclose(fp_write);
        return;
    }

    int found = 0;
    int id, hours;
    float salary;
    char name[100], position[100];
    while (fscanf(fp_read, "%d %99s %99s %f %d", &id, name, position, &salary, &hours) == 5) {
        if (id == deleteId) {
            found = 1;
            printf("Deleting employee ID %d: %s\n", id, name);
            // Do not write this employee to the temp file
        } else {
            fprintf(fp_write, "%d %s %s %.2f %d\n", id, name, position, salary, hours);
        }
    }

    fclose(fp_read);
    fclose(fp_write);

    if (found) {
        remove(EMPLOYEES_FILE);
        if (rename("temp_delete.txt", EMPLOYEES_FILE) == 0) {
            printf("Employee deleted successfully!\n");
            logActivity("Employee deleted");
        } else {
             printf("Error renaming temporary file. Deletion might have failed.\n");
        }
        // Note: IDs are NOT reused. Next added employee gets max_id_found + 1
    } else {
        remove("temp_delete.txt"); // Delete temp file if not used
        printf("Employee with ID %d not found.\n", deleteId);
    }
}

void generateSalarySlip() {
    int empId;
    printf("Enter Employee ID for salary slip: ");
    empId = getIntegerInput();
    if (empId <= 0) { printf("Invalid ID.\n"); return; }

    FILE *fp = fopen(EMPLOYEES_FILE, "r");
    if (fp == NULL) {
        printf("No employees found.\n");
        return;
    }

    int id, hours;
    float salary;
    char name[100], position[100];
    while (fscanf(fp, "%d %99s %99s %f %d", &id, name, position, &salary, &hours) == 5) {
        if (id == empId) {
            fclose(fp);

            // --- Assumed Calculation Logic (Simplified) ---
            float basic_salary = salary;
            float hra = basic_salary * 0.20f;
            float da = basic_salary * 0.10f;
            float conveyance_allowance = 1600.0f;
            float medical_allowance = 1250.0f;
            float gross_earnings = basic_salary + hra + da + conveyance_allowance + medical_allowance;
            float pf_deduction = basic_salary * 0.12f;
            float tax_deduction = gross_earnings * 0.05f; // Simplified flat rate
            float total_deductions = pf_deduction + tax_deduction;
            float net_pay = gross_earnings - total_deductions;


            // --- Print the Detailed Salary Slip to Console ---
            printf("\n=============================================\n");
            printf("           XYZ TECHNOLOGIES PVT. LTD.        \n");
            printf("           www.xyztech.com | est. 2010       \n");
            printf("---------------------------------------------\n");
            printf("              SALARY SLIP FOR APRIL 2025     \n"); // Use actual month/year
            printf("---------------------------------------------\n");
            printf("Employee ID      : %d\n", id);
            printf("Employee Name    : %s\n", name);
            printf("Designation      : %s\n", position); // Using position as designation
            printf("Department       : IT Services\n"); // Placeholder Department
            printf("---------------------------------------------\n");
            printf("EARNINGS (INR)\n");
            printf("---------------------------------------------\n");
            printf("Basic Salary     : %.2f\n", basic_salary);
            printf("HRA (20%%)        : %.2f\n", hra);
            printf("DA (10%%)         : %.2f\n", da);
            printf("Conveyance Allow.: %.2f\n", conveyance_allowance);
            printf("Medical Allowance: %.2f\n", medical_allowance);
            printf("---------------------------------------------\n");
            printf("GROSS EARNINGS   : %.2f\n", gross_earnings);
            printf("---------------------------------------------\n");
            printf("DEDUCTIONS (INR)\n");
            printf("---------------------------------------------\n");
            printf("Provident Fund (12%%): %.2f\n", pf_deduction);
            printf("Tax (TDS @ 5%%)     : %.2f\n", tax_deduction); // Simplified TDS description
            printf("---------------------------------------------\n");
            printf("TOTAL DEDUCTIONS : %.2f\n", total_deductions);
            printf("=============================================\n");
            printf("NET TAKE-HOME PAY: %.2f INR\n", net_pay);
            printf("=============================================\n");
            printf("Payment Method   : Direct Deposit (Ac XXXX%d)\n", id * 123 % 10000); // Placeholder Ac No
            printf("Generated On     : 05-APR-2025\n"); // Placeholder Date
            printf("\n---------------------------------------------\n");
            printf("      *** This is a computer-generated slip ***\n");
            printf("=============================================\n");

            logActivity("Salary slip generated");
            return;
        }
    }

    printf("Employee with ID %d not found.\n", empId);
    fclose(fp);
}

// --- Main Program Loop ---
int main() {
    char input_buffer[100]; // Buffer for fgets
    int choice;
    struct User currentUser;
    int isLoggedIn = 0;

    printf("\nWelcome to the Consolidated Payroll Text-System!\n");

    while (1) {
        if (!isLoggedIn) {
            printf("\n1. Login\n2. Register\n3. Exit\nChoose an option: ");
            if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
                printf("\nInput error. Exiting.\n");
                break; // Exit loop on input error
            }
            // Use sscanf to extract the number from the line
            if (sscanf(input_buffer, "%d", &choice) != 1) {
                printf("Invalid input. Please enter a number (1, 2, or 3).\n");
                continue; // Go back to the start of the loop for login/register
            }

            switch (choice) {
            case 1:
                if (loginUser(&currentUser)) {
                    isLoggedIn = 1;
                }
                break;
            case 2:
                registerUser();
                break;
            case 3:
                printf("Exiting the Payroll System. Goodbye!\n");
                return 0; // Exit program
            default:
                printf("Invalid choice. Please select 1, 2, or 3.\n");
                break;
            }
        } else { // User is logged in
            printf("\n--- Main Menu ---\n");
            // Options accessible to ALL logged-in users
            printf("6. Generate Salary Slip\n");
            printf("7. Logout\n");

            // Options accessible ONLY to Managers
            if (currentUser.isManager) {
                printf("1. Add Employee\n");
                printf("2. View Employees\n");
                printf("3. Search Employee\n"); // Moved inside manager block
                printf("4. Update Employee\n");
                printf("5. Delete Employee\n");
                printf("8. Export Users to TXT\n");
                printf("9. View Activity Logs\n");
            }

            printf("Choose Option: ");
            if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
                printf("\nInput error. Logging out.\n");
                isLoggedIn = 0; // Force logout on input error
                continue;
            }
            if (sscanf(input_buffer, "%d", &choice) != 1) {
                printf("Invalid input. Please enter a number.\n");
                continue; // Stay logged in, go back to menu
            }

            switch (choice) {
            case 1: // Add Employee
                if (currentUser.isManager) {
                    addEmployee();
                } else {
                    printf("Access denied. Managers only.\n");
                }
                break;
            case 2: // View Employees
                if (currentUser.isManager) {
                    viewEmployees();
                } else {
                    printf("Access denied. Managers only.\n");
                }
                break;
            case 3: // Search Employee
                 // Only accessible to managers based on menu visibility
                 if (currentUser.isManager) {
                    searchEmployee();
                 } else {
                    // Technically shouldn't happen due to menu hiding, but good practice
                    printf("Access denied. Managers only.\n");
                 }
                break;
            case 4: // Update Employee
                if (currentUser.isManager) {
                    updateEmployee();
                } else {
                    printf("Access denied. Managers only.\n");
                }
                break;
            case 5: // Delete Employee
                if (currentUser.isManager) {
                    deleteEmployee();
                } else {
                    printf("Access denied. Managers only.\n");
                }
                break;
            case 6: // Generate Salary Slip (Accessible to all)
                generateSalarySlip();
                break;
            case 7: // Logout
                isLoggedIn = 0;
                printf("Logged out successfully.\n");
                break;
            case 8: // Export Users (Accessible to managers)
                if (currentUser.isManager) {
                    exportUsersToTxt();
                } else {
                    printf("Access denied. Managers only.\n");
                }
                break;
            case 9: // View Activity Logs (Accessible to managers)
                if (currentUser.isManager) {
                    FILE *fp = fopen(LOG_FILE, "r");
                    if (fp) {
                        printf("\n--- Activity Log ---\n");
                        int c;
                        while ((c = fgetc(fp)) != EOF) putchar(c);
                        printf("--- End of Log ---\n");
                        fclose(fp);
                    } else {
                        printf("No activity log found yet.\n");
                    }
                } else {
                    printf("Access denied. Managers only.\n");
                }
                break;
            default:
                printf("Invalid option. Please choose a valid menu item.\n");
                break;
            }
        }
    }
    return 0; // Should never reach here with the exit option, but good practice
}
