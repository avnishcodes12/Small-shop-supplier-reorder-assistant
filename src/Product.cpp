// Product.cpp - implementation of the Product entity class.

#include "Product.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>

// A static data member is only DECLARED inside the class.
// It must be DEFINED (memory given to it) exactly once, here.
int Product::totalAdded = 0;

// ---------------------------------------------------------------------------
// setAll: fill every field through its own setter.
// If one value is rejected, that field stays empty/zero, the function
// returns false, isValid() stays false, and the store will refuse it.
// ---------------------------------------------------------------------------
bool Product::setAll(const std::string& productCode, const std::string& name,
                     const std::string& supplierName, const std::string& supplierPhone,
                     int currentStock, int reorderLevel, int reorderQty, double unitCost) {
    bool ok = true;
    if (!setProductCode(productCode))     ok = false;
    if (!setName(name))                   ok = false;
    if (!setSupplierName(supplierName))   ok = false;
    if (!setSupplierPhone(supplierPhone)) ok = false;
    if (!setCurrentStock(currentStock))   ok = false;
    if (!setReorderLevel(reorderLevel))   ok = false;
    if (!setReorderQty(reorderQty))       ok = false;
    if (!setUnitCost(unitCost))           ok = false;
    return ok;
}

// ---------------------------------------------------------------------------
// Static validators
// ---------------------------------------------------------------------------
std::string Product::normalizeCode(const std::string& rawCode) {
    std::string code = trim(rawCode);
    toUpperCase(code);              // modified in place (call by reference)
    return code;
}

// Code: 3-10 characters, letters/digits/hyphen, must start with a letter
// or digit. Checked on the NORMALIZED code.
bool Product::isValidCode(const std::string& code) {
    int len = code.length();
    if (len < CODE_MIN_LEN || len > CODE_MAX_LEN) return false;
    if (!isLetterChar(code[0]) && !isDigitChar(code[0])) return false;
    for (int i = 0; i < len; ++i) {
        char c = code[i];
        if (!isLetterChar(c) && !isDigitChar(c) && c != '-') return false;
    }
    return true;
}

// Name / supplier name: 1 to 40 characters after trimming, printable
// characters only (no tabs or other control characters).
bool Product::isValidText(const std::string& text) {
    std::string t = trim(text);
    int len = t.length();
    if (len == 0 || len > TEXT_MAX_LEN) return false;
    for (int i = 0; i < len; ++i) {
        char c = t[i];
        if (c < 32 || c > 126) return false;   // printable ASCII only
    }
    return true;
}

// Phone: exactly 10 digits.
bool Product::isValidPhone(const std::string& phone) {
    std::string p = trim(phone);
    int len = p.length();
    if (len != PHONE_LEN) return false;
    for (int i = 0; i < PHONE_LEN; ++i)
        if (!isDigitChar(p[i])) return false;
    return true;
}

bool Product::isValidQty(int qty) {
    return qty >= 0 && qty <= QTY_MAX;
}

bool Product::isValidCost(double cost) {
    return cost > 0.0 && cost <= 1000000.0;
}

// ---------------------------------------------------------------------------
// Setters. The parameter has the SAME name as the data member, so
// "this->name" means the object's member and plain "name" means the
// parameter. Without "this->", the parameter would hide the member.
// ---------------------------------------------------------------------------
bool Product::setProductCode(const std::string& productCode) {
    std::string code = normalizeCode(productCode);
    if (!isValidCode(code)) return false;
    this->productCode = code;
    return true;
}

bool Product::setName(const std::string& name) {
    if (!isValidText(name)) return false;
    this->name = trim(name);
    return true;
}

bool Product::setSupplierName(const std::string& supplierName) {
    if (!isValidText(supplierName)) return false;
    this->supplierName = trim(supplierName);
    return true;
}

bool Product::setSupplierPhone(const std::string& supplierPhone) {
    if (!isValidPhone(supplierPhone)) return false;
    this->supplierPhone = trim(supplierPhone);
    return true;
}

bool Product::setCurrentStock(int currentStock) {
    if (!isValidQty(currentStock)) return false;
    this->currentStock = currentStock;
    return true;
}

bool Product::setReorderLevel(int reorderLevel) {
    if (!isValidQty(reorderLevel)) return false;
    this->reorderLevel = reorderLevel;
    return true;
}

// Ordering 0 units makes no sense, so reorder quantity must be at least 1.
bool Product::setReorderQty(int reorderQty) {
    if (reorderQty < 1 || !isValidQty(reorderQty)) return false;
    this->reorderQty = reorderQty;
    return true;
}

bool Product::setUnitCost(double unitCost) {
    if (!isValidCost(unitCost)) return false;
    this->unitCost = unitCost;
    return true;
}

// ---------------------------------------------------------------------------
// Object-level checks
// ---------------------------------------------------------------------------
bool Product::isValid() const {
    return isValidCode(productCode) && isValidText(name) &&
           isValidText(supplierName) && isValidPhone(supplierPhone) &&
           isValidQty(currentStock) && isValidQty(reorderLevel) &&
           reorderQty >= 1 && isValidQty(reorderQty) && isValidCost(unitCost);
}

bool Product::hasSameKey(const Product& other) const {
    // "this" points to the object this function was called on.
    return this->productCode == other.productCode;
}

// ---------------------------------------------------------------------------
// Display
// ---------------------------------------------------------------------------
void Product::displayHeader() {
    std::cout << std::left
              << std::setw(11) << "CODE"
              << std::setw(22) << "PRODUCT"
              << std::right
              << std::setw(7)  << "STOCK"
              << std::setw(8)  << "LEVEL"
              << std::setw(8)  << "ORDQTY"
              << std::setw(11) << "COST(Rs)"
              << "  " << std::left
              << std::setw(18) << "SUPPLIER"
              << "PHONE" << "\n";
    std::cout << std::string(100, '-') << "\n";
}

// FRIEND FUNCTION: not a member of Product (no "Product::" before its name),
// yet it reads private members like p.unitCost, because Product declared it
// as a friend.
void displayProduct(const Product& p) {
    std::cout << std::left
              << std::setw(11) << p.productCode
              << std::setw(22) << p.name.substr(0, 21)
              << std::right
              << std::setw(7)  << p.currentStock
              << std::setw(8)  << p.reorderLevel
              << std::setw(8)  << p.reorderQty
              << std::setw(11) << std::fixed << std::setprecision(2) << p.unitCost
              << "  " << std::left
              << std::setw(18) << p.supplierName.substr(0, 17)
              << p.supplierPhone
              << (p.needsReorder() ? "  *REORDER*" : "")
              << "\n";
}
