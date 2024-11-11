#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <unordered_map>

using namespace std;

// Login function
string login() {
    unordered_map<string, string> credentials = {
        {"admin", "admin123"}, 
        {"user", "user123"}    
    };

    string username, password;
    cout << "Login\n";
    cout << "Enter Username: ";
    cin >> username;
    cout << "Enter Password: ";
    cin >> password;

    if (credentials.find(username) != credentials.end() && credentials[username] == password) {
        cout << "Login Successful!\n";
        return username;
    } else {
        cout << "Invalid login credentials!\n";
        return "";
    }
}

// Product Class
class Product {
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

    void displayProduct() const {
        cout << "ID: " << productID << ", Name: " << name
             << ", Quantity: " << quantity << ", Price: $" << price << endl;
    }

    string toFileFormat() const {
        return productID + "," + name + "," + to_string(quantity) + "," + to_string(price);
    }

    static Product fromFileFormat(const string &line) {
        size_t pos = 0;
        vector<string> tokens;
        string modifiableLine = line;

        while ((pos = modifiableLine.find(',')) != string::npos) {
            tokens.push_back(modifiableLine.substr(0, pos));
            modifiableLine.erase(0, pos + 1);
        }
        tokens.push_back(modifiableLine);
        return Product(tokens[0], tokens[1], stoi(tokens[2]), stod(tokens[3]));
    }
};

// Order Class
class Order {
private:
    string orderID;
    vector<string> orderedProductIDs;

public:
    Order(string id) : orderID(id) {}

    void addProductID(const string &productID) {
        orderedProductIDs.push_back(productID);
    }
    vector<string> getOrderProductIDs() const { return orderedProductIDs; }

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

    string toFileFormat() const {
        string result = orderID;
        for (const auto &productID : orderedProductIDs) {
            result += "|" + productID;
        }
        return result;
    }

    static Order fromFileFormat(const string &line) {
        size_t pos = line.find('|');
        string orderID = line.substr(0, pos);
        Order order(orderID);

        size_t start = pos + 1;
        while ((pos = line.find('|', start)) != string::npos) {
            order.addProductID(line.substr(start, pos - start));
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
    void addProduct(const Product &product) {
        inventory.push_back(product);
    }

    void addOrder() {
        string orderID;
        cout << "Enter Order ID: ";
        cin >> orderID;

        Order newOrder(orderID);
        string productID;
        char addMore;

        do {
            cout << "Enter Product ID to add to order: ";
            cin >> productID;

            // Find the product in the inventory
            auto it = find_if(inventory.begin(), inventory.end(), [&](const Product &product) {
                return product.getProductID() == productID;
            });

            if (it != inventory.end()) {
                newOrder.addProductID(productID);
                cout << "Product " << productID << " added to the order.\n";
            } else {
                cout << "Product ID " << productID << " not found in inventory.\n";
            }

            cout << "Add more products to the order? (y/n): ";
            cin >> addMore;

        } while (addMore == 'y' || addMore == 'Y');

        orders.push_back(newOrder);
        cout << "Order " << orderID << " added successfully!\n";
    }

    void viewInventory() const {
        cout << "Inventory:" << endl;
        for (const auto &product : inventory) {
            product.displayProduct();
        }
    }

    void viewOrders() const {
        cout << "Orders:" << endl;
        for (const auto &order : orders) {
            order.displayOrder(inventory);
        }
    }

    void addOrder(const Order &order) {
        orders.push_back(order);
    }

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

    void updateProduct(const string &id) {
        auto it = find_if(inventory.begin(), inventory.end(), [&](Product &product) {
            return product.getProductID() == id;
        });

        if (it != inventory.end()) {
            int choice;
            cout << "1. Update Name\n";
            cout << "2. Update Quantity\n";
            cout << "3. Update Price\n";
            cout << "Enter the attribute to update: ";
            cin >> choice;

            switch (choice) {
            case 1: {
                string newName;
                cout << "Enter new name: ";
                cin >> newName;
                it->updateName(newName);
                cout << "Product name updated successfully.\n";
                break;
            }
            case 2: {
                int newQty;
                cout << "Enter new quantity: ";
                cin >> newQty;
                it->updateQuantity(newQty);
                cout << "Product quantity updated successfully.\n";
                break;
            }
            case 3: {
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
        } else {
            cout << "Product ID not found!\n";
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

    string role = login();
    if (role.empty()) {
        return 0;
    }

    int choice;
    do {
        cout << "\nWarehouse Management System\n";
        cout << "1. View Inventory\n";
        cout << "2. Search Product\n";
        cout << "3. View Orders\n";
        cout << "4. Add Order\n";   // New option for adding an order
        if (role == "admin") {
            cout << "5. Add Product to Inventory\n";
            cout << "6. Delete Product\n";
            cout << "7. Calculate Total Sales\n";
            cout << "8. Update Product\n";
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
            warehouse.addOrder();  // Calling the new addOrder function
            break;
        case 5:
            if (role == "admin") {
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
                cout << "Product added successfully!\n";
            }
            break;
        case 6:
            if (role == "admin") {
                string id;
                cout << "Enter Product ID to delete: ";
                cin >> id;
                warehouse.deleteProduct(id);
            }
            break;
        case 7:
            if (role == "admin") {
                cout << "Total Sales: $" << warehouse.calculateTotalSales() << endl;
            }
            break;
        case 8:
            if (role == "admin") {
                string id;
                cout << "Enter Product ID to update: ";
                cin >> id;
                warehouse.updateProduct(id);
            }
            break;
        case 0:
            warehouse.saveInventoryToFile("inventory.txt");
            warehouse.saveOrdersToFile("orders.txt");
            cout << "Exiting...\n";
            break;
        default:
            cout << "Invalid choice! Please try again.\n";
        }
    } while (choice != 0);

    return 0;
}