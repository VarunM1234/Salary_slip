// auth.c - User authentication and export

#include "payroll.h"

const char *USERS_FILE = "users.txt";
const char *EMPLOYEES_FILE = "employees.txt";
const char *LOG_FILE = "activity.log";

void registerUser() {
    struct User newUser;
    static int userIdCounter = 1; // Simple ID generator

    FILE *fp = fopen(USERS_FILE, "a"); // Append text mode
    if (fp == NULL) {
        printf("Error opening users file for registration.\n");
        return;
    }

    newUser.id = userIdCounter++;

    printf("Enter username: ");
    scanf("%49s", newUser.username);
    printf("Enter password: ");
    scanf("%49s", newUser.password);

    int choice;
    printf("Role? (1 = Manager, 2 = Employee): ");
    scanf("%d", &choice);
    newUser.isManager = (choice == 1) ? 1 : 0;

    // Write user data as text to the file
    fprintf(fp, "%d %s %s %d\n", newUser.id, newUser.username, newUser.password, newUser.isManager);
    fclose(fp);

    printf("Registered User ID %d successfully!\n", newUser.id);
    logActivity("New user registered");
}

int loginUser(struct User *currentUser) {
    char username[50], password[50];
    FILE *fp = fopen(USERS_FILE, "r");
    if (fp == NULL) {
        printf("Register first.\n");
        return 0;
    }

    printf("Username: ");
    scanf("%49s", username);
    printf("Password: ");
    scanf("%49s", password);

    int id, isManager;
    char fileUsername[50], filePassword[50];
    while (fscanf(fp, "%d %s %s %d", &id, fileUsername, filePassword, &isManager) == 4) {
        if (strcmp(fileUsername, username) == 0 && strcmp(filePassword, password) == 0) {
            currentUser->id = id;
            strcpy(currentUser->username, fileUsername);
            strcpy(currentUser->password, filePassword);
            currentUser->isManager = isManager;
            fclose(fp);
            printf("Welcome back %s!\n", currentUser->username);
            logActivity("User logged in");
            return 1;
        }
    }

    fclose(fp);
    printf("Login failed.\n");
    return 0;
}

void exportUsersToTxt() {
    FILE *fp_in = fopen(USERS_FILE, "r");
    FILE *fp_out = fopen("users_export.txt", "w");
    if (fp_in == NULL) {
        if(fp_out) fclose(fp_out);
        printf("No user data found.\n");
        return;
    }
    if (fp_out == NULL) {
        fclose(fp_in);
        printf("Could not create export file.\n");
        return;
    }

    fprintf(fp_out, "User Export Report\n");
    fprintf(fp_out, "==================\n");
    fprintf(fp_out, "%-5s %-20s %-20s %-10s\n", "ID", "Username", "Password", "Role");
    fprintf(fp_out, "----------------------------------------\n");

    int id, isManager;
    char username[50], password[50];
    while (fscanf(fp_in, "%d %s %s %d", &id, username, password, &isManager) == 4) {
        fprintf(fp_out, "%-5d %-20s %-20s %-10s\n", id, username, password, isManager ? "Manager" : "Employee");
    }

    fclose(fp_in);
    fclose(fp_out);
    printf("Successfully built 'users_export.txt'\n");
    logActivity("Users exported to text file");
}
