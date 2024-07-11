#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

using namespace std;

class Customer {
public:
    string name;
    string address;

    void serialize(ofstream& out) const {
        size_t nameLen = name.size();
        out.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        out.write(name.c_str(), nameLen);

        size_t addressLen = address.size();
        out.write(reinterpret_cast<const char*>(&addressLen), sizeof(addressLen));
        out.write(address.c_str(), addressLen);
    }

    void deserialize(ifstream& in) {
        size_t nameLen;
        in.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        name.resize(nameLen);
        in.read(&name[0], nameLen);

        size_t addressLen;
        in.read(reinterpret_cast<char*>(&addressLen), sizeof(addressLen));
        address.resize(addressLen);
        in.read(&address[0], addressLen);
    }
};

class Employee {
public:
    string name;
    float salary;

    void serialize(ofstream& out) const {
        size_t nameLen = name.size();
        out.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        out.write(name.c_str(), nameLen);

        out.write(reinterpret_cast<const char*>(&salary), sizeof(salary));
    }

    void deserialize(ifstream& in) {
        size_t nameLen;
        in.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        name.resize(nameLen);
        in.read(&name[0], nameLen);

        in.read(reinterpret_cast<char*>(&salary), sizeof(salary));
    }
};

class Sale {
public:
    Customer customer;
    Employee salerep;
    float amount;
    string date;

    void serialize(ofstream& out) const {
        customer.serialize(out);
        salerep.serialize(out);
        out.write(reinterpret_cast<const char*>(&amount), sizeof(amount));

        size_t dateLen = date.size();
        out.write(reinterpret_cast<const char*>(&dateLen), sizeof(dateLen));
        out.write(date.c_str(), dateLen);
    }

    void deserialize(ifstream& in) {
        customer.deserialize(in);
        salerep.deserialize(in);
        in.read(reinterpret_cast<char*>(&amount), sizeof(amount));

        size_t dateLen;
        in.read(reinterpret_cast<char*>(&dateLen), sizeof(dateLen));
        date.resize(dateLen);
        in.read(&date[0], dateLen);
    }
};

class DataStorage {
private:
    vector<Employee> employees;
    vector<Customer> customers;
    vector<Sale> sales;

public:
    void addEmployee(const string& name, float salary) {
        employees.push_back({name, salary});
    }

    void addCustomer(const string& name, const string& address) {
        customers.push_back({name, address});
    }

    void addSale(const Customer& customer, const Employee& salerep, float amount, const string& date) {
        sales.push_back({customer, salerep, amount, date});
    }

    void serialize(const string& filename) const {
        ofstream out(filename, ios::binary);
        if (!out) {
            cerr << "Cannot open file for writing.\n";
            return;
        }

        size_t employeeCount = employees.size();
        out.write(reinterpret_cast<const char*>(&employeeCount), sizeof(employeeCount));
        for (const auto& employee : employees) {
            employee.serialize(out);
        }

        size_t customerCount = customers.size();
        out.write(reinterpret_cast<const char*>(&customerCount), sizeof(customerCount));
        for (const auto& customer : customers) {
            customer.serialize(out);
        }

        size_t saleCount = sales.size();
        out.write(reinterpret_cast<const char*>(&saleCount), sizeof(saleCount));
        for (const auto& sale : sales) {
            sale.serialize(out);
        }

        out.close();
    }

    void deserialize(const string& filename) {
        ifstream in(filename, ios::binary);
        if (!in) {
            cerr << "File not found.\n";
            return;
        }

        size_t employeeCount, customerCount, saleCount;

        in.read(reinterpret_cast<char*>(&employeeCount), sizeof(employeeCount));
        employees.resize(employeeCount);
        for (auto& employee : employees) {
            employee.deserialize(in);
        }

        in.read(reinterpret_cast<char*>(&customerCount), sizeof(customerCount));
        customers.resize(customerCount);
        for (auto& customer : customers) {
            customer.deserialize(in);
        }

        in.read(reinterpret_cast<char*>(&saleCount), sizeof(saleCount));
        sales.resize(saleCount);
        for (auto& sale : sales) {
            sale.deserialize(in);
        }

        in.close();
    }

    void loadFile(const string& filename) {
        ifstream in(filename, ios::binary);
        if (in) {
            deserialize(filename);
        } else {
            cerr << "File " << filename << " not found!\n";
        }
    }

    void saveFile(const string& filename) const {
        serialize(filename);
    }
};

int main() {
    DataStorage data;
    string filename = "Data.bin";

    // Automatically load serialized data from Data.bin on startup
    data.loadFile(filename);

    // Automatically save serialized data to Data.bin before program exit
    atexit([&data, &filename] { data.saveFile(filename); });

    // Add employees, customers, and sales for testing
    data.addEmployee("John Doe", 50000.0f);
    data.addCustomer("Jane Smith", "123 Maple Street");

    // Find added customer and employee for adding a sale
    for (auto& customer : data.customers) {
        if (customer.name == "Jane Smith") {
            for (auto& employee : data.employees) {
                if (employee.name == "John Doe") {
                    data.addSale(customer, employee, 250.0f, "2024-07-10");
                }
            }
        }
    }

    // Rest of your program logic

    return 0;
}
