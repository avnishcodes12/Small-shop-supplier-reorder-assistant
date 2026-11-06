// ProductStore.h
// Keeps every product in memory in a fixed-size ARRAY OF OBJECTS and
// provides the Phase 1 operations: Create (add) and Read (find / list).

#ifndef PRODUCT_STORE_H
#define PRODUCT_STORE_H

#include "Product.h"
#include <string>

// ENUMERATION: names for every possible result of an add attempt,
// instead of "magic numbers" like 0, 1, 2, 3.
enum AddResult {
    ADD_OK,          // stored successfully
    ADD_INVALID,     // one or more fields failed validation
    ADD_DUPLICATE,   // productCode already exists
    ADD_FULL         // array already holds MAX_PRODUCTS products
};

class ProductStore {
private:
    Product items[Product::MAX_PRODUCTS];   // ARRAY OF OBJECTS (50 slots)
    int count = 0;                          // how many slots are in use

public:

    // ---- CREATE ----
    // The product is passed by const reference: no copy on the way in, and
    // the caller's object cannot be changed. One copy is made only when it
    // is actually stored in the array.
    AddResult addProduct(const Product& p);

    // ---- READ ----
    // Returns a POINTER to the stored object, or nullptr when not found.
    const Product* findByCode(const std::string& rawCode) const;

    void displayAll() const;
    int  displayReorderList() const;        // returns how many need reorder
    int  countNeedingReorder() const;

    int  getCount()  const { return count; }   // inline
    bool isFull()    const { return count >= Product::MAX_PRODUCTS; }
    bool isEmpty()   const { return count == 0; }
};

// Turns an AddResult into a message for the user.
std::string addResultMessage(AddResult result);

#endif
