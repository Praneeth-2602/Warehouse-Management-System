#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <iomanip>
#include <sstream>

using namespace std;

// Function to trim whitespace
string trim(const string &str)
{
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// Hash function for password
string hashPassword(const string &password)
{
    unsigned long hash = 0;
    for (char c : password)
    {
        hash = (hash * 31) + c;
    }
    stringstream ss;
    ss << hex << hash; // Convert the hash to a hexadecimal string
    return ss.str();
}

// Product Class
class Product
{
private:
    string productID;
    string name;
    int quantity;
    double price;

public:
    Product(string id, string name, int qty, double price)
        : productID(id), name(name), quantity(qty), price(price) {}

    string getProductID() const { return productID; }
    string getName() const { return name; }
    int getQuantity() const { return quantity; }
    double getPrice() const { return price; }

    void updateQuantity(int qty) { quantity = qty; }
    void updatePrice(double newPrice) { price = newPrice; }
    void updateName(string newName) { name = newName; }

    void displayProduct() const
    {
        cout << "ID: " << productID << ", Name: " << name
             << ", Quantity: " << quantity << ", Price: $" << price << endl;
    }

    string toFileFormat() const
    {
        return productID + "," + name + "," + to_string(quantity) + "," + to_string(price);
    }

    static Product fromFileFormat(const string &line)
    {
        size_t pos = 0;
        vector<string> tokens;
        string modifiableLine = line;

        while ((pos = modifiableLine.find(',')) != string::npos)
        {
            tokens.push_back(modifiableLine.substr(0, pos));
            modifiableLine.erase(0, pos + 1);
        }
        tokens.push_back(modifiableLine);
        return Product(tokens[0], tokens[1], stoi(tokens[2]), stod(tokens[3]));
    }
};

// Order Class
class Order
{
private:
    string orderID;                   // Unique identifier for the order
    vector<string> orderedProductIDs; // List of product IDs in the order
    vector<int> quantities;           // List of quantities for each product
    time_t orderDate;                 // Date of the order
    static int orderCounter;          // Counter for order IDs

public:
    Order(string id, time_t date) : orderID(id), orderDate(date) {}

    void addProduct(const string &productID, int quantity)
    {
        orderedProductIDs.push_back(productID);
        quantities.push_back(quantity);
    }

    vector<string> getOrderProductIDs() const { return orderedProductIDs; }
    vector<int> getQuantities() const { return quantities; }
    string getOrderID() const { return orderID; }
    time_t getOrderDate() const { return orderDate; }

    void displayOrder(const vector<Product> &inventory) const
    {
        cout << "Order ID: " << orderID << ", Date: " << ctime(&orderDate);
        for (size_t i = 0; i < orderedProductIDs.size(); ++i)
        {
            auto it = find_if(inventory.begin(), inventory.end(), [&](const Product &p)
                              { return p.getProductID() == orderedProductIDs[i]; });
            if (it != inventory.end())
            {
                cout << "Product ID: " << orderedProductIDs[i] << ", Quantity: " << quantities[i] << endl;
                it->displayProduct();
            }
            else
            {
                cout << "Product ID " << orderedProductIDs[i] << " not found in inventory." << endl;
            }
        }
    }

    string toFileFormat() const
    {
        string result = orderID + "," + to_string(orderDate);
        for (size_t i = 0; i < orderedProductIDs.size(); ++i)
        {
            result += "|" + orderedProductIDs[i] + "," + to_string(quantities[i]);
        }
        return result;
    }

    static Order fromFileFormat(const string &line)
    {
        stringstream ss(line);
        string orderID;
        string dateStr;
        getline(ss, orderID, ',');
        getline(ss, dateStr, ',');
        time_t orderDate;
        try
        {
            orderDate = static_cast<time_t>(stoll(dateStr)); // Convert string to time_t
        }
        catch (const std::invalid_argument &e)
        {
            cerr << "Invalid date format in order file: " << dateStr << endl;
            orderDate = time(0); // Set to current time as a fallback
        }

        Order order(orderID, orderDate);
        string productData;
        while (getline(ss, productData, '|'))
        {
            size_t pos = productData.find(',');
            string productID = productData.substr(0, pos);
            int quantity = stoi(productData.substr(pos + 1));
            order.addProduct(productID, quantity);
        }
        return order;
    }
};

int Order::orderCounter = 1;

// Warehouse Class
class Warehouse
{
private:
    vector<Product> inventory;
    vector<Order> orders;

public:
    void addProduct(const Product &product)
    {
        inventory.push_back(product);
    }

    void addOrder()
    {
        // Generate a new order ID
        string orderID = "O" + to_string(orders.size() + 1);

        time_t now = time(0);
        Order newOrder(orderID, now);
        string productID;
        int quantity;
        char addMore;

        do
        {
            cout << "Enter Product ID to add to order: ";
            cin >> productID;
            cout << "Enter Quantity: ";
            cin >> quantity;

            // Find the product in the inventory
            auto it = find_if(inventory.begin(), inventory.end(), [&](Product &product)
                              { return product.getProductID() == productID; });

            if (it != inventory.end())
            {
                if (it->getQuantity() >= quantity)
                {
                    newOrder.addProduct(productID, quantity);
                    it->updateQuantity(it->getQuantity() - quantity);
                    cout << "Product " << productID << " added to the order.\n";
                }
                else
                {
                    cout << "Insufficient quantity in inventory.\n";
                }
            }
            else
            {
                cout << "Product ID " << productID << " not found in inventory.\n";
            }

            cout << "Add more products to the order? (y/n): ";
            cin >> addMore;

        } while (addMore == 'y' || addMore == 'Y');

        orders.push_back(newOrder);
        cout << "Order " << orderID << " added successfully!\n";
        system("pause"); // Pause after adding an order
    }

    void viewInventory() const
    {
        cout << "Inventory:" << endl;
        for (const auto &product : inventory)
        {
            product.displayProduct();
        }
        system("pause"); // Pause after viewing inventory
    }

    void viewOrders() const
    {
        cout << "Orders:" << endl;
        for (const auto &order : orders)
        {
            order.displayOrder(inventory);
        }
        system("pause"); // Pause after viewing orders
    }

    void searchProduct(const string &searchTerm)
    {
        cout << "Search Results for: " << searchTerm << endl;
        bool found = false;
        for (const auto &product : inventory)
        {
            if (product.getProductID() == searchTerm || product.getName() == searchTerm)
            {
                product.displayProduct();
                found = true;
            }
        }
        if (!found)
        {
            cout << "No products found matching: " << searchTerm << endl;
        }
        system("pause"); // Pause after search results
    }

    void deleteProduct(const string &id)
    {
        auto it = remove_if(inventory.begin(), inventory.end(), [&](const Product &product)
                            { return product.getProductID() == id; });
        if (it != inventory.end())
        {
            inventory.erase(it, inventory.end());
            cout << "Product deleted successfully!" << endl;
        }
        else
        {
            cout << "Product ID not found!" << endl;
        }
        system("pause"); // Pause after deleting a product
    }

    void updateProduct(const string &id)
    {
        auto it = find_if(inventory.begin(), inventory.end(), [&](Product &product)
                          { return product.getProductID() == id; });

        if (it != inventory.end())
        {
            int choice;
            cout << "1. Update Name\n";
            cout << "2. Update Quantity\n";
            cout << "3. Update Price\n";
            cout << "Enter the attribute to update: ";
            cin >> choice;

            switch (choice)
            {
            case 1:
            {
                string newName;
                cout << "Enter new name: ";
                cin >> newName;
                it->updateName(newName);
                cout << "Product name updated successfully.\n";
                break;
            }
            case 2:
            {
                int newQty;
                cout << "Enter new quantity: ";
                cin >> newQty;
                it->updateQuantity(newQty);
                cout << "Product quantity updated successfully.\n";
                break;
            }
            case 3:
            {
                double newPrice;
                cout << "Enter new price: ";
                cin >> newPrice;
                it->updatePrice(newPrice);
                cout << "Product price updated successfully.\n";
                break;
            }
            default:
                cout << "Invalid choice.\n";
            }
        }
        else
        {
            cout << "Product ID not found!\n";
        }
        system("pause"); // Pause after updating a product
    }

    void saveInventoryToFile(const string &filename) const
    {
        ofstream outFile(filename);
        for (const auto &product : inventory)
        {
            outFile << product.toFileFormat() << endl;
        }
        outFile.close();
    }

    void loadInventoryFromFile(const string &filename)
    {
        ifstream inFile(filename);
        string line;
        while (getline(inFile, line))
        {
            inventory.push_back(Product::fromFileFormat(line));
        }
        inFile.close();
    }

    void saveOrdersToFile(const string &filename) const
    {
        ofstream outFile(filename);
        for (const auto &order : orders)
        {
            outFile << order.toFileFormat() << endl;
        }
        outFile.close();
    }

    void loadOrdersFromFile(const string &filename)
    {
        ifstream inFile(filename);
        string line;
        while (getline(inFile, line))
        {
            orders.push_back(Order::fromFileFormat(line));
        }
        inFile.close();
    }
};

class SalesReport
{
public:
    void printBarChart(const string &timeFrame)
    {
        generateSalesReport();
        // Sample implementation for reading orders from a file
        vector<Order> filteredOrders = filterOrdersByTimeFrame(timeFrame);

        // Process the filtered orders to gather sales data
        unordered_map<string, int> salesData;
        for (const auto &order : filteredOrders)
        {
            const auto &productIDs = order.getOrderProductIDs();
            const auto &quantities = order.getQuantities();
            for (size_t i = 0; i < productIDs.size(); ++i)
            {
                salesData[productIDs[i]] += quantities[i]; // Sum quantities for each product
            }
        }

        // Find the maximum sales value for scaling the chart
        int maxSales = 0;
        for (const auto &data : salesData)
        {
            if (data.second > maxSales)
            {
                maxSales = data.second;
            }
        }

        // Print the bar chart
        cout << "Sales Report Bar Chart for " << timeFrame << ":" << endl;
        for (const auto &data : salesData)
        {
            cout << setw(10) << left << data.first << " | ";
            int barLength = static_cast<int>((data.second / static_cast<double>(maxSales)) * 50);
            for (int i = 0; i < barLength; ++i)
            {
                cout << "#";
            }
            cout << " " << data.second << endl;
        }
        system("pause"); // Pause after displaying the sales report
    }

    vector<Order> filterOrdersByTimeFrame(const string &timeFrame)
    {
        vector<Order> filteredOrders;
        ifstream ordersFile("orders.txt");
        string line;
        time_t now = time(0);
        tm *ltm = localtime(&now);

        while (getline(ordersFile, line))
        {
            Order order = Order::fromFileFormat(line);
            time_t orderDate = order.getOrderDate();

            // Calculate the time frames
            if (timeFrame == "last week")
            {
                tm lastWeek = *ltm;
                lastWeek.tm_mday -= 7; // Move back 7 days
                mktime(&lastWeek);     // Normalize the date
                if (orderDate >= mktime(&lastWeek) && orderDate <= now)
                {
                    filteredOrders.push_back(order);
                }
            }
            else if (timeFrame == "last month")
            {
                tm lastMonth = *ltm;
                lastMonth.tm_mon -= 1; // Move back 1 month
                mktime(&lastMonth);    // Normalize the date
                if (orderDate >= mktime(&lastMonth) && orderDate <= now)
                {
                    filteredOrders.push_back(order);
                }
            }
            else if (timeFrame == "last year")
            {
                tm lastYear = *ltm;
                lastYear.tm_year -= 1; // Move back 1 year
                mktime(&lastYear);     // Normalize the date
                if (orderDate >= mktime(&lastYear) && orderDate <= now)
                {
                    filteredOrders.push_back(order);
                }
            }
        }
        ordersFile.close();
        return filteredOrders;
    }

    void readInvoiceData()
    {
        // Placeholder: Implement logic to read invoice data from file or database
        cout << "Reading invoice data..." << endl;
    }

    void readStockData()
    {
        // Placeholder: Implement logic to read stock data from file or database
        cout << "Reading stock data..." << endl;
    }

    void generateSalesReport()
    {
        // Placeholder: Implement logic to generate a full sales report
        cout << "Generating sales report..." << endl;
    }

    void profitAnalysis()
    {
        // Placeholder: Implement logic for profit analysis based on sales and expenses
        cout << "Performing profit analysis..." << endl;
    }

    void stockLevelAnalysis()
    {
        // Placeholder: Implement logic for analyzing current stock levels
        cout << "Analyzing stock levels..." << endl;
    }

    void topSellingProducts()
    {
        // Placeholder: Implement logic to identify top-selling products based on sales
        cout << "Identifying top-selling products..." << endl;
    }

    void unsoldStockAnalysis()
    {
        // Placeholder: Implement logic to analyze unsold stock and identify slow-moving products
        cout << "Analyzing unsold stock..." << endl;
    }

    void printHeader()
    {
        // Placeholder: Implement header printing for reports
        cout << "===========================" << endl;
        cout << "         Sales Report       " << endl;
        cout << "===========================" << endl;
    }

    void printFooter()
    {
        // Placeholder: Implement footer printing for reports
        cout << "===========================" << endl;
        cout << "         End of Report      " << endl;
        cout << "===========================" << endl;
    }

private:
    // Helper function to extract year and week from order ID (e.g., "2023-45")
    pair<int, int> extractYearAndWeekFromOrderID(const string &orderID)
    {
        // Example implementation assuming orderID format is "YYYY-WW"
        int year = stoi(orderID.substr(0, 4));
        int week = stoi(orderID.substr(5, 2));
        return make_pair(year, week);
    }
};

// Admin Registration
bool registerAdmin()
{
    string username, password;
    cout << "\tRegister Admin" << endl;
    cout << "\tUsername: ";
    cin.ignore();
    getline(cin, username);
    username = trim(username);
    cout << "\tPassword: ";
    getline(cin, password);
    password = trim(password);

    string hashedPassword = hashPassword(password);

    ofstream outFile("admin_credentials.csv", ios::app);
    if (!outFile.is_open())
    {
        cout << "\tError: Unable to open admin credentials file." << endl;
        return false;
    }

    outFile << username << "," << hashedPassword << endl;
    outFile.close();

    cout << "\tAdmin registered successfully!" << endl;
    system("pause"); // Pause after admin registration
    return true;
}

// Admin Login
bool loginAdmin()
{
    string username, password;
    int failedAttempts = 0;
    const int MAX_FAILED_ATTEMPTS = 3;
    time_t lockTime = 0;
    const int LOCK_TIME_MINUTES = 15;

    cout << "\tLogin Admin" << endl;
    cout << "\tUsername: ";
    cin.ignore();
    getline(cin, username);
    username = trim(username);

    cout << "\tPassword: ";
    getline(cin, password);
    password = trim(password);

    string hashedPassword = hashPassword(password);

    ifstream inFile("admin_credentials.csv");
    if (!inFile.is_open())
    {
        cout << "\tError: Unable to open admin credentials file." << endl;
        return false;
    }

    string line, storedUsername, storedPassword;
    while (getline(inFile, line))
    {
        stringstream ss(line);
        getline(ss, storedUsername, ',');
        getline(ss, storedPassword, ',');

        if (storedUsername == username && storedPassword == hashedPassword)
        {
            cout << "\tAdmin logged in successfully!" << endl;
            inFile.close();
            return true;
        }
    }
    inFile.close();

    if (failedAttempts < MAX_FAILED_ATTEMPTS)
    {
        failedAttempts++;
        cout << "\tInvalid username or password. You have " << MAX_FAILED_ATTEMPTS - failedAttempts << " attempts left." << endl;
    }
    else
    {
        time_t currentTime = time(0);
        if (currentTime < lockTime)
        {
            cout << "\tAccount locked. Please wait " << (LOCK_TIME_MINUTES - (difftime(currentTime, lockTime) / 60)) << " minutes before trying again." << endl;
        }
        else
        {
            lockTime = currentTime + (LOCK_TIME_MINUTES * 60);
            cout << "\tAccount locked for " << LOCK_TIME_MINUTES << " minutes due to too many failed login attempts." << endl;
        }
    }

    return false;
}

// Customer Registration
bool registerCustomer()
{
    string username, password;
    cout << "\tRegister Customer" << endl;
    cout << "\tUsername: ";
    cin.ignore();
    getline(cin, username);
    username = trim(username);

    cout << "\tPassword: ";
    getline(cin, password);
    password = trim(password);

    string hashedPassword = hashPassword(password);

    ofstream outFile("customer_credentials.csv", ios::app);
    if (!outFile.is_open())
    {
        cout << "\tError: Unable to open customer credentials file." << endl;
        return false;
    }

    outFile << username << "," << hashedPassword << endl;
    outFile.close();

    cout << "\tCustomer registered successfully!" << endl;
    system("pause"); // Pause after customer registration
    return true;
}

// Customer Login
bool loginCustomer()
{
    string username, password;
    int failedAttempts = 0;
    const int MAX_FAILED_ATTEMPTS = 3;
    time_t lockTime = 0;
    const int LOCK_TIME_MINUTES = 15;

    cout << "\tLogin Customer" << endl;
    cout << "\tUsername: ";
    cin.ignore();
    getline(cin, username);
    username = trim(username);

    cout << "\tPassword: ";
    getline(cin, password);
    password = trim(password);

    string hashedPassword = hashPassword(password);

    ifstream inFile("customer_credentials.csv");
    if (!inFile.is_open())
    {
        cout << "\tError: Unable to open customer credentials file." << endl;
        return false;
    }

    string line, storedUsername, storedPassword;
    while (getline(inFile, line))
    {
        stringstream ss(line);
        getline(ss, storedUsername, ',');
        getline(ss, storedPassword, ',');

        // Check if the account is locked
        if (failedAttempts >= MAX_FAILED_ATTEMPTS)
        {
            time_t currentTime = time(0);
            if (currentTime < lockTime)
            {
                cout << "\tAccount locked. Please wait " << (LOCK_TIME_MINUTES - (difftime(currentTime, lockTime) / 60)) << " minutes before trying again." << endl;
                inFile.close();
                return false;
            }
            else
            {
                // Reset failed attempts after lock time
                failedAttempts = 0;
            }
        }

        // Validate username and password
        if (storedUsername == username && storedPassword == hashedPassword)
        {
            cout << "\tCustomer logged in successfully!" << endl;
            inFile.close();
            return true;
        }
    }
    inFile.close();

    failedAttempts++;
    cout << "\tInvalid username or password. You have " << (MAX_FAILED_ATTEMPTS - failedAttempts) << " attempts left." << endl;

    // Lock the account if max attempts are reached
    if (failedAttempts >= MAX_FAILED_ATTEMPTS)
    {
        lockTime = time(0) + (LOCK_TIME_MINUTES * 60); // Set the lock time
        cout << "\tAccount locked for " << LOCK_TIME_MINUTES << " minutes due to too many failed login attempts." << endl;
    }

    return false;
}

// Main Menu Functions
void displayHeader(const string &title)
{
    system("cls");
    cout << "\n\t************************************************************\n";
    cout << "\t*                                                          *\n";
    cout << "\t*               " << title << "                *\n";
    cout << "\t*                                                          *\n";
    cout << "\t************************************************************\n\n";
}

void salesReportMenu()
{
    int choice;
    do
    {
        displayHeader("Sales Report Menu");
        cout << "1. Last Week\n";
        cout << "2. Last Month\n";
        cout << "3. Last Year\n";
        cout << "4. Back to Admin Menu\n";
        cout << "Enter your choice: ";
        cin >> choice;

        SalesReport salesReport;
        switch (choice)
        {
        case 1:
            salesReport.printBarChart("last week");
            break;
        case 2:
            salesReport.printBarChart("last month");
            break;
        case 3:
            salesReport.printBarChart("last year");
            break;
        case 4:
            cout << "Returning to Admin Menu..." << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
        system("pause"); // Pause after sales report menu
    } while (choice != 4);
}

// Admin menu
void adminMenu(Warehouse &warehouse)
{
    int choice;
    do
    {
        displayHeader("Admin Menu");
        cout << "1. Add Product\n";
        cout << "2. Update Product\n";
        cout << "3. Remove Product\n";
        cout << "4. View Inventory\n";
        cout << "5. View Orders\n";
        cout << "6. Generate Sales Report\n"; // New option for sales report
        cout << "7. Logout\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            string id, name;
            int qty;
            double price;
            cout << "Enter Product ID: ";
            cin >> id;
            cout << "Enter Name: ";
            cin >> name;
            cout << "Enter Quantity: ";
            cin >> qty;
            cout << "Enter Price: ";
            cin >> price;
            warehouse.addProduct(Product(id, name, qty, price));
            cout << "Product added successfully!" << endl;
            system("pause"); // Pause after adding a product
            break;
        }
        case 2:
        {
            string id;
            cout << "Enter Product ID to Update: ";
            cin >> id;
            warehouse.updateProduct(id);
            break;
        }
        case 3:
        {
            string id;
            cout << "Enter Product ID to Remove: ";
            cin >> id;
            warehouse.deleteProduct(id);
            system("pause"); // Pause after removing a product
            break;
        }
        case 4:
            warehouse.viewInventory();
            break;
        case 5:
            warehouse.viewOrders();
            break;
        case 6:
        {
            salesReportMenu();
            break;
        }
        case 7:
            cout << "Logging out..." << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 7);
}

void customerMenu(Warehouse &warehouse)
{
    int choice;
    do
    {
        displayHeader("Customer Menu");
        cout << "1. View Inventory\n";
        cout << "2. Place Order\n";
        cout << "3. Logout\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            warehouse.viewInventory();
            break;
        case 2:
        {
            warehouse.addOrder();
            break;
        }
        case 3:
            cout << "Logging out..." << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 3);
}

// Main Function
int main()
{
    Warehouse warehouse;
    warehouse.loadInventoryFromFile("inventory.txt");
    warehouse.loadOrdersFromFile("orders.txt");

    int choice;
    do
    {
        displayHeader("Warehouse Management System");
        cout << "1. Admin Registration\n";
        cout << "2. Admin Login\n";
        cout << "3. Customer Registration\n";
        cout << "4. Customer Login\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            registerAdmin();
            break;
        case 2:
            if (loginAdmin())
            {
                adminMenu(warehouse);
            }
            break;
        case 3:
            registerCustomer();
            break;
        case 4:
            if (loginCustomer())
            {
                customerMenu(warehouse);
            }
            break;
        case 5:
            warehouse.saveInventoryToFile("inventory.txt");
            warehouse.saveOrdersToFile("orders.txt");
            cout << "Exiting the program. Thank you!" << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
        system("pause"); // Pause after main menu options
    } while (choice != 5);

    return 0;
}