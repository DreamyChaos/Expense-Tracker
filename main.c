/*
Project Title: Expense & Budget Tracker (Personal Finance)
Student Name: [Your Name Here]
Register No: [Your Reg No Here]
Description: A complete CRUD application to track daily expenses,
monitor against a monthly budget, and store data persistently.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EXPENSES 1000
#define FILE_NAME "expenses.csv"
#define BUDGET_FILE "budget.txt"

// 1. Structure Definition
typedef struct {
    int id;
    char date[15];     // Format: YYYY-MM-DD
    char category[30];
    float amount;
    char note[100];
} Expense;

// Global Variables
Expense expenses[MAX_EXPENSES];
int expenseCount = 0;
float monthlyBudget = 0.0;
int nextId = 1;

// Function Prototypes (Minimum 5 required, we have 9)
void loadData();
void saveData();
void setBudget();
void addExpense();
void viewSummary();
void viewCategoryBreakdown();
void searchExpenses();
void deleteExpense();
void clearBuffer();

int main() {
    int choice;
    loadData(); // Load existing data from files on startup

    // Menu-Driven Program using switch/case
    while (1) {
        printf("\n=========================================\n");
        printf("   EXPENSE & BUDGET TRACKER (FINANCE)    \n");
        printf("=========================================\n");
        printf("1. Set/Update Monthly Budget\n");
        printf("2. Add New Expense\n");
        printf("3. View Monthly Summary & Budget\n");
        printf("4. View Category-wise Breakdown\n");
        printf("5. Search Expenses\n");
        printf("6. Delete an Expense\n");
        printf("7. Exit & Save\n");
        printf("-----------------------------------------\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("\n[ERROR] Invalid input! Please enter a number.\n");
            clearBuffer();
            continue;
        }
        clearBuffer();

        switch (choice) {
            case 1: setBudget(); break;
            case 2: addExpense(); break;
            case 3: viewSummary(); break;
            case 4: viewCategoryBreakdown(); break;
            case 5: searchExpenses(); break;
            case 6: deleteExpense(); break;
            case 7:
                saveData();
                printf("\nData saved successfully. Exiting program... Goodbye!\n");
                exit(0);
            default:
                printf("\n[ERROR] Invalid choice! Please select between 1-7.\n");
        }
    }
    return 0;
}

// Utility: Clears input buffer to prevent infinite loops on bad input
void clearBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// File Handling: Load Data
void loadData() {
    // Load Budget
    FILE *bf = fopen(BUDGET_FILE, "r");
    if (bf != NULL) {
        fscanf(bf, "%f", &monthlyBudget);
        fclose(bf);
    }

    // Load Expenses
    FILE *f = fopen(FILE_NAME, "r");
    if (f == NULL) return; // File doesn't exist yet, which is fine

    while (fscanf(f, "%d,%14[^,],%29[^,],%f,%99[^\n]\n",
           &expenses[expenseCount].id,
           expenses[expenseCount].date,
           expenses[expenseCount].category,
           &expenses[expenseCount].amount,
           expenses[expenseCount].note) == 5) {

        if (expenses[expenseCount].id >= nextId) {
            nextId = expenses[expenseCount].id + 1;
        }
        expenseCount++;
    }
    fclose(f);
}

// File Handling: Save Data
void saveData() {
    // Save Budget
    FILE *bf = fopen(BUDGET_FILE, "w");
    if (bf != NULL) {
        fprintf(bf, "%.2f", monthlyBudget);
        fclose(bf);
    }

    // Save Expenses
    FILE *f = fopen(FILE_NAME, "w");
    if (f == NULL) {
        printf("\n[ERROR] Could not save data!\n");
        return;
    }
    for (int i = 0; i < expenseCount; i++) {
        fprintf(f, "%d,%s,%s,%.2f,%s\n",
                expenses[i].id,
                expenses[i].date,
                expenses[i].category,
                expenses[i].amount,
                expenses[i].note);
    }
    fclose(f);
}

// Feature: Set Budget (Input Validation)
void setBudget() {
    printf("\nEnter new monthly budget limit (Rs): ");
    if (scanf("%f", &monthlyBudget) != 1 || monthlyBudget < 0) {
        printf("[ERROR] Invalid amount! Must be a positive number.\n");
        clearBuffer();
        return;
    }
    clearBuffer();
    printf("[SUCCESS] Budget updated to Rs %.2f\n", monthlyBudget);
}

// Feature: Add Expense
void addExpense() {
    if (expenseCount >= MAX_EXPENSES) {
        printf("\n[ERROR] Storage full! Cannot add more expenses.\n");
        return;
    }

    Expense e;
    e.id = nextId++;

    printf("\nEnter Date (YYYY-MM-DD): ");
    scanf("%14s", e.date);
    clearBuffer();

    // Smart Category Menu
    int catChoice;
    printf("Select Category:\n1. Food\n2. Travel\n3. Medical\n4. Utilities\n5. Other\nChoice: ");
    scanf("%d", &catChoice);
    clearBuffer();
    switch(catChoice) {
        case 1: strcpy(e.category, "Food"); break;
        case 2: strcpy(e.category, "Travel"); break;
        case 3: strcpy(e.category, "Medical"); break;
        case 4: strcpy(e.category, "Utilities"); break;
        default: strcpy(e.category, "Other"); break;
    }

    // Input validation for amount
    printf("Enter Amount (Rs): ");
    if (scanf("%f", &e.amount) != 1 || e.amount <= 0) {
        printf("[ERROR] Invalid amount! Expense not added.\n");
        clearBuffer();
        return;
    }
    clearBuffer();

    printf("Enter Note/Description: ");
    fgets(e.note, sizeof(e.note), stdin);
    e.note[strcspn(e.note, "\n")] = 0; // Remove trailing newline

    expenses[expenseCount++] = e;
    printf("\n[SUCCESS] Expense added successfully!\n");
}

// Feature: View Summary & Budget Warning (Visual ASCII Bar)
void viewSummary() {
    if (expenseCount == 0) {
        printf("\nNo expenses recorded yet.\n");
        return;
    }

    char targetMonth[10];
    printf("\nEnter month to summarize (YYYY-MM): ");
    scanf("%7s", targetMonth);
    clearBuffer();

    float total = 0.0;
    for (int i = 0; i < expenseCount; i++) {
        if (strncmp(expenses[i].date, targetMonth, 7) == 0) {
            total += expenses[i].amount;
        }
    }

    printf("\n--- SUMMARY FOR %s ---\n", targetMonth);
    printf("Total Spent: Rs %.2f\n", total);
    printf("Budget Limit: Rs %.2f\n", monthlyBudget);

    if (monthlyBudget > 0) {
        float percentage = (total / monthlyBudget) * 100;
        printf("Budget Used: [");
        int barFill = (int)(percentage / 5); // 20 blocks max
        if (barFill > 20) barFill = 20;

        for (int i = 0; i < 20; i++) {
            if (i < barFill) printf("#");
            else printf("-");
        }
        printf("] %.1f%%\n", percentage);

        if (total > monthlyBudget) {
            printf("\n[WARNING] You have exceeded your monthly budget by Rs %.2f!\n", total - monthlyBudget);
        } else if (percentage >= 80.0) {
            printf("\n[ALERT] You are nearing your budget limit!\n");
        }
    }
}

// Feature: Category Breakdown
void viewCategoryBreakdown() {
    float food = 0, travel = 0, medical = 0, util = 0, other = 0;

    for (int i = 0; i < expenseCount; i++) {
        if (strcmp(expenses[i].category, "Food") == 0) food += expenses[i].amount;
        else if (strcmp(expenses[i].category, "Travel") == 0) travel += expenses[i].amount;
        else if (strcmp(expenses[i].category, "Medical") == 0) medical += expenses[i].amount;
        else if (strcmp(expenses[i].category, "Utilities") == 0) util += expenses[i].amount;
        else other += expenses[i].amount;
    }

    printf("\n--- CATEGORY BREAKDOWN ---\n");
    printf("Food:      Rs %.2f\n", food);
    printf("Travel:    Rs %.2f\n", travel);
    printf("Medical:   Rs %.2f\n", medical);
    printf("Utilities: Rs %.2f\n", util);
    printf("Other:     Rs %.2f\n", other);
}

// Feature: Search Expenses
void searchExpenses() {
    char keyword[30];
    printf("\nEnter Date (YYYY-MM-DD) or Category to search: ");
    scanf("%29s", keyword);
    clearBuffer();

    int found = 0;
    printf("\nID  | Date       | Category     | Amount     | Note\n");
    printf("----------------------------------------------------------\n");
    for (int i = 0; i < expenseCount; i++) {
        if (strstr(expenses[i].date, keyword) || strstr(expenses[i].category, keyword)) {
            printf("%-3d | %-10s | %-12s | Rs %-7.2f | %s\n",
                   expenses[i].id, expenses[i].date, expenses[i].category, expenses[i].amount, expenses[i].note);
            found++;
        }
    }
    if (found == 0) printf("No expenses found matching '%s'.\n", keyword);
}

// Feature: Delete Expense
void deleteExpense() {
    int delId, found = -1;
    printf("\nEnter the ID of the expense to delete: ");
    scanf("%d", &delId);
    clearBuffer();

    for (int i = 0; i < expenseCount; i++) {
        if (expenses[i].id == delId) {
            found = i;
            break;
        }
    }

    if (found != -1) {
        for (int i = found; i < expenseCount - 1; i++) {
            expenses[i] = expenses[i + 1];
        }
        expenseCount--;
        printf("[SUCCESS] Expense ID %d deleted.\n", delId);
    } else {
        printf("[ERROR] Expense ID not found.\n");
    }
}