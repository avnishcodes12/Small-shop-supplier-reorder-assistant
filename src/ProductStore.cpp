// ProductStore.cpp - Create and Read operations on the array of products.

#include "ProductStore.h"
#include <iostream>
#include <iomanip>

// items[] starts as 50 blank Product objects (every field empty or zero),
// and count starts at 0, so no slot is in use yet.

// ---------------------------------------------------------------------------
// CREATE
// Order of checks: invalid -> duplicate -> full.
// ---------------------------------------------------------------------------
AddResult ProductStore::addProduct(const Product& p) {
    if (!p.isValid())
        return ADD_INVALID;

    // ProductStore is a friend class, so it can read p.productCode directly.
    if (findByCode(p.productCode) != nullptr)
        return ADD_DUPLICATE;

    if (isFull())
        return ADD_FULL;

    items[count] = p;           // copy the object into the next free slot
    count++;
    Product::totalAdded++;      // private static member, allowed as friend
    return ADD_OK;
}

// ---------------------------------------------------------------------------
// READ: linear search using a POINTER that walks through the array.
// ---------------------------------------------------------------------------
const Product* ProductStore::findByCode(const std::string& rawCode) const {
    std::string key = Product::normalizeCode(rawCode);   // "rc-101" -> "RC-101"

    const Product* ptr = items;          // array name = address of items[0]
    const Product* end = items + count;  // one past the last USED slot

    while (ptr < end) {
        if (ptr->getProductCode() == key)   // "->" accesses a member through a pointer
            return ptr;
        ptr++;                           // pointer arithmetic: move to the next object
    }
    return nullptr;                      // not found
}

void ProductStore::displayAll() const {
    if (isEmpty()) {
        std::cout << "No products added yet.\n";
        return;
    }
    Product::displayHeader();
    for (int i = 0; i < count; ++i)
        displayProduct(items[i]);        // friend function
    std::cout << "Total products: " << count << "\n";
}

int ProductStore::countNeedingReorder() const {
    int n = 0;
    for (int i = 0; i < count; ++i)
        if (items[i].needsReorder()) n++;
    return n;
}

int ProductStore::displayReorderList() const {
    int listed = 0;
    double totalCost = 0.0;

    for (int i = 0; i < count; ++i) {
        const Product* p = &items[i];    // pointer to an object in the array
        if (!p->needsReorder()) continue;

        if (listed == 0) {
            std::cout << std::left
                      << std::setw(11) << "CODE"
                      << std::setw(22) << "PRODUCT"
                      << std::right
                      << std::setw(7)  << "STOCK"
                      << std::setw(8)  << "LEVEL"
                      << std::setw(8)  << "ORDER"
                      << std::setw(18) << "EST.COST"
                      << "  " << std::left << "CALL SUPPLIER" << "\n";
            std::cout << std::string(100, '-') << "\n";
        }
        std::cout << std::left
                  << std::setw(11) << p->getProductCode()
                  << std::setw(22) << p->getName().substr(0, 21)
                  << std::right
                  << std::setw(7)  << p->getCurrentStock()
                  << std::setw(8)  << p->getReorderLevel()
                  << std::setw(8)  << p->getReorderQty()
                  << std::setw(18) << std::fixed << std::setprecision(2) << p->reorderCost()
                  << "  " << std::left
                  << p->getSupplierName() << " (" << p->getSupplierPhone() << ")\n";
        totalCost += p->reorderCost();
        listed++;
    }

    if (listed == 0)
        std::cout << "All products are above their reorder level. Nothing to order.\n";
    else
        std::cout << "Items to reorder: " << listed
                  << "   Estimated total cost: Rs. " << std::fixed
                  << std::setprecision(2) << totalCost << "\n";
    return listed;
}

std::string addResultMessage(AddResult result) {
    switch (result) {
        case ADD_OK:        return "Product added successfully.";
        case ADD_INVALID:   return "Error: product has invalid or missing fields.";
        case ADD_DUPLICATE: return "Error: a product with this code already exists.";
        case ADD_FULL:      return "Error: store is full (maximum 50 products).";
    }
    return "Unknown result.";
}
