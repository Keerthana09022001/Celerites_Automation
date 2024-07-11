#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[100];
    char address[200];
} Customer;

typedef struct {
    char name[100];
    float salary;
} Employee;

typedef struct {
    Customer* customer;
    Employee* salerep;
    float amount;
    char date[20];
} Sale;

Customer* customers = NULL;
size_t customerCount = 0;

Employee* employees = NULL;
size_t employeeCount = 0;

Sale* sales = NULL;
size_t saleCount = 0;

void addEmployee(const char* name, float salary) {
    employees = realloc(employees, (employeeCount + 1) * sizeof(Employee));
    strcpy(employees[employeeCount].name, name);
    employees[employeeCount].salary = salary;
    employeeCount++;
}

void addCustomer(const char* name, const char* address) {
    customers = realloc(customers, (customerCount + 1) * sizeof(Customer));
    strcpy(customers[customerCount].name, name);
    strcpy(customers[customerCount].address, address);
    customerCount++;
}

void addSale(Customer* customer, Employee* salerep, float amount, const char* date) {
    sales = realloc(sales, (saleCount + 1) * sizeof(Sale));
    sales[saleCount].customer = customer;
    sales[saleCount].salerep = salerep;
    sales[saleCount].amount = amount;
    strcpy(sales[saleCount].date, date);
    saleCount++;
}

void serialize() {
    FILE* file = fopen("Data.bin", "wb");

    fwrite(&employeeCount, sizeof(size_t), 1, file);
    fwrite(employees, sizeof(Employee), employeeCount, file);

    fwrite(&customerCount, sizeof(size_t), 1, file);
    fwrite(customers, sizeof(Customer), customerCount, file);

    fwrite(&saleCount, sizeof(size_t), 1, file);
    for (size_t i = 0; i < saleCount; i++) {
        fwrite(sales[i].customer, sizeof(Customer), 1, file);
        fwrite(sales[i].salerep, sizeof(Employee), 1, file);
        fwrite(&sales[i].amount, sizeof(float), 1, file);
        fwrite(sales[i].date, sizeof(sales[i].date), 1, file);
    }

    fclose(file);
}

void deserialize() {
    FILE* file = fopen("Data.bin", "rb");
    if (file == NULL) {
        printf("Data.bin not found!\n");
        return;
    }

    fread(&employeeCount, sizeof(size_t), 1, file);
    employees = realloc(employees, employeeCount * sizeof(Employee));
    fread(employees, sizeof(Employee), employeeCount, file);

    fread(&customerCount, sizeof(size_t), 1, file);
    customers = realloc(customers, customerCount * sizeof(Customer));
    fread(customers, sizeof(Customer), customerCount, file);

    fread(&saleCount, sizeof(size_t), 1, file);
    sales = realloc(sales, saleCount * sizeof(Sale));
    for (size_t i = 0; i < saleCount; i++) {
        sales[i].customer = malloc(sizeof(Customer));
        fread(sales[i].customer, sizeof(Customer), 1, file);
        sales[i].salerep = malloc(sizeof(Employee));
        fread(sales[i].salerep, sizeof(Employee), 1, file);
        fread(&sales[i].amount, sizeof(float), 1, file);
        fread(sales[i].date, sizeof(sales[i].date), 1, file);
    }

    fclose(file);
}

void loadFile() {
    deserialize();
}

void writeFile() {
    serialize();
}

int main() {
    atexit(writeFile);

    loadFile();

    addEmployee("John Doe", 50000.0f);
    addCustomer("Jane Smith", "123 Maple Street");

    Customer* customer = NULL;
    Employee* employee = NULL;

    for (size_t i = 0; i < customerCount; i++) {
        if (strcmp(customers[i].name, "Jane Smith") == 0) {
            customer = &customers[i];
            break;
        }
    }

    for (size_t i = 0; i < employeeCount; i++) {
        if (strcmp(employees[i].name, "John Doe") == 0) {
            employee = &employees[i];
            break;
        }
    }

    if (customer && employee) {
        addSale(customer, employee, 250.0f, "2024-07-10");
    }



    return 0;
}
