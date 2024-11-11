#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <unordered_map>

using namespace std;

// Product Class
string login() {
    unordered_map<string, string> credentials = {
        {"admin", "admin123"}, // username:password for admin
        {"user", "user123"}    // username:password for regular user
    };

    string username, password;
    cout << "Login\n";
    cout << "Enter Username: ";
    cin >> username;
    cout << "Enter Password: ";
    cin >> password;

    if (credentials.find(username) != credentials.end() && credentials[username] == password) {
        cout << "Login Successful!\n";
        return username; // return user role
    } else {
        cout << "Invalid login credentials!\n";
        return ""; // return empty if login fails
    }
}
class Product {
private:
    string productID;
    string name;
    int quantity;
    double price;

public:
    // Constructor
    Product(string id, string name, int qty, double price)
        : productID(id), name(name), quantity(qty), price(price) {}

    // Getters
    string getProductID() const { return productID; }
    string getName() const { return name; }
    int getQuantity() const { return quantity; }
    double getPrice() const { return price; }

    // Setters
    void updateQuantity(int qty) { quantity = qty; }
    void updatePrice(double newPrice) { price = newPrice; }
    void updateName(string newName) { name = newName; }

    // Display product details
    void displayProduct() const {
        cout << "ID: " << productID << ", Name: " << name
             << ", Quantity: " << quantity << ", Price: $" << price << endl;
    }

    // Save product to file
    string toFileFormat() const {
        return productID + "," + name + "," + to_string(quantity) + "," + to_string(price);
    }

    // Load product from file
    static Product fromFileFormat(const string &line) {
        size_t pos = 0;
        vector<string> tokens;
        string modifiableLine = line;

        while ((pos = modifiableLine.find(',')) != string::npos) {
            tokens.push_back(modifiableLine.substr(0, pos));
            modifiableLine.erase(0, pos + 1);
        }
        tokens.push_back(modifiableLine); // Last token
    
        return Product(tokens[0], tokens[1], stoi(tokens[2]), stod(tokens[3]));
    }
};

// Order Class
class Order {
private:
    string orderID;
    vector<string> orderedProductIDs;

public:
    // Constructor
    Order(string id) : orderID(id) {}

    // Add product ID to order
    void addProductID(const string &productID) {
        orderedProductIDs.push_back(productID);
    }
    vector<string> getOrderProductIDs() const {
    return orderedProductIDs;
}

    // Display order details
    void displayOrder(const vector<Product> &inventory) const {
        cout << "Order ID: " << orderID << endl;
        for (const auto &productID : orderedProductIDs) {
            auto it = find_if(inventory.begin(), inventory.end(), [&](const Product &p) {
                return p.getProductID() == productID;
            });
            if (it != inventory.end()) {
                it->displayProduct();
            } else {
                cout << "Product ID " << productID << " not found in inventory." << endl;
            }
        }
    }

    // Save order to file
    string toFileFormat() const {
        string result = orderID;
        for (const auto &productID : orderedProductIDs) {
            result += "|" + productID;
        }
        return result;
    }

    // Load order from file
    static Order fromFileFormat(const string &line) {
        size_t pos = line.find('|');
        string orderID = line.substr(0, pos);
        Order order(orderID);

        size_t start = pos + 1;
        while ((pos = line.find('|', start)) != string::npos) {
            string productID = line.substr(start, pos - start);
            order.addProductID(productID);
            start = pos + 1;
        }
        if (start < line.length()) {
            order.addProductID(line.substr(start));
        }
        return order;
    }
};

// Warehouse Class
class Warehouse {
private:
    vector<Product> inventory;
    vector<Order> orders;

public:
    // Add product to inventory
    void addProduct(const Product &product) {
        inventory.push_back(product);
    }

    // View all products in inventory
    void viewInventory() const {
        cout << "Inventory:" << endl;
        for (const auto &product : inventory) {
            product.displayProduct();
        }
    }
    vector<Product> getInventory() const {
        return inventory;
    }

    // Add order
    void addOrder(const Order &order) {
        orders.push_back(order);
    }

    // View all orders
    void viewOrders() const {
        cout << "Orders:" << endl;
        for (const auto &order : orders) {
            order.displayOrder(inventory);
        }
    }

    void searchProduct(const string &searchTerm) {
        cout << "Search Results for: " << searchTerm << endl;
        bool found = false;
        for (const auto &product : inventory) {
            if (product.getProductID() == searchTerm || product.getName() == searchTerm) {
                product.displayProduct();
                found = true;
            }
        }
        if (!found) {
            cout << "No products found matching: " << searchTerm << endl;
        }
    }

    void deleteProduct(const string &id) {
        auto it = remove_if(inventory.begin(), inventory.end(), [&](const Product &product) {
            return product.getProductID() == id;
        });
        if (it != inventory.end()) {
            inventory.erase(it, inventory.end());
            cout << "Product deleted successfully!" << endl;
        } else {
            cout << "Product ID not found!" << endl;
        }
    }

    double calculateTotalSales() const {
    double totalSales = 0.0;
    for (const auto &order : orders) {
        for (const auto &productID : order.getOrderProductIDs()) {
            auto it = find_if(inventory.begin(), inventory.end(), [&](const Product &p) {
                return p.getProductID() == productID;
            });
            if (it != inventory.end()) {
                totalSales += it->getPrice() * it->getQuantity();
            }
        }
    }
    return totalSales;
}

    void saveInventoryToFile(const string &filename) const {
        ofstream outFile(filename);
        for (const auto &product : inventory) {
            outFile << product.toFileFormat() << endl;
        }
        outFile.close();
    }

    void loadInventoryFromFile(const string &filename) {
        ifstream inFile(filename);
        string line;
        while (getline(inFile, line)) {
            inventory.push_back(Product::fromFileFormat(line));
        }
        inFile.close();
    }

    void saveOrdersToFile(const string &filename) const {
        ofstream outFile(filename);
        for (const auto &order : orders) {
            outFile << order.toFileFormat() << endl;
        }
        outFile.close();
    }

    void loadOrdersFromFile(const string &filename) {
        ifstream inFile(filename);
        string line;
        while (getline(inFile, line)) {
            orders.push_back(Order::fromFileFormat(line));
        }
        inFile.close();
    }
};

// Main Function
int main() {
    Warehouse warehouse;
    warehouse.loadInventoryFromFile("inventory.txt");
    warehouse.loadOrdersFromFile("orders.txt");

    string role = login();  // Login function to determine user role
    if (role.empty()) {
        return 0; // Exit if login fails
    }

    int choice;
    do {
        cout << "\nWarehouse Management System\n";
        cout << "1. View Inventory\n";
        cout << "2. Search Product\n";
        cout << "3. View Orders\n";
        if (role == "admin") {
            cout << "4. Add Product to Inventory\n";
            cout << "5. Delete Product\n";
            cout << "6. Calculate Total Sales\n";
            cout << "7. Add Order\n";
        }
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            warehouse.viewInventory();
            break;
        case 2: {
            string searchTerm;
            cout << "Enter Product ID or Name to search: ";
            cin >> searchTerm;
            warehouse.searchProduct(searchTerm);
            break;
        }
        case 3:
            warehouse.viewOrders();
            break;
        case 4:
            if (role == "admin") {
                string id, name;
                int quantity;
                double price;
                cout << "Enter Product ID: ";
                cin >> id;
                cout << "Enter Product Name: ";
                cin >> name;
                cout << "Enter Quantity: ";
                cin >> quantity;
                cout << "Enter Price: ";
                cin >> price;

                Product newProduct(id, name, quantity, price);
                warehouse.addProduct(newProduct);
            } else {
                cout << "Access denied. Admin only feature.\n";
            }
            break;
        case 5:
            if (role == "admin") {
                string id;
                cout << "Enter Product ID to delete: ";
                cin >> id;
                warehouse.deleteProduct(id);
            } else {
                cout << "Access denied. Admin only feature.\n";
            }
            break;
        case 6:
            if (role == "admin") {
                cout << "Total Sales: $" << warehouse.calculateTotalSales() << endl;
            } else {
                cout << "Access denied. Admin only feature.\n";
            }
            break;
        case 7:
            if (role == "admin") {
                string orderID;
                cout << "Enter Order ID: ";
                cin >> orderID;
                Order newOrder(orderID);

                int numProducts;
                cout << "Enter number of products in the order: ";
                cin >> numProducts;

                for (int i = 0; i < numProducts; ++i) {
                    string prodID;
                    cout << "Enter Product ID to add: ";
                    cin >> prodID;
                    bool found = false;

                    for (const auto &product : warehouse.getInventory()) {
                        if (product.getProductID() == prodID) {
                            newOrder.addProductID(prodID);
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        cout << "Product ID not found in inventory!" << endl;
                    }
                }
                warehouse.addOrder(newOrder);
            } else {
                cout << "Access denied. Admin only feature.\n";
            }
            break;
        case 0:
            warehouse.saveInventoryToFile("inventory.txt");
            break;
        default:
            cout << "Invalid choice, please try again.\n";
        }
    } while (choice != 0);

    return 0;
}