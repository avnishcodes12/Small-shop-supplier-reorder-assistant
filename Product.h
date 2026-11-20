// Product.h
// Small Shop Supplier Reorder Assistant
//
// Product is the MAIN ENTITY CLASS. One Product object = one item the shop
// keeps on its shelf, plus the supplier it is reordered from.
// UNIQUE KEY: productCode (stored in UPPERCASE, so "rc-101" and "RC-101"
// count as the same key).
//
// Phase 1 (CA1): static, inline, friend, this, string, validation.
// Phase 2 (CA2): default, parameterised and copy constructors, initializer
//                lists, and conversion to/from a fixed-size file record.

#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>

class ProductStore;     // forward declaration, needed for the friend class line
struct ProductRecord;   // defined below the class (it uses Product's limits)

class Product {
private:
    std::string productCode;    // unique key, e.g. "RC-101"
    std::string name;           // e.g. "Basmati Rice 5kg"
    std::string supplierName;   // e.g. "Sharma Traders"
    std::string supplierPhone;  // exactly 10 digits
    int currentStock;           // units on the shelf now
    int reorderLevel;           // reorder when stock <= this level
    int reorderQty;             // units to order each time
    double unitCost;            // purchase price per unit (Rs.)

    // STATIC DATA MEMBER: only ONE copy exists, shared by all Product objects.
    // Counts how many products have been added in this run of the program.
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

    // ---- CONSTRUCTORS (Phase 2) ----
    // Default constructor: builds a blank product. "Product items[50]" calls
    // it 50 times, once for each element of the array.
    Product();

    // Parameterised constructor: builds a product from all its values.
    // Every value still goes through its setter, so the rules are checked.
    Product(const std::string& productCode, const std::string& name,
            const std::string& supplierName, const std::string& supplierPhone,
            int currentStock, int reorderLevel, int reorderQty, double unitCost);

    // Copy constructor: builds a new product as an exact copy of another.
    // Used when a product is passed or returned by value, and to keep a
    // "before" copy when a product is updated.
    Product(const Product& other);

    // Builds a product from a record read from the binary file.
    Product(const ProductRecord& record);

    // Copies every value from another product into THIS existing object.
    // Used instead of "a = b": the class has its own copy constructor, so
    // copying into an object that already exists is done by a normal,
    // clearly named member function.
    void copyFrom(const Product& other);

    // Turns this product into a fixed-size record that can be written to
    // the binary file in one go.
    ProductRecord toRecord() const;

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

// ---------------------------------------------------------------------------
// ProductRecord (Phase 2): the shape of ONE product on disk.
//
// A std::string keeps its letters somewhere else in memory and only stores
// the address inside the object. Writing a Product straight to a binary file
// would save that address, not the letters, and it would be useless after a
// restart. So each product is copied into this structure first: every text
// field is a fixed-size char array (maximum length + 1 for the '\0').
// Every record therefore has the same size, so record number n always starts
// at byte n * sizeof(ProductRecord). That is what makes seekg/seekp work.
// ---------------------------------------------------------------------------
struct ProductRecord {
    char code[Product::CODE_MAX_LEN + 1];          // 11 bytes
    char name[Product::TEXT_MAX_LEN + 1];          // 41 bytes
    char supplierName[Product::TEXT_MAX_LEN + 1];  // 41 bytes
    char supplierPhone[Product::PHONE_LEN + 1];    // 11 bytes
    int currentStock;
    int reorderLevel;
    int reorderQty;
    double unitCost;
};

#endif
