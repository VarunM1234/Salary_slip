// employee.c - Employee management and salary slips

#include "payroll.h"

int getNextAvailableEmpId() {
    FILE *fp = fopen(EMPLOYEES_FILE, "r");
    if (fp == NULL) {
        return 1; // Start from 1 if file doesn't exist
    }

    int maxId = 0, id, hours;
    float salary;
    char name[100], position[100];
    // Read lines and find the highest ID
    while (fscanf(fp, "%d %99s %99s %f %d", &id, name, position, &salary, &hours) == 5) {
        if (id > maxId) {
            maxId = id;
        }
    }
    fclose(fp);
    return maxId + 1;
}

void addEmployee() {
    struct Employee emp;
    emp.id = getNextAvailableEmpId();

    printf("Name: ");
    scanf("%99s", emp.name);
    printf("Designation: ");
    scanf("%99s", emp.position);
    printf("Basic Salary: ");
    scanf("%f", &emp.salary);
    printf("Hours: ");
    scanf("%d", &emp.hoursWorked);

    FILE *fp = fopen(EMPLOYEES_FILE, "a");
    if (fp != NULL) {
        fprintf(fp, "%d %s %s %.2f %d\n", emp.id, emp.name, emp.position, emp.salary, emp.hoursWorked);
        fclose(fp);
        printf("Added ID %d\n", emp.id);
        logActivity("Employee added");
    } else {
        printf("Error saving employee.\n");
    }
}

void viewEmployees() {
    FILE *fp = fopen(EMPLOYEES_FILE, "r");
    if (fp == NULL) {
        printf("Empty.\n");
        return;
    }

    printf("\nAll Employees:\n");
    int id, hours;
    float salary;
    char name[100], position[100];
    int serial = 1;
    while (fscanf(fp, "%d %99s %99s %f %d", &id, name, position, &salary, &hours) == 5) {
        printf("%d) ID: %d | Name: %s | Role: %s | Pay: %.2f | Hr: %d\n",
               serial++, id, name, position, salary, hours);
    }
    fclose(fp);
}

void searchEmployee() {
    int searchId;
    printf("Enter ID: ");
    scanf("%d", &searchId);

    FILE *fp = fopen(EMPLOYEES_FILE, "r");
    if (fp == NULL) {
        printf("No employees.\n");
        return;
    }

    int id, hours;
    float salary;
    char name[100], position[100];
    while (fscanf(fp, "%d %99s %99s %f %d", &id, name, position, &salary, &hours) == 5) {
        if (id == searchId) {
            printf("\nFound -> Name: %s | Role: %s | Pay: %.2f\n", name, position, salary);
            fclose(fp);
            return;
        }
    }
    printf("Not found.\n");
    fclose(fp);
}

void updateEmployee() {
    int updateId;
    printf("Update ID: ");
    scanf("%d", &updateId);

    FILE *fp_read = fopen(EMPLOYEES_FILE, "r");
    FILE *fp_write = fopen("temp_update.txt", "w");
    if (fp_read == NULL || fp_write == NULL) {
        printf("File error.\n");
        return;
    }

    int found = 0;
    int id, hours;
    float salary;
    char name[100], position[100];
    while (fscanf(fp_read, "%d %99s %99s %f %d", &id, name, position, &salary, &hours) == 5) {
        if (id == updateId) {
            found = 1;
            printf("Current Name: %s, New Name: ", name);
            scanf("%99s", name);
            printf("Current Role: %s, New Role: ", position);
            scanf("%99s", position);
            printf("Current Salary: %.2f, New Salary: ", salary);
            scanf("%f", &salary);
            printf("Current Hours: %d, New Hours: ", hours);
            scanf("%d", &hours);
        }
        fprintf(fp_write, "%d %s %s %.2f %d\n", id, name, position, salary, hours);
    }

    fclose(fp_read);
    fclose(fp_write);

    if (found) {
        remove(EMPLOYEES_FILE);
        rename("temp_update.txt", EMPLOYEES_FILE);
        printf("Updated!\n");
        logActivity("Employee updated");
    } else {
        remove("temp_update.txt");
        printf("ID not found.\n");
    }
}

void deleteEmployee() {
    int deleteId;
    printf("Delete ID: ");
    scanf("%d", &deleteId);

    FILE *fp_read = fopen(EMPLOYEES_FILE, "r");
    FILE *fp_write = fopen("temp_delete.txt", "w");
    if (fp_read == NULL || fp_write == NULL) {
        printf("File error.\n");
        return;
    }

    int found = 0;
    int id, hours;
    float salary;
    char name[100], position[100];
    while (fscanf(fp_read, "%d %99s %99s %f %d", &id, name, position, &salary, &hours) == 5) {
        if (id == deleteId) {
            found = 1;
            printf("Removing %s (ID %d)\n", name, id);
        } else {
            fprintf(fp_write, "%d %s %s %.2f %d\n", id, name, position, salary, hours);
        }
    }

    fclose(fp_read);
    fclose(fp_write);

    if (found) {
        remove(EMPLOYEES_FILE);
        rename("temp_delete.txt", EMPLOYEES_FILE);
        printf("Deleted!\n");
        logActivity("Employee deleted");
    } else {
        remove("temp_delete.txt");
        printf("ID not found.\n");
    }
}

void generateSalarySlip() {
    int empId;
    printf("ID for slip: ");
    scanf("%d", &empId);

    FILE *fp = fopen(EMPLOYEES_FILE, "r");
    if (fp == NULL) {
        printf("No employees.\n");
        return;
    }

    int id, hours;
    float salary;
    char name[100], position[100];
    while (fscanf(fp, "%d %99s %99s %f %d", &id, name, position, &salary, &hours) == 5) {
        if (id == empId) {
            fclose(fp);

            // --- Salary Calculation ---
            float basic_salary = salary;
            float hra = basic_salary * 0.20f;
            float da = basic_salary * 0.10f;
            float conveyance_allowance = 1600.0f;
            float medical_allowance = 1250.0f;
            float gross_earnings = basic_salary + hra + da + conveyance_allowance + medical_allowance;
            float pf_deduction = basic_salary * 0.12f;
            float tax_deduction = gross_earnings * 0.05f;
            float total_deductions = pf_deduction + tax_deduction;
            float net_pay = gross_earnings - total_deductions;

            // --- Print the Detailed Salary Slip ---
            printf("\n=============================================\n");
            printf("           XYZ TECHNOLOGIES PVT. LTD.        \n");
            printf("           www.xyztech.com | est. 2010       \n");
            printf("---------------------------------------------\n");
            printf("              SALARY SLIP FOR APRIL 2025     \n"); // Placeholder
            printf("---------------------------------------------\n");
            printf("Employee ID      : %d\n", id);
            printf("Employee Name    : %s\n", name);
            printf("Designation      : %s\n", position);
            printf("Department       : IT Services\n"); // Placeholder
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
            printf("Tax (TDS @ 5%%)     : %.2f\n", tax_deduction);
            printf("---------------------------------------------\n");
            printf("TOTAL DEDUCTIONS : %.2f\n", total_deductions);
            printf("=============================================\n");
            printf("NET TAKE-HOME PAY: %.2f INR\n", net_pay);
            printf("=============================================\n");
            printf("Payment Method   : Direct Deposit (Ac XXXX%d)\n", id * 123 % 10000); // Placeholder
            printf("Generated On     : 05-APR-2025\n"); // Placeholder
            printf("\n---------------------------------------------\n");
            printf("      *** This is a computer-generated slip ***\n");
            printf("=============================================\n");

            logActivity("Salary slip generated");
            return;
        }
    }
    printf("Missing.\n");
    fclose(fp);
}

void logActivity(const char *message) {
    FILE *fp = fopen(LOG_FILE, "a");
    if (fp != NULL) {
        fprintf(fp, "- %s\n", message);
        fclose(fp);
    }
}
