// main.cpp
// Small Shop Supplier Reorder Assistant - Phase 2 (CA2)
// Menu-driven console program. Phase 1: Create and Read with validation.
// Phase 2: products are saved in a binary file, searched with seekg and
// updated in place with seekp, so the data survives a restart.
//
// Build:  g++ -std=c++11 -Wall -o reorder src/main.cpp src/Product.cpp src/ProductFile.cpp src/ProductStore.cpp src/Utils.cpp

#include <iostream>
#include <string>
#include "Product.h"
#include "ProductStore.h"
#include "Utils.h"

using namespace std;

// ---------------------------------------------------------------------------
// Input helpers. Every value is read as a whole line with getline() and then
// checked, so wrong input such as "abc" for a number never leaves cin in a
// failed state. Each helper returns false if the user leaves the line blank
// (cancel) or input ends; otherwise it fills "out" (call by reference).
// ---------------------------------------------------------------------------
bool readLine(const string& prompt, string& out) {
    cout << prompt;
    if (!getline(cin, out)) {          // end of input (Ctrl+Z / Ctrl+D / end of file)
        out = "";
        return false;
    }
    return true;
}

bool readTextField(const string& prompt, string& out) {
    string input;
    while (readLine(prompt, input)) {
        if (trim(input).empty()) return false;              // blank = cancel
        if (Product::isValidText(input)) { out = input; return true; }
        cout << "  Invalid: use 1-" << Product::TEXT_MAX_LEN
             << " printable characters. Try again.\n";
    }
    return false;
}

bool readPhoneField(const string& prompt, string& out) {
    string input;
    while (readLine(prompt, input)) {
        if (trim(input).empty()) return false;
        if (Product::isValidPhone(input)) { out = input; return true; }
        cout << "  Invalid: phone must be exactly 10 digits. Try again.\n";
    }
    return false;
}

bool readIntField(const string& prompt, int minValue, int maxValue, int& out) {
    string input;
    while (readLine(prompt, input)) {
        if (trim(input).empty()) return false;
        if (parseInt(input, minValue, maxValue, out)) return true;
        cout << "  Invalid: enter a whole number from " << minValue
             << " to " << maxValue << ". Try again.\n";
    }
    return false;
}

bool readMoneyField(const string& prompt, double& out) {
    string input;
    while (readLine(prompt, input)) {
        if (trim(input).empty()) return false;
        if (parseMoney(input, out) && Product::isValidCost(out)) return true;
        cout << "  Invalid: enter an amount above 0 and up to 1000000, "
                "with at most 2 decimals (e.g. 45 or 45.50). Try again.\n";
    }
    return false;
}

// ---------------------------------------------------------------------------
// Menu actions. The store is passed BY REFERENCE (ProductStore& store), so
// the function works on the real store in main(), not on a copy.
// ---------------------------------------------------------------------------
void addProductMenu(ProductStore& store) {
    if (store.isFull()) {
        cout << addResultMessage(ADD_FULL) << "\n";
        return;
    }
    cout << "\n--- Add Product (leave any field blank to cancel) ---\n";

    Product temp;                       // empty object, filled field by field
    string input;

    // 1) Product code: checked for format AND duplicates straight away,
    //    so the user does not type 7 more fields for nothing.
    while (true) {
        if (!readLine("Product code (3-10 chars: A-Z, 0-9, -): ", input) ||
            trim(input).empty()) {
            cout << "Add cancelled.\n";
            return;
        }
        string code = Product::normalizeCode(input);
        if (!Product::isValidCode(code)) {
            cout << "  Invalid: code must be 3-10 characters using letters, "
                    "digits or '-', starting with a letter or digit.\n";
            continue;
        }
        if (store.findByCode(code) != nullptr) {
            cout << "  Duplicate: product " << code
                 << " already exists. Enter a different code.\n";
            continue;
        }
        temp.setProductCode(code);
        break;
    }

    string text;
    int number = 0;
    double money = 0.0;

    if (!readTextField("Product name: ", text))  { cout << "Add cancelled.\n"; return; }
    temp.setName(text);

    if (!readTextField("Supplier name: ", text)) { cout << "Add cancelled.\n"; return; }
    temp.setSupplierName(text);

    if (!readPhoneField("Supplier phone (10 digits): ", text)) { cout << "Add cancelled.\n"; return; }
    temp.setSupplierPhone(text);

    if (!readIntField("Current stock (units): ", 0, Product::QTY_MAX, number)) { cout << "Add cancelled.\n"; return; }
    temp.setCurrentStock(number);

    if (!readIntField("Reorder level (reorder when stock <= this): ", 0, Product::QTY_MAX, number)) { cout << "Add cancelled.\n"; return; }
    temp.setReorderLevel(number);

    if (!readIntField("Reorder quantity (units per order): ", 1, Product::QTY_MAX, number)) { cout << "Add cancelled.\n"; return; }
    temp.setReorderQty(number);

    if (!readMoneyField("Unit cost (Rs.): ", money)) { cout << "Add cancelled.\n"; return; }
    temp.setUnitCost(money);

    // The store runs its own checks again (valid, duplicate, full), so it
    // stays safe even if it is called from somewhere other than this menu.
    AddResult result = store.addProduct(temp);
    cout << addResultMessage(result) << "\n";
    if (result == ADD_OK && temp.needsReorder())
        cout << "Note: stock is already at or below the reorder level.\n";
}

// Phase 2: the search now reads the DATA FILE with seekg, one record at a
// time, and reports where on disk the product was found.
void searchProductMenu(ProductStore& store) {
    string input;
    if (!readLine("\nEnter product code to search: ", input) || trim(input).empty()) {
        cout << "Search cancelled.\n";
        return;
    }
    Product found;                                  // default constructor
    int recordIndex = store.searchFile(input, found);
    if (recordIndex < 0) {
        cout << "No product found with code " << Product::normalizeCode(input) << ".\n";
        return;
    }
    cout << "Found in the data file: record " << (recordIndex + 1)
         << ", starting at byte " << (long)recordIndex * ProductFile::RECORD_SIZE << ".\n\n";
    Product::displayHeader();
    displayProduct(found);
}

// Phase 2: read one record directly by its number (random access).
void viewRecordMenu(ProductStore& store) {
    int total = store.fileRecordCount();
    if (total == 0) {
        cout << "\nThe data file has no records yet.\n";
        return;
    }
    cout << "\nThe data file holds " << total << " record(s).\n";
    int number = 0;
    if (!readIntField("Record number to view: ", 1, total, number)) {
        cout << "Cancelled.\n";
        return;
    }
    Product p;
    if (!store.readRecord(number - 1, p)) {
        cout << "Could not read that record.\n";
        return;
    }
    cout << "Record " << number << " of " << total << ", starting at byte "
         << (long)(number - 1) * ProductFile::RECORD_SIZE << ":\n\n";
    Product::displayHeader();
    displayProduct(p);
}

// Phase 2: change one field of a product and save it in place with seekp.
void updateProductMenu(ProductStore& store) {
    string input;
    if (!readLine("\nEnter code of the product to update: ", input) || trim(input).empty()) {
        cout << "Update cancelled.\n";
        return;
    }
    const Product* found = store.findByCode(input);
    if (found == nullptr) {
        cout << "No product found with code " << Product::normalizeCode(input) << ".\n";
        return;
    }

    Product before(*found);     // COPY CONSTRUCTOR: keep the old values to show later
    Product changed(*found);    // a second copy, which we edit

    cout << "\nCurrent details:\n";
    Product::displayHeader();
    displayProduct(before);

    cout << "\nWhat do you want to change? (the product code cannot change)\n"
         << "1. Current stock\n"
         << "2. Reorder level\n"
         << "3. Reorder quantity\n"
         << "4. Unit cost\n"
         << "5. Supplier phone\n"
         << "0. Cancel\n";
    int field = 0;
    if (!readIntField("Enter choice: ", 0, 5, field) || field == 0) {
        cout << "Update cancelled.\n";
        return;
    }

    int number = 0;
    double money = 0.0;
    string text;
    bool ok = false;
    switch (field) {
        case 1: ok = readIntField("New current stock: ", 0, Product::QTY_MAX, number)
                     && changed.setCurrentStock(number); break;
        case 2: ok = readIntField("New reorder level: ", 0, Product::QTY_MAX, number)
                     && changed.setReorderLevel(number); break;
        case 3: ok = readIntField("New reorder quantity: ", 1, Product::QTY_MAX, number)
                     && changed.setReorderQty(number); break;
        case 4: ok = readMoneyField("New unit cost (Rs.): ", money)
                     && changed.setUnitCost(money); break;
        case 5: ok = readPhoneField("New supplier phone (10 digits): ", text)
                     && changed.setSupplierPhone(text); break;
    }
    if (!ok) {
        cout << "Update cancelled.\n";
        return;
    }

    UpdateResult result = store.updateProduct(changed);
    cout << updateResultMessage(result) << "\n";
    if (result == UPDATE_OK) {
        cout << "\nBefore:\n";
        Product::displayHeader();
        displayProduct(before);
        cout << "After:\n";
        displayProduct(changed);
    }
}

void showSummary(ProductStore& store) {
    cout << "\n--- Summary ---\n";
    cout << "Products stored            : " << store.getCount() << " / "
         << Product::MAX_PRODUCTS << "\n";
    cout << "Loaded from file at start  : " << store.getLoadedCount() << "\n";
    cout << "Added in this run          : " << Product::getTotalAdded() << "\n";
    cout << "Need reorder now           : " << store.countNeedingReorder() << "\n";
    cout << "Data file                  : " << store.getFilePath()
         << (store.isFileOpen() ? "" : "  (NOT OPEN)") << "\n";
    cout << "Records in file            : " << store.fileRecordCount() << "\n";
    cout << "Record size                : " << ProductFile::RECORD_SIZE << " bytes\n";
    cout << "File size                  : " << store.fileSizeInBytes() << " bytes\n";
    if (store.getSkippedCount() > 0)
        cout << "Damaged/duplicate skipped  : " << store.getSkippedCount() << "\n";
}

// Adds a few ready-made products for the demo. Each element of the array is
// built with the PARAMETERISED CONSTRUCTOR. Running it twice shows the
// duplicate-key check in action, even after a restart.
void loadDemoProducts(ProductStore& store) {
    Product demo[5] = {
        Product("RC-101",  "Basmati Rice 5kg",    "Sharma Traders",   "9876543210", 8,  10, 20, 410.00),
        Product("OIL-01",  "Mustard Oil 1L",      "Punjab Oil Depot", "9812345678", 25, 12, 24, 165.50),
        Product("SUG-01",  "Sugar 1kg",           "Sharma Traders",   "9876543210", 5,  15, 30, 44.00),
        Product("TEA-250", "Tea Leaves 250g",     "Assam Tea House",  "9123456780", 40, 10, 20, 120.00),
        Product("SOAP-04", "Bath Soap Pack of 4", "Gupta Wholesale",  "9988776655", 12, 12, 10, 150.00)
    };
    for (int i = 0; i < 5; ++i) {
        AddResult r = store.addProduct(demo[i]);
        cout << "  " << demo[i].getProductCode() << ": " << addResultMessage(r) << "\n";
    }
}

void showMenu() {
    cout << "\n========== SMALL SHOP SUPPLIER REORDER ASSISTANT ==========\n"
         << "1. Add product\n"
         << "2. View all products\n"
         << "3. Search product by code (in the data file)\n"
         << "4. View reorder list\n"
         << "5. Summary\n"
         << "6. Load demo products\n"
         << "7. Update a product\n"
         << "8. View a record by its number\n"
         << "0. Exit\n";
}

int main() {
    // The constructor opens data/products.dat and loads every saved product.
    // When main() ends, "store" goes out of scope, and the ProductFile
    // destructor inside it closes the file.
    ProductStore store;

    if (store.isFileOpen())
        cout << "Data file " << store.getFilePath() << " opened. "
             << store.getLoadedCount() << " product(s) loaded.\n";
    else
        cout << "Warning: could not open " << store.getFilePath()
             << ". Products cannot be saved in this run.\n";
    if (store.getSkippedCount() > 0)
        cout << "Warning: " << store.getSkippedCount()
             << " damaged or duplicate record(s) were skipped.\n";

    string input;
    int choice = -1;

    while (true) {
        showMenu();
        if (!readLine("Enter choice: ", input)) {
            cout << "\nInput ended. Exiting.\n";
            break;
        }
        if (!parseInt(input, 0, 8, choice)) {
            cout << "Invalid choice. Enter a number from 0 to 8.\n";
            continue;
        }
        if (choice == 0) {
            cout << "Goodbye! All changes are already saved in "
                 << store.getFilePath() << ".\n";
            break;
        }
        switch (choice) {
            case 1: addProductMenu(store); break;
            case 2: cout << "\n"; store.displayAll(); break;
            case 3: searchProductMenu(store); break;
            case 4: cout << "\n"; store.displayReorderList(); break;
            case 5: showSummary(store); break;
            case 6: loadDemoProducts(store); break;
            case 7: updateProductMenu(store); break;
            case 8: viewRecordMenu(store); break;
        }
    }
    return 0;
}
