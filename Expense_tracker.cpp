#include <iostream>
#include <fstream>
#include <unordered_map>
#include <iomanip>
#include <string>
#include <ctime>
#include <sstream>
#include <conio.h> // For _getch()
#include <windows.h> // For system("cls")

class User {
public:
    std::string username;
    std::string password;

    User(const std::string& uname, const std::string& pwd)
        : username(uname), password(pwd) {}

    void saveToFile() const {
        std::ofstream outfile("users.txt", std::ios_base::app);
        outfile << username << " " << password << " user\n";
    }

    static bool authenticate(const std::unordered_map<std::string, std::pair<std::string, std::string>>& users, const std::string& uname, const std::string& pwd) {
        auto it = users.find(uname);
        return it != users.end() && it->second.first == pwd && it->second.second == "user";
    }

    static void addExpense(const std::string& username) {
        double amount;
        std::string category, date = getCurrentDate();
        std::cout << "Enter expense amount: ";
        std::cin >> amount;
        std::cin.ignore();
        std::cout << "Enter expense category: ";
        std::getline(std::cin, category);

        std::ofstream userExpenseFile(username + "_expenses.txt", std::ios_base::app);
        userExpenseFile << amount << " " << category << " " << date << "\n";

        std::ofstream allTransactionsFile("all_transactions.txt", std::ios_base::app);
        allTransactionsFile << amount << " " << category << " " << date << "\n";

        logActivity(username, "Added expense: " + std::to_string(amount) + " for " + category);
        std::cout << "Expense added successfully.\n";
        pressAnyKeyToContinue();
    }

    static double calculateTotalExpense(const std::string& filename) {
        std::ifstream expenseFile(filename);
        double amount, total = 0.0;
        std::string category, date;
        while (expenseFile >> amount >> category >> date) {
            total += amount;
        }
        return total;
    }

    static void viewExpensesOrPayback(const std::unordered_map<std::string, std::pair<std::string, std::string>>& users, const std::string& username) {
        double totalExpense = calculateTotalExpense(username + "_expenses.txt");
        double dividedExpense = calculateTotalExpense("all_transactions.txt") / users.size();

        double amountDue = dividedExpense - totalExpense;
        std::cout << "Divided amount: Rs. " << dividedExpense << "\n";
        std::cout << "Total paid: Rs. " << totalExpense << "\n";

        if (amountDue > 0) {
            std::cout << "You owe: Rs. " << amountDue << "\n";
        } else {
            std::cout << "You have paid more than your share: Rs. " << -amountDue << "\n";
        }

        std::ifstream userExpenseFile(username + "_expenses.txt");
        std::cout << "Detailed expenses:\n";
        double amount;
        std::string category, date;
        while (userExpenseFile >> amount >> category >> date) {
            std::cout << "Amount: Rs. " << amount << ", Category: " << category 
                      << ", Date: " << date << "\n";
        }

        pressAnyKeyToContinue();
    }

    static std::string getCurrentDate() {
        std::time_t now_time = std::time(nullptr);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    static void logActivity(const std::string& username, const std::string& action) {
        std::ofstream logFile("activity_log.txt", std::ios_base::app);
        logFile << getCurrentDate() << " - " << username << ": " << action << "\n";
    }

    static void pressAnyKeyToContinue() {
        std::cout << "Press any key to continue...";
        _getch(); // Wait for key press
        std::cout << std::endl; 
    }

    static void clearScreen() {
        system("cls");
    }
};

class Admin : public User {
public:
    Admin(const std::string& uname, const std::string& pwd)
        : User(uname, pwd) {}

    void saveToFile() const {
        std::ofstream outfile("users.txt", std::ios_base::app);
        outfile << username << " " << password << " admin\n";
    }

    static bool authenticate(const std::unordered_map<std::string, std::pair<std::string, std::string>>& users, const std::string& uname, const std::string& pwd) {
        auto it = users.find(uname);
        return it != users.end() && it->second.first == pwd && it->second.second == "admin";
    }

    static void registerUser(std::unordered_map<std::string, std::pair<std::string, std::string>>& users) {
        std::string username, password;
        std::cout << "Enter a new username: ";
        std::cin >> username;
        std::cout << "Enter a new password: ";
        std::cin >> password;

        if (users.find(username) != users.end()) {
            std::cout << "Username already exists. Try a different one.\n";
            pressAnyKeyToContinue();
            return;
        }

        User user(username, password);
        user.saveToFile();
        users[username] = {password, "user"};
        logActivity("admin", "Registered user: " + username);
        std::cout << "User registered successfully.\n";
        pressAnyKeyToContinue();
    }

    static void loadUsers(std::unordered_map<std::string, std::pair<std::string, std::string>>& users) {
        std::ifstream infile("users.txt");
        std::string username, password, role;
        while (infile >> username >> password >> role) {
            users[username] = {password, role};
        }
    }

    static bool loginUser(const std::unordered_map<std::string, std::pair<std::string, std::string>>& users, std::string& loggedInUser, bool& isAdmin) {
        std::string username, password;
        std::cout << "Enter your username: ";
        std::cin >> username;

        std::cout << "Enter your password: ";
        char ch;
        password.clear();
        while ((ch = _getch()) != '\r') { // Use _getch() to read password without echoing
            password.push_back(ch);
            std::cout << '*'; // Mask the input with '*'
        }
        std::cout << std::endl; // New line after password input

        auto it = users.find(username);
        if (it != users.end() && it->second.first == password) {
            loggedInUser = username;
            isAdmin = (it->second.second == "admin");
            std::cout << "Login successful. Welcome to EXPENSE TRACKER, " << username << "!\n";
            return true;
        } else {
            std::cout << "Invalid username or password.\n";
            pressAnyKeyToContinue();
            return false;
        }
    }

    static void removeUser(std::unordered_map<std::string, std::pair<std::string, std::string>>& users, const std::string& username) {
        auto it = users.find(username);
        if (it != users.end()) {
            users.erase(it);
            std::cout << "User " << username << " removed successfully.\n";
            logActivity("admin", "Removed user: " + username);
        } else {
            std::cout << "User not found.\n";
        }
        pressAnyKeyToContinue();
    }

    static void viewUsers(const std::unordered_map<std::string, std::pair<std::string, std::string>>& users) {
        std::cout << "Registered Users:\n";
        for (const auto& user : users) {
            std::cout << "Username: " << user.first << ", Role: " << user.second.second << "\n";
        }
        pressAnyKeyToContinue();
    }

    static void viewUserTransactions(const std::unordered_map<std::string, std::pair<std::string, std::string>>& users) {
        std::string username;
        std::cout << "Enter username to view transactions: ";
        std::cin >> username;

        if (users.find(username) != users.end()) {
            std::ifstream userExpenseFile(username + "_expenses.txt");
            std::cout << "Transactions for user " << username << ":\n";
            double amount;
            std::string category, date;
            while (userExpenseFile >> amount >> category >> date) {
                std::cout << "Amount: Rs. " << amount << ", Category: " << category 
                          << ", Date: " << date << "\n";
            }
            logActivity("admin", "Viewed transactions for user: " + username);
        } else {
            std::cout << "User not found.\n";
        }
        pressAnyKeyToContinue();
    }

    static void viewActivityLog() {
        std::ifstream logFile("activity_log.txt");
        std::cout << "Activity Log:\n";
        std::string logEntry;
        while (std::getline(logFile, logEntry)) {
            std::cout << logEntry << "\n";
        }
        pressAnyKeyToContinue();
    }
};

void adminPortal(const std::string& username, std::unordered_map<std::string, std::pair<std::string, std::string>>& users);
void userPortal(const std::string& username, const std::unordered_map<std::string, std::pair<std::string, std::string>>& users);

void clearScreen() {
    system("cls");
}

int main() {
    std::unordered_map<std::string, std::pair<std::string, std::string>> users;
    Admin::loadUsers(users);

    // Predefine admin username and password
    users["expense"] = {"expense123", "admin"};

    int choice;
    std::string loggedInUser;
    bool isAdmin;

    do {
        clearScreen();
        std::cout << "Welcome to EXPENSE TRACKER AND DIVIDER\n";
        std::cout << "1. Admin Login\n";
        std::cout << "2. User Login\n";
        std::cout << "3. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
        case 1:
            if (Admin::loginUser(users, loggedInUser, isAdmin)) {
                clearScreen();
                adminPortal(loggedInUser, users);
            }
            break;
        case 2:
            if (Admin::loginUser(users, loggedInUser, isAdmin)) {
                clearScreen();
                userPortal(loggedInUser, users);
            }
            break;
        case 3:
            std::cout << "Thank you for using EXPENSE TRACKER AND DIVIDER. Goodbye!\n";
            break;
        default:
            std::cout << "Invalid choice. Please try again.\n";
            User::pressAnyKeyToContinue();
            break;
        }
    } while (choice != 3);

    return 0;
}

void adminPortal(const std::string& username, std::unordered_map<std::string, std::pair<std::string, std::string>>& users) {
    int choice;

    do {
        clearScreen();
        std::cout << "Admin Portal - Logged in as: " << username << "\n";
        std::cout << "1. Register User\n";
        std::cout << "2. Remove User\n";
        std::cout << "3. View Users\n";
        std::cout << "4. View User Transactions\n";
        std::cout << "5. View Activity Log\n";
        std::cout << "6. Logout\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
        case 1:
            Admin::registerUser(users);
            break;
        case 2: {
            // Create a new block to handle variable declarations within this case
            std::string userToRemove;
            std::cout << "Enter username to remove: ";
            std::cin >> userToRemove;
            Admin::removeUser(users, userToRemove);
            break;
        }
        case 3:
            Admin::viewUsers(users);
            break;
        case 4:
            Admin::viewUserTransactions(users);
            break;
        case 5:
            Admin::viewActivityLog();
            break;
        case 6:
            std::cout << "Logging out...\n";
            break;
        default:
            std::cout << "Invalid choice. Please try again.\n";
            User::pressAnyKeyToContinue();
            break;
        }
    } while (choice != 6);
}

void userPortal(const std::string& username, const std::unordered_map<std::string, std::pair<std::string, std::string>>& users) {
    int choice;

    do {
        clearScreen();
        std::cout << "User Portal - Logged in as: " << username << "\n";
        std::cout << "1. Add Expense\n";
        std::cout << "2. View Expenses/Payback\n";
        std::cout << "3. Logout\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
        case 1:
            User::addExpense(username);
            break;
        case 2:
            User::viewExpensesOrPayback(users, username);
            break;
        case 3:
            std::cout << "Logging out...\n";
            break;
        default:
            std::cout << "Invalid choice. Please try again.\n";
            User::pressAnyKeyToContinue();
            break;
        }
    } while (choice != 3);
}
