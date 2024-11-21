#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <filesystem>

using namespace std;

// User class to hold user credentials
class User {
public:
    string username;
    string password;

    // Default constructor
    User() : username(""), password("") {}

    // Constructor with parameters
    User(string u, string p) : username(u), password(p) {}

    // Function to authenticate user
    bool authenticate(const string& enteredPassword) const {
        return password == enteredPassword;
    }
};

// Financial Application class
class MoneyMateApp {
private:
    unordered_map<string, User> users;  // stores users by username
    string currentUser;
    unordered_map<string, double> expenses;  // stores user expenses by category
    bool unsavedChanges = false;  // Flag to track if there are unsaved changes

public:
    // Function to load user data from a file
    void loadUserData(const string& filename) {
        ifstream file(filename);
        if (file) {
            string line;
            while (getline(file, line)) {
                stringstream ss(line);
                string username, password;
                getline(ss, username, ',');
                getline(ss, password);
                users[username] = User(username, password);
            }
        }
    }

    // Function to save user data to a file
    void saveUserData(const string& filename) {
        ofstream file(filename);
        for (const auto& user : users) {
            file << user.second.username << "," << user.second.password << endl;
        }
    }

    // Register a new user
    void registerUser() {
        string username, password;
        cout << "Enter a username: ";
        cin >> username;
        cout << "Enter a password: ";
        cin >> password;
        if (users.find(username) != users.end()) {
            cout << "Username already exists! Try again with a different username.\n";
            return;
        }
        users[username] = User(username, password);
        cout << "User registered successfully.\n";
    }

    // Login functionality
    bool loginUser() {
        if (!currentUser.empty()) {
            cout << "A user is already logged in. Please log out first.\n";
            return false;  // Only one user can be logged in at a time
        }

        string username, password;
        cout << "Enter username: ";
        cin >> username;
        if (users.find(username) == users.end()) {
            cout << "Username not found. Try again.\n";
            return false;
        }

        cout << "Enter password: ";
        cin >> password;
        if (users[username].authenticate(password)) {
            currentUser = username;
            cout << "Login successful.\n";
            loadExpenses();
            return true;
        } else {
            cout << "Incorrect password. Try again.\n";
            return false;
        }
    }

    // Add an expense
    void addExpense() {
        if (currentUser.empty()) {
            cout << "Please log in first.\n";
            return;
        }
        
        string expenseCategory;
        double amount;
        cout << "Enter expense category: ";
        cin >> expenseCategory;
        cout << "Enter expense amount: ";
        cin >> amount;

        expenses[expenseCategory] += amount;
        unsavedChanges = true;  // Mark as unsaved changes
        cout << "Expense added.\n";
    }

    // View all expenses
    void viewExpenses() {
        if (currentUser.empty()) {
            cout << "Please log in first.\n";
            return;
        }
        
        if (expenses.empty()) {
            cout << "No expenses to display.\n";
            return;
        }

        cout << "Expenses for " << currentUser << ":\n";
        for (const auto& expense : expenses) {
            cout << "Category: " << expense.first << ", Amount: " << fixed << setprecision(2) << expense.second << endl;
        }
    }

    // Save financial data to a user-specific file
    void saveFinancialData() {
        if (currentUser.empty()) {
            cout << "Please log in first.\n";
            return;
        }

        // Save the financial data to a file specific to the user
        string fileName = currentUser + "_expenses.txt";
        ofstream file(fileName);
        for (const auto& expense : expenses) {
            file << expense.first << ": " << expense.second << endl;
        }
        unsavedChanges = false;  // Reset unsaved changes flag
        cout << "Financial data saved to " << fileName << ".\n";
    }

    // Load expenses for the current user from their file
    void loadExpenses() {
        string fileName = currentUser + "_expenses.txt";
        ifstream file(fileName);
        
        if (!file) {
            cout << "No expenses found for this user. Start adding expenses.\n";
            return;  // No existing expenses file for this user
        }

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string category;
            double amount;
            if (getline(ss, category, ':')) {
                ss >> amount;
                expenses[category] = amount;
            }
        }
    }

    // Prompt to save expenses before logout
    void promptSaveBeforeLogout() {
        if (unsavedChanges) {
            char choice;
            cout << "You have unsaved changes. Would you like to save them before logging out? (y/n): ";
            cin >> choice;
            if (choice == 'y' || choice == 'Y') {
                saveFinancialData();
            } else {
                cout << "Changes were not saved.\n";
            }
        }
    }

    // Logout functionality
    void logoutUser() {
        if (currentUser.empty()) {
            cout << "No user is logged in.\n";
            return;
        }

        promptSaveBeforeLogout();  // Alert the user to save before logging out
        expenses.clear();  // Clear current user's expenses on logout
        currentUser.clear();  // Reset logged-in user
        cout << "Logged out.\n";
    }

    // Display menu
    void showMenu() {
        int choice = 0;
        while (true) {
            cout << "\n1. Register\n2. Login\n3. Add Expense\n4. View Expenses\n5. Save Financial Data\n6. Logout\n7. Exit\n";
            cout << "Enter choice: ";
            cin >> choice;

            switch (choice) {
            case 1:
                registerUser();
                break;
            case 2:
                if (loginUser()) {
                    break;
                }
                break;
            case 3:
                addExpense();
                break;
            case 4:
                viewExpenses();
                break;
            case 5:
                saveFinancialData();
                break;
            case 6:
                logoutUser();
                break;
            case 7:
                cout << "Exiting MoneyMate. Goodbye!\n";
                return;
            default:
                cout << "Invalid choice. Please try again.\n";
            }
        }
    }
};

int main() {
    MoneyMateApp app;
    string userFile = "users.txt";

    app.loadUserData(userFile);
    app.showMenu();
    app.saveUserData(userFile);

    return 0;
}
