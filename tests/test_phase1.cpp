// test_phase1.cpp
// Automated tests for Phase 1: normal, invalid and boundary cases.
//
// Build & run (from the project root):
//   g++ -std=c++11 -Wall -Isrc -o test_phase1 tests/test_phase1.cpp src/Product.cpp src/ProductStore.cpp src/Utils.cpp
//   ./test_phase1

#include <iostream>
#include <string>
#include "Product.h"
#include "ProductStore.h"
#include "Utils.h"

using namespace std;

// Global counters, updated by every call of check()
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

Product makeValid(const string& code) {
    Product p;
    p.setAll(code, "Test Item", "Test Supplier", "9876543210", 10, 5, 20, 99.50);
    return p;
}

// Builds a code like "P001", "P002" ... from a number (uses string modifiers).
string makeCode(int n) {
    string digits = "";
    char hundreds = '0' + (n / 100) % 10;
    char tens     = '0' + (n / 10) % 10;
    char ones     = '0' + n % 10;
    digits.append(1, hundreds);
    digits.append(1, tens);
    digits.append(1, ones);
    digits.insert(0, "P");
    return digits;
}

void testNormalCases() {
    cout << "\n== Normal cases ==\n";
    ProductStore store;

    check(store.addProduct(makeValid("RC-101")) == ADD_OK, "N1 add a valid product");
    check(store.getCount() == 1, "N2 count becomes 1 after one add");

    const Product* p = store.findByCode("RC-101");
    check(p != nullptr, "N3 find an existing product by code");
    check(p != nullptr && p->getName() == "Test Item", "N4 found product has correct name");

    check(store.findByCode("rc-101") != nullptr, "N5 search is case-insensitive (rc-101)");
    check(store.findByCode("  RC-101  ") != nullptr, "N6 search ignores surrounding spaces");

    Product low;
    low.setAll("SUG-01", "Sugar", "Sharma", "9876543210", 5, 15, 30, 44.0);
    check(low.needsReorder(), "N7 stock 5 <= level 15 needs reorder");
    check(low.reorderCost() == 30 * 44.0, "N8 reorder cost = qty x unit cost");

    Product ok;
    ok.setAll("OIL-01", "Oil", "Depot", "9812345678", 25, 12, 24, 165.5);
    check(!ok.needsReorder(), "N9 stock 25 > level 12 does not need reorder");

    Product a = makeValid("ABC");
    Product b = makeValid("abc");
    check(a.hasSameKey(b), "N10 hasSameKey true for ABC vs abc");
}

void testInvalidCases() {
    cout << "\n== Invalid cases ==\n";
    ProductStore store;
    store.addProduct(makeValid("RC-101"));

    check(store.addProduct(makeValid("RC-101")) == ADD_DUPLICATE, "I1 exact duplicate code rejected");
    check(store.addProduct(makeValid("rc-101")) == ADD_DUPLICATE, "I2 duplicate in lowercase rejected");
    check(store.getCount() == 1, "I3 count unchanged after rejected duplicates");

    Product blank;
    check(store.addProduct(blank) == ADD_INVALID, "I4 blank product rejected");
    check(store.findByCode("XYZ-999") == nullptr, "I5 search for missing code returns nullptr");

    check(!Product::isValidCode(Product::normalizeCode("")), "I6 empty code invalid");
    check(!Product::isValidCode("A B1"), "I7 code with space invalid");
    check(!Product::isValidCode("RC_101"), "I8 code with underscore invalid");
    check(!Product::isValidCode("-RC1"), "I9 code starting with '-' invalid");

    check(!Product::isValidText("   "), "I10 blank name invalid");
    check(!Product::isValidText("Rice\t5kg"), "I11 name with a tab character invalid");

    check(!Product::isValidPhone("98765"), "I12 short phone invalid");
    check(!Product::isValidPhone("98765abcde"), "I13 phone with letters invalid");

    Product p;
    check(!p.setCurrentStock(-1), "I14 negative stock rejected");
    check(!p.setReorderQty(0), "I15 reorder quantity 0 rejected");
    check(!p.setUnitCost(0.0), "I16 unit cost 0 rejected");
    check(!p.setUnitCost(-5.0), "I17 negative unit cost rejected");

    int n = 0;
    check(!parseInt("12abc", 0, 100, n), "I18 parseInt rejects 12abc");
    check(!parseInt("abc", 0, 100, n), "I19 parseInt rejects abc");
    check(!parseInt("", 0, 100, n), "I20 parseInt rejects empty input");
    double d = 0;
    check(!parseMoney("4.555", d), "I21 parseMoney rejects 3 decimals");
    check(!parseMoney("-5", d), "I22 parseMoney rejects negative");
    check(!parseMoney("1.2.3", d), "I23 parseMoney rejects two dots");

    Product bad;
    bool allOk = bad.setAll("RC-200", "Rice", "Sharma", "123", 10, 5, 20, 50.0);   // bad phone
    check(!allOk && !bad.isValid(), "I24 setAll with bad phone gives invalid object");
    check(store.addProduct(bad) == ADD_INVALID, "I25 store rejects invalid object");
}

void testBoundaryCases() {
    cout << "\n== Boundary cases ==\n";

    check(Product::isValidCode("ABC"), "B1 code length 3 (minimum) valid");
    check(!Product::isValidCode("AB"), "B2 code length 2 invalid");
    check(Product::isValidCode("ABCDEFGHIJ"), "B3 code length 10 (maximum) valid");
    check(!Product::isValidCode("ABCDEFGHIJK"), "B4 code length 11 invalid");

    check(Product::isValidText(string(40, 'x')), "B5 name of 40 characters valid");
    check(!Product::isValidText(string(41, 'x')), "B6 name of 41 characters invalid");

    Product p;
    check(p.setCurrentStock(0), "B7 stock 0 valid");
    check(p.setCurrentStock(Product::QTY_MAX), "B8 stock 100000 valid");
    check(!p.setCurrentStock(Product::QTY_MAX + 1), "B9 stock 100001 invalid");
    check(p.setReorderQty(1), "B10 reorder quantity 1 valid");

    Product equal;
    equal.setAll("EQ-1", "Equal", "Sup", "9876543210", 10, 10, 5, 10.0);
    check(equal.needsReorder(), "B11 stock equal to reorder level needs reorder");

    double d = 0;
    check(parseMoney("0.01", d) && Product::isValidCost(d), "B12 smallest cost 0.01 valid");
    check(parseMoney("4.5", d) && d == 4.5, "B13 4.5 parsed as 4.50");

    int n = 0;
    check(parseInt("0", 0, 6, n) && n == 0, "B14 menu choice 0 (lowest) valid");
    check(parseInt("6", 0, 6, n) && n == 6, "B15 menu choice 6 (highest) valid");
    check(!parseInt("7", 0, 6, n), "B16 menu choice 7 invalid");

    // Fill the store to exactly 50 products, then try a 51st.
    ProductStore store;
    int addedOk = 0;
    for (int i = 1; i <= Product::MAX_PRODUCTS; ++i)
        if (store.addProduct(makeValid(makeCode(i))) == ADD_OK) addedOk++;
    check(addedOk == Product::MAX_PRODUCTS, "B17 exactly 50 products can be added");
    check(store.isFull(), "B18 store reports full at 50");
    check(store.addProduct(makeValid("EXTRA-1")) == ADD_FULL, "B19 51st product rejected as FULL");
    check(store.getCount() == Product::MAX_PRODUCTS, "B20 count stays 50");
    check(store.findByCode("P050") != nullptr, "B21 last slot (P050) can be found");
    check(store.findByCode("P001") != nullptr, "B22 first slot (P001) can be found");
}

int main() {
    testNormalCases();
    testInvalidCases();
    testBoundaryCases();

    cout << "\n----------------------------------\n";
    cout << "Passed: " << passed << "   Failed: " << failed << "\n";
    return failed == 0 ? 0 : 1;
}
