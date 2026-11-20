// test_phase2.cpp
// Automated tests for Phase 2 (CA2): constructors and destructor, binary file
// persistence, reading records, search with seekg and update with seekp.
// Normal, invalid and boundary cases.
//
// Build & run (from the project root):
//   g++ -std=c++11 -Wall -Isrc -o test_phase2 tests/test_phase2.cpp src/Product.cpp src/ProductFile.cpp src/ProductStore.cpp src/Utils.cpp
//   ./test_phase2
//
// "Restart" is tested by letting a ProductStore go out of scope (its
// ProductFile destructor closes the file) and then building a NEW store on
// the same file, exactly what happens when the program is closed and run again.

#include <iostream>
#include <fstream>
#include <string>
#include "Product.h"
#include "ProductFile.h"
#include "ProductStore.h"
#include "Utils.h"

using namespace std;

const string TEST_FILE = "tests/test_phase2.dat";

int passed = 0;
int failed = 0;

void check(bool condition, const string& testName) {
    if (condition) {
        passed++;
        cout << "[PASS] " << testName << "\n";
    } else {
        failed++;
        cout << "[FAIL] " << testName << "\n";
    }
}

// ios::trunc erases whatever the file held, so each test starts empty.
void emptyTestFile() {
    ofstream f(TEST_FILE.c_str(), ios::out | ios::binary | ios::trunc);
    f.close();
}

// Appends raw bytes to the end of the test file (used to fake damage).
void appendJunkBytes(int howMany) {
    ofstream f(TEST_FILE.c_str(), ios::out | ios::binary | ios::app);
    for (int i = 0; i < howMany; ++i) f.put('#');
    f.close();
}

long fileSize() {
    ifstream f(TEST_FILE.c_str(), ios::in | ios::binary);
    f.seekg(0, ios::end);
    long size = f.tellg();
    f.close();
    return size;
}

bool sameValues(const Product& a, const Product& b) {
    return a.getProductCode() == b.getProductCode() && a.getName() == b.getName() &&
           a.getSupplierName() == b.getSupplierName() &&
           a.getSupplierPhone() == b.getSupplierPhone() &&
           a.getCurrentStock() == b.getCurrentStock() &&
           a.getReorderLevel() == b.getReorderLevel() &&
           a.getReorderQty() == b.getReorderQty() && a.getUnitCost() == b.getUnitCost();
}

// Builds a code like "P001" from a number.
string makeCode(int n) {
    string code = "P";
    char hundreds = '0' + (n / 100) % 10;
    char tens     = '0' + (n / 10) % 10;
    char ones     = '0' + n % 10;
    code.append(1, hundreds);
    code.append(1, tens);
    code.append(1, ones);
    return code;
}

Product rice() { return Product("RC-101", "Basmati Rice 5kg", "Sharma Traders",   "9876543210", 8,  10, 20, 410.00); }
Product oil()  { return Product("OIL-01", "Mustard Oil 1L",   "Punjab Oil Depot", "9812345678", 25, 12, 24, 165.50); }
Product sugar(){ return Product("SUG-01", "Sugar 1kg",        "Sharma Traders",   "9876543210", 5,  15, 30, 44.00); }

// Adds the three sample products in a store that lives only inside this
// function. When the function returns, the store is destroyed and the
// ProductFile destructor closes the file.
void addThreeAndClose() {
    ProductStore store(TEST_FILE);
    store.addProduct(rice());
    store.addProduct(oil());
    store.addProduct(sugar());
}

// ---------------------------------------------------------------------------
void testConstructors() {
    cout << "\n== Constructors ==\n";

    Product blank;                                         // default constructor
    check(!blank.isValid() && blank.getProductCode() == "" && blank.getCurrentStock() == 0,
          "C1 default constructor gives a blank, invalid product");

    Product p = rice();                                    // parameterised constructor
    check(p.isValid() && p.getProductCode() == "RC-101" && p.getUnitCost() == 410.00,
          "C2 parameterised constructor fills every field");

    Product lower("rc-101", "Rice", "Sharma", "9876543210", 1, 1, 1, 1.0);
    check(lower.getProductCode() == "RC-101", "C3 parameterised constructor still normalises the code");

    Product bad("RC-102", "Rice", "Sharma", "123", 1, 1, 1, 1.0);   // bad phone
    check(!bad.isValid(), "C4 parameterised constructor with a bad value gives an invalid product");

    Product copy(p);                                       // copy constructor
    check(sameValues(copy, p), "C5 copy constructor copies every field");

    copy.setCurrentStock(99);
    check(p.getCurrentStock() == 8 && copy.getCurrentStock() == 99,
          "C6 changing the copy does not change the original");

    ProductRecord r = p.toRecord();
    Product fromRecord(r);                                 // record constructor
    check(sameValues(fromRecord, p), "C7 product -> record -> product keeps every value");

    check(string(r.code) == "RC-101" && r.code[6] == '\0' && r.code[10] == '\0',
          "C8 record text is ended and padded with '\\0'");

    Product target;
    target.copyFrom(p);
    check(sameValues(target, p), "C9 copyFrom copies into an existing object");

    check(ProductFile::RECORD_SIZE == (int)sizeof(ProductRecord),
          "C10 every record has the same fixed size");
}

void testDestructorAndPersistence() {
    cout << "\n== Destructor and persistence (restart) ==\n";
    emptyTestFile();

    {
        ProductFile f(TEST_FILE);                          // constructor opens
        check(f.isOpen(), "D1 constructor opens the file");
        f.append(rice().toRecord());
    }                                                      // destructor closes here
    check(fileSize() == ProductFile::RECORD_SIZE,
          "D2 after the destructor closes the file, the record is on disk");

    emptyTestFile();
    addThreeAndClose();                                    // run 1
    ProductStore after(TEST_FILE);                         // run 2 ("restart")
    check(after.getCount() == 3 && after.getLoadedCount() == 3,
          "P1 three products survive a restart");

    const Product* p = after.findByCode("SUG-01");
    check(p != nullptr && sameValues(*p, sugar()), "P2 every field is the same after the restart");

    check(after.addProduct(rice()) == ADD_DUPLICATE,
          "P3 duplicate check still works after a restart");

    check(after.fileRecordCount() == 3 && after.fileSizeInBytes() == 3L * ProductFile::RECORD_SIZE,
          "P4 file size = number of records x record size");

    check(after.addProduct(Product("TEA-250", "Tea Leaves 250g", "Assam Tea House",
                                   "9123456780", 40, 10, 20, 120.00)) == ADD_OK &&
          after.fileRecordCount() == 4,
          "P5 a new product is appended as record 4");
}

void testSearchAndRead() {
    cout << "\n== Search with seekg and reading records ==\n";
    emptyTestFile();
    addThreeAndClose();
    ProductStore store(TEST_FILE);
    Product found;

    check(store.searchFile("RC-101", found) == 0 && found.getName() == "Basmati Rice 5kg",
          "S1 search finds the first record (index 0)");
    check(store.searchFile("SUG-01", found) == 2 && found.getCurrentStock() == 5,
          "S2 search finds the last record (index 2)");
    check(store.searchFile("  oil-01 ", found) == 1, "S3 search ignores case and spaces");
    check(store.searchFile("XYZ-999", found) == -1, "S4 search for a missing code returns -1");

    check(store.readRecord(1, found) && found.getProductCode() == "OIL-01",
          "S5 readRecord jumps straight to record 1");
    check(!store.readRecord(-1, found), "S6 record -1 is refused");
    check(!store.readRecord(3, found), "S7 record past the end is refused");
}

void testUpdate() {
    cout << "\n== Update with seekp ==\n";
    emptyTestFile();
    addThreeAndClose();

    {
        ProductStore store(TEST_FILE);
        Product changed(*store.findByCode("OIL-01"));      // copy constructor
        changed.setCurrentStock(3);
        check(store.updateProduct(changed) == UPDATE_OK, "U1 update a product's stock");
        check(store.findByCode("OIL-01")->getCurrentStock() == 3, "U2 memory shows the new stock");
        check(store.fileRecordCount() == 3, "U3 update rewrites in place, the file does not grow");

        Product unknown("NEW-01", "New", "Someone", "9000000000", 1, 1, 1, 1.0);
        check(store.updateProduct(unknown) == UPDATE_NOT_FOUND, "U4 updating an unknown code is refused");

        Product invalid;                                   // blank = invalid
        check(store.updateProduct(invalid) == UPDATE_INVALID, "U5 updating with invalid values is refused");

        Product negative(*store.findByCode("RC-101"));
        check(!negative.setCurrentStock(-5) && negative.getCurrentStock() == 8,
              "U6 a negative stock is refused by the setter");
    }

    ProductStore restarted(TEST_FILE);
    Product found;
    check(restarted.findByCode("OIL-01")->getCurrentStock() == 3 &&
          restarted.searchFile("OIL-01", found) == 1 && found.getCurrentStock() == 3,
          "U7 the updated stock is still there after a restart");
    check(restarted.findByCode("RC-101")->getCurrentStock() == 8 &&
          restarted.findByCode("SUG-01")->getCurrentStock() == 5,
          "U8 the other records were not touched");
}

void testDamagedAndMissingFiles() {
    cout << "\n== Damaged and missing files ==\n";

    emptyTestFile();
    addThreeAndClose();
    appendJunkBytes(10);                                   // half-written record
    {
        ProductStore store(TEST_FILE);
        check(store.getCount() == 3 && store.fileRecordCount() == 3,
              "F1 extra bytes at the end are ignored (only whole records count)");
        check(store.addProduct(Product("TEA-250", "Tea Leaves 250g", "Assam Tea House",
                                       "9123456780", 40, 10, 20, 120.00)) == ADD_OK,
              "F2 a new product overwrites the junk bytes");
    }
    check(fileSize() == 4L * ProductFile::RECORD_SIZE, "F3 file is clean again: exactly 4 records");

    // Write a record with a bad phone straight into the file.
    emptyTestFile();
    {
        ProductFile f(TEST_FILE);
        f.append(rice().toRecord());
        ProductRecord bad = oil().toRecord();
        copyToCharArray("12AB", bad.supplierPhone, Product::PHONE_LEN + 1);
        f.append(bad);
        f.append(rice().toRecord());                       // duplicate code
        f.append(sugar().toRecord());
    }
    ProductStore checked(TEST_FILE);
    check(checked.getCount() == 2 && checked.getSkippedCount() == 2,
          "F4 damaged and duplicate records are skipped on load");
    check(checked.findByCode("SUG-01") != nullptr, "F5 good records after a bad one still load");

    Product changed(*checked.findByCode("SUG-01"));
    changed.setCurrentStock(50);
    checked.updateProduct(changed);
    ProductRecord raw;
    ProductFile direct(TEST_FILE);
    check(direct.readAt(3, raw) && raw.currentStock == 50,
          "F6 update writes to the right record number even after skipped records");

    ProductStore missing("no_such_folder/products.dat");
    check(!missing.isFileOpen() && missing.getCount() == 0,
          "F7 a file that cannot be opened is reported");
    check(missing.addProduct(rice()) == ADD_FILE_ERROR && missing.getCount() == 0,
          "F8 add fails cleanly when the file cannot be written");
}

void testBoundaries() {
    cout << "\n== Boundary cases ==\n";
    emptyTestFile();
    {
        ProductStore store(TEST_FILE);
        check(store.getCount() == 0 && store.fileRecordCount() == 0, "B1 empty file loads 0 products");
        Product found;
        check(store.searchFile("RC-101", found) == -1 && !store.readRecord(0, found),
              "B2 search and read on an empty file find nothing");

        int ok = 0;
        for (int i = 1; i <= Product::MAX_PRODUCTS; ++i)
            if (store.addProduct(Product(makeCode(i), "Item", "Supplier", "9000000000",
                                         10, 5, 10, 1.0)) == ADD_OK) ok++;
        check(ok == Product::MAX_PRODUCTS, "B3 50 products are saved");
        check(store.addProduct(Product("EXTRA-1", "Item", "Supplier", "9000000000",
                                       10, 5, 10, 1.0)) == ADD_FULL &&
              store.fileRecordCount() == Product::MAX_PRODUCTS,
              "B4 the 51st is refused and not written to the file");
    }

    ProductStore store(TEST_FILE);
    check(store.getCount() == Product::MAX_PRODUCTS, "B5 all 50 reload after a restart");

    Product first(*store.findByCode("P001"));
    first.setCurrentStock(0);
    Product last(*store.findByCode("P050"));
    last.setCurrentStock(Product::QTY_MAX);
    check(store.updateProduct(first) == UPDATE_OK && store.updateProduct(last) == UPDATE_OK,
          "B6 first and last records can be updated (stock 0 and 100000)");

    Product found;
    check(store.readRecord(0, found) && found.getCurrentStock() == 0 &&
          store.readRecord(49, found) && found.getCurrentStock() == Product::QTY_MAX,
          "B7 record 0 and record 49 hold the new values");
    check(store.searchFile("P050", found) == 49, "B8 search reaches the very last record");
}

int main() {
    testConstructors();
    testDestructorAndPersistence();
    testSearchAndRead();
    testUpdate();
    testDamagedAndMissingFiles();
    testBoundaries();

    emptyTestFile();
    cout << "\n----------------------------------\n";
    cout << "Passed: " << passed << "   Failed: " << failed << "\n";
    return failed == 0 ? 0 : 1;
}
