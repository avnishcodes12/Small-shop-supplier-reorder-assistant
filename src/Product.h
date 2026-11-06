// Product.h
// Small Shop Supplier Reorder Assistant - Phase 1 (CA1)
//
// Product is the MAIN ENTITY CLASS. One Product object = one item the shop
// keeps on its shelf, plus the supplier it is reordered from.
// UNIQUE KEY: productCode (stored in UPPERCASE, so "rc-101" and "RC-101"
// count as the same key).

#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>

class ProductStore;   // forward declaration, needed for the friend class line

class Product {
private:
    std::string productCode;    // unique key, e.g. "RC-101"
    std::string name;           // e.g. "Basmati Rice 5kg"
    std::string supplierName;   // e.g. "Sharma Traders"
    std::string supplierPhone;  // exactly 10 digits
    int currentStock = 0;       // units on the shelf now
    int reorderLevel = 0;       // reorder when stock <= this level
    int reorderQty = 0;         // units to order each time
    double unitCost = 0.0;      // purchase price per unit (Rs.)
    // Every field starts empty or zero, so a new Product object is "blank"
    // until its setters fill it. "Product items[50]" therefore gives 50
    // blank objects, and isValid() is false for each of them.

    // STATIC DATA MEMBER: only ONE copy exists, shared by all Product objects.
    // Counts how many products have been added to the store so far.
    // It is private so only Product and its friend ProductStore can change it.
    static int totalAdded;

public:
    // Class-wide limits (static const = one shared, unchangeable value).
    static const int MAX_PRODUCTS = 50;
    static const int CODE_MIN_LEN = 3;
    static const int CODE_MAX_LEN = 10;
    static const int TEXT_MAX_LEN = 40;
    static const int PHONE_LEN    = 10;
    static const int QTY_MAX      = 100000;

    // Fills every field at once by calling each setter in turn.
    // Returns true only if ALL values were valid.
    bool setAll(const std::string& productCode, const std::string& name,
                const std::string& supplierName, const std::string& supplierPhone,
                int currentStock, int reorderLevel, int reorderQty, double unitCost);

    // ---- Setters: validate first, then store using this-> ----
    bool setProductCode(const std::string& productCode);
    bool setName(const std::string& name);
    bool setSupplierName(const std::string& supplierName);
    bool setSupplierPhone(const std::string& supplierPhone);
    bool setCurrentStock(int currentStock);
    bool setReorderLevel(int reorderLevel);
    bool setReorderQty(int reorderQty);
    bool setUnitCost(double unitCost);

    // ---- INLINE getters: defined inside the class, so they are inline ----
    std::string getProductCode()   const { return productCode; }
    std::string getName()          const { return name; }
    std::string getSupplierName()  const { return supplierName; }
    std::string getSupplierPhone() const { return supplierPhone; }
    int    getCurrentStock() const { return currentStock; }
    int    getReorderLevel() const { return reorderLevel; }
    int    getReorderQty()   const { return reorderQty; }
    double getUnitCost()     const { return unitCost; }

    // Business rules: short and called often, so marked inline.
    inline bool needsReorder() const { return currentStock <= reorderLevel; }
    inline double reorderCost() const { return reorderQty * unitCost; }

    // True only when every field holds a valid value.
    bool isValid() const;

    // Compares this object's key with another object's key using "this".
    bool hasSameKey(const Product& other) const;

    // ---- STATIC MEMBER FUNCTIONS: called as Product::xyz(), no object needed ----
    static std::string normalizeCode(const std::string& rawCode);
    static bool isValidCode(const std::string& code);
    static bool isValidText(const std::string& text);
    static bool isValidPhone(const std::string& phone);
    static bool isValidQty(int qty);
    static bool isValidCost(double cost);
    static int  getTotalAdded() { return totalAdded; }
    static void displayHeader();

    // ---- FRIEND FUNCTION: not a member, but may read private data ----
    friend void displayProduct(const Product& p);

    // ---- FRIEND CLASS: ProductStore may access private members ----
    // Needed so the store can update the private static counter totalAdded
    // without making that counter public to the whole program.
    friend class ProductStore;
};

// Normal (non-member) declaration of the friend function, so any file
// that includes Product.h can call it.
void displayProduct(const Product& p);

#endif
