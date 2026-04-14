#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

// Forward declaration for Friend Function
class BankManager;

// ================= Base Account Class =================
class Account {
protected:
    int accNum;
    string name;
    double balance;
    string type;

public:
    Account(int a = 0, string n = "", double b = 0.0, string t = "General") 
        : accNum(a), name(n), balance(b), type(t) {}

    virtual ~Account() {}

    // Pure Virtual Functions (Abstraction)
    virtual void deposit(double amt) = 0;
    virtual void withdraw(double amt) = 0;
    
    // Friend function to allow the manager to see private details easily
    friend void showAccountStatus(const Account& acc);

    int getAccNum() const { return accNum; }
    double getBalance() const { return balance; }
    string getName() const { return name; }
    string getType() const { return type; }

    virtual void displayHeader() const {
        cout << left << setw(10) << accNum << setw(20) << name 
             << setw(12) << type << "$" << fixed << setprecision(2) << balance << endl;
    }
};

// ================= Derived: Savings Account =================
class SavingsAccount : public Account {
    const double minBalance = 500.0;
public:
    SavingsAccount(int a, string n, double b) : Account(a, n, b, "Savings") {}

    void deposit(double amt) override {
        balance += amt;
        cout << "\tDeposited: $" << amt << " (Interest will accrue monthly)\n";
    }

    void withdraw(double amt) override {
        if (balance - amt < minBalance) {
            cout << "\tError: Cannot drop below minimum balance of $500.\n";
        } else {
            balance -= amt;
            cout << "\tWithdrawal successful.\n";
        }
    }
};

// ================= Derived: Current Account =================
class CurrentAccount : public Account {
    const double overdraftLimit = 1000.0;
public:
    CurrentAccount(int a, string n, double b) : Account(a, n, b, "Current") {}

    void deposit(double amt) override {
        balance += amt;
        cout << "\tDeposited: $" << amt << "\n";
    }

    void withdraw(double amt) override {
        if (balance - amt < -overdraftLimit) {
            cout << "\tError: Overdraft limit of $1000 exceeded.\n";
        } else {
            balance -= amt;
            cout << "\tWithdrawal successful (Overdraft used if balance < 0).\n";
        }
    }
};

// ================= Manager Class =================
class BankManager {
private:
    vector<Account*> database;
    const string dataFile = "bank_v2.txt";

public:
    BankManager() { loadData(); }
    ~BankManager() { saveData(); for(auto a : database) delete a; }

    void createAccount() {
        int id, typeChoice;
        string n;
        double b;

        cout << "\n--- New Account Menu ---\n";
        cout << "1. Savings\n2. Current\nEnter choice: ";
        cin >> typeChoice;
        cout << "Enter Account Number: "; cin >> id;
        
        // Check for duplicates
        if (findAccount(id) != nullptr) {
            cout << "Error: Account number already exists!\n";
            return;
        }

        cout << "Enter Holder Name: "; cin.ignore(); getline(cin, n);
        cout << "Initial Deposit: "; cin >> b;

        if (typeChoice == 1) database.push_back(new SavingsAccount(id, n, b));
        else database.push_back(new CurrentAccount(id, n, b));
        
        cout << "Account registered successfully!\n";
    }

    Account* findAccount(int id) {
        for (auto a : database) {
            if (a->getAccNum() == id) return a;
        }
        return nullptr;
    }

    void displayAll() {
        cout << "\n" << string(55, '-') << endl;
        cout << left << setw(10) << "ID" << setw(20) << "Name" 
             << setw(12) << "Type" << "Balance" << endl;
        cout << string(55, '-') << endl;
        for (auto a : database) a->displayHeader();
        cout << string(55, '-') << endl;
    }

    void processTransaction() {
        int id, action;
        double amt;
        cout << "Enter Account ID: "; cin >> id;
        Account* acc = findAccount(id);
        
        if (!acc) {
            cout << "Account not found!\n";
            return;
        }

        cout << "1. Deposit\n2. Withdraw\nChoice: "; cin >> action;
        cout << "Enter Amount: "; cin >> amt;

        if (action == 1) acc->deposit(amt);
        else if (action == 2) acc->withdraw(amt);
    }

    void loadData() {
        ifstream infile(dataFile);
        int id; string name, type; double bal;
        while (infile >> id >> type >> bal) {
            infile.ignore();
            getline(infile, name); // Assuming name is at the end of the line
            if (type == "Savings") database.push_back(new SavingsAccount(id, name, bal));
            else database.push_back(new CurrentAccount(id, name, bal));
        }
    }

    void saveData() {
        ofstream outfile(dataFile);
        for (auto a : database) {
            outfile << a->getAccNum() << " " << a->getType() << " " 
                    << a->getBalance() << " " << a->getName() << endl;
        }
    }
};

// ================= Main Program =================
int main() {
    BankManager myBank;
    int choice;

    do {
        cout << "\n================================";
        cout << "\n    ELITE BANKING SYSTEM 2.0    ";
        cout << "\n================================";
        cout << "\n1. Create Account";
        cout << "\n2. View All Accounts";
        cout << "\n3. Deposit/Withdraw";
        cout << "\n4. Exit";
        cout << "\nSelection: ";
        cin >> choice;

        switch (choice) {
            case 1: myBank.createAccount(); break;
            case 2: myBank.displayAll(); break;
            case 3: myBank.processTransaction(); break;
            case 4: cout << "System shutting down... Data Saved.\n"; break;
            default: cout << "Invalid Selection.\n";
        }
    } while (choice != 4);

    return 0;
}