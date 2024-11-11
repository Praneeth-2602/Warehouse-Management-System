#include <iostream>
#include <vector>
#include <string>
#include <fstream> // For file handling
#include <algorithm> // For remove_if

using namespace std;

// Product Class
class Product
{
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
    void displayProduct() const
    {
        cout << "ID: " << productID << ", Name: " << name
            << ", Quantity: " << quantity << ", Price: $" << price << endl;
    }

    // Save product to file
    string toFileFormat() const
    {
        return productID + "," + name + "," + to_string(quantity) + "," + to_string(price);
    }

    // Load product from file
    static Product fromFileFormat(const string &line)
    {
        size_t pos = 0;
        vector<string> tokens;
        string modifiableLine = line; // Create a modifiable copy of the line
        while ((pos = modifiableLine.find(',')) != string::npos)
        {
            tokens.push_back(modifiableLine.substr(0, pos));
            modifiableLine.erase(0, pos + 1);
        }
        tokens.push_back(modifiableLine); // Last token

        return Product(tokens[0], tokens[1], stoi(tokens[2]), stod(tokens[3]));
    }
};

// Order Class
class Order
{
private:
    string orderID;
    vector<Product> orderedProducts;

public:
    // Constructor
    Order(string id) : orderID(id) {}

    // Getter for ordered products
    const vector<Product> &getProducts() const { return orderedProducts; }

    // Add product to order
    void addProduct(const Product &product)
    {
        orderedProducts.push_back(product);
    }

    // Display order details
    void displayOrder() const
    {
        cout << "Order ID: " << orderID << endl;
        for (const auto &product : orderedProducts)
        {
            product.displayProduct();
        }
    }

    // Save order to file
    string toFileFormat() const
    {
        string result = orderID;
        for (const auto &product : orderedProducts)
        {
            result += "|" + product.getProductID(); // Use "|" to separate products
        }
        return result;
    }

    // Load order from file
    static Order fromFileFormat(const string &line)
    {
        size_t pos = line.find('|');
        string orderID = line.substr(0, pos);
        Order order(orderID);

        size_t start = pos + 1;
        while ((pos = line.find('|', start)) != string::npos)
        {
            string productID = line.substr(start, pos - start);
            order.addProduct(Product::fromFileFormat(productID)); // Assuming products exist
            start = pos + 1;
        }
        // Last product
        if (start < line.length())
        {
            string productID = line.substr(start);
            order.addProduct(Product::fromFileFormat(productID));
        }

        return order;
    }
};

// Warehouse Class
class Warehouse
{
private:
    vector<Product> inventory;
    vector<Order> orders;

public:
    // Getter for inventory
    const vector<Product> &getInventory() const { return inventory; }

    // Add product to inventory
    void addProduct(const Product &product)
    {
        inventory.push_back(product);
    }

    // View all products in inventory
    void viewInventory() const
    {
        cout << "Inventory:" << endl;
        for (const auto &product : inventory)
        {
            product.displayProduct();
        }
    }

    // Add order
    void addOrder(const Order &order)
    {
        orders.push_back(order);
    }

    // View all orders
    void viewOrders() const
    {
        cout << "Orders:" << endl;
        for (const auto &order : orders)
        {
            order.displayOrder();
        }
    }

    void searchProduct(const string &searchTerm)
    {
        cout << "Search Results for: " << searchTerm << endl;
        bool found = false;
        for (const auto &product : inventory)
        {
            if (product.getProductID() == searchTerm ||
                product.getName() == searchTerm)
            {
                product.displayProduct();
                found = true;
            }
        }
        if (!found)
        {
            cout << "No products found matching: " << searchTerm << endl;
        }
    }

    // Update product in inventory
    void updateProduct(const string &id)
    {
        for (auto &product : inventory)
        {
            if (product.getProductID() == id)
            {
                string newName = product.getName();      // Default to current name
                int newQuantity = product.getQuantity(); // Default to current quantity
                double newPrice = product.getPrice();    // Default to current price
                char choice;

                // Ask user if they want to update name
                cout << "Current Name: " << product.getName() << endl;
                cout << "Do you want to update the name? (y/n): ";
                cin >> choice;
                if (choice == 'y' || choice == 'Y')
                {
                    cout << "Enter new name: ";
                    cin >> newName;
                }

                // Ask user if they want to update quantity
                cout << "Current Quantity: " << product.getQuantity() << endl;
                cout << "Do you want to update the quantity? (y/n): ";
                cin >> choice;
                if (choice == 'y' || choice == 'Y')
                {
                    cout << "Enter new quantity: ";
                    cin >> newQuantity;
                }

                // Ask user if they want to update price
                cout << "Current Price: $" << product.getPrice() << endl;
                cout << "Do you want to update the price? (y/n): ";
                cin >> choice;
                if (choice == 'y' || choice == 'Y')
                {
                    cout << "Enter new price: ";
                    cin >> newPrice;
                }

                // Update the product with new values
                product.updateQuantity(newQuantity);
                product.updatePrice(newPrice);
                cout << "Product updated successfully!" << endl;
                return;
            }
        }
        cout << "Product ID not found!" << endl;
    }

    // Delete product from inventory
    void deleteProduct(const string &id)
    {
        auto it = std::remove_if(inventory.begin(), inventory.end(), [&](const Product &product)
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
    }

    // Calculate total sales
    double calculateTotalSales() const
    {
        double totalSales = 0.0;
        for (const auto &order : orders)
        {
            for (const auto &product : order.getProducts())
            {
                totalSales += product.getPrice() * product.getQuantity(); // Assuming quantity is tracked
            }
        }
        return totalSales;
    }

    // Save inventory to file
    void saveInventoryToFile(const string &filename) const
    {
        ofstream outFile(filename);
        for (const auto &product : inventory)
        {
            outFile << product.toFileFormat() << endl;
        }
        outFile.close();
    }

    // Load inventory from file
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

    // Save orders to file
    void saveOrdersToFile(const string &filename) const
    {
        ofstream outFile(filename);
        for (const auto &order : orders)
        {
            outFile << order.toFileFormat() << endl;
        }
        outFile.close();
    }

    // Load orders from file
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

// Main Function
int main()
{
    Warehouse warehouse;

    // Load inventory and orders from files
    warehouse.loadInventoryFromFile("inventory.txt");
    warehouse.loadOrdersFromFile("orders.txt");

    // Menu for user interaction
    int choice;
    do
    {
        cout << "\nWarehouse Management System\n";
        cout << "1. View Inventory\n";
        cout << "2. Add Product to Inventory\n";
        cout << "3. View Orders\n";
        cout << "4. Add Order\n";
        cout << "5. Search Product\n";
        cout << "6. Update Product\n";
        cout << "7. Delete Product\n";
        cout << "8. Calculate Total Sales\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            warehouse.viewInventory();
            break;

        case 2:
        {
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
            break;
        }

        case 3:
            warehouse.viewOrders();
            break;

        case 4:
        {
            string id;
            cout << "Enter Order ID: ";
            cin >> id;
            Order newOrder(id);

            string productID;
            char choice;
            do
            {
                cout << "Enter Product ID to add to order: ";
                cin >> productID;
                // Search for product in inventory
                bool found = false;
                for (const auto &product : warehouse.getInventory())
                {
                    if (product.getProductID() == productID)
                    {
                        newOrder.addProduct(product);
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    cout << "Product ID not found in inventory!" << endl;
                }

                cout << "Do you want to add another product to the order? (y/n): ";
                cin >> choice;
            } while (choice == 'y' || choice == 'Y');

            warehouse.addOrder(newOrder);
            break;
        }
        case 5:
        {
            string searchTerm;
            cout << "Enter Product ID or Name to search: ";
            cin >> searchTerm;
            warehouse.searchProduct(searchTerm);
            break;
        }

        case 6:
        {
            string id;
            cout << "Enter Product ID to update: ";
            cin >> id;
            warehouse.updateProduct(id);
            break;
        }

        case 7:
        {
            string id;
            cout << "Enter Product ID to delete: ";
            cin >> id;
            warehouse.deleteProduct(id);
            break;
        }

        case 8:
        {
            cout << "Total Sales: $" << warehouse.calculateTotalSales() << endl;
            break;
        }

        case 0:
            // Save inventory and orders to files before exiting
            warehouse.saveInventoryToFile("inventory.txt");
            warehouse.saveOrdersToFile("orders.txt");
            cout << "Exiting the program. Goodbye!" << endl;
            break;

        default:
            cout << "Invalid choice. Please try again." << endl;
            break;
        }
    } while (choice != 0);

    return 0;
}
