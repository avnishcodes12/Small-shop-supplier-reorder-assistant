// ProductStore.cpp - Create, Read and (Phase 2) Update, kept in memory and
// in the binary data file.

#include "ProductStore.h"
#include <iostream>
#include <iomanip>

// ---------------------------------------------------------------------------
// CONSTRUCTOR (Phase 2). The initializer list sets the counters and builds
// the ProductFile member with the given path, which opens the file.
// By the time the body runs, items[] already holds 50 blank products
// (the Product default constructor ran once for each element).
// ---------------------------------------------------------------------------
ProductStore::ProductStore(const std::string& path)
    : count(0), loadedCount(0), skippedCount(0), file(path) {
    loadFromFile();
}

// SEQUENTIAL READ: go back to the start of the file and read the records
// one after another until the end. Each good record goes into the array.
// A record is skipped (and counted) if it is damaged, has a code already
// loaded, or there is no room left.
void ProductStore::loadFromFile() {
    if (!file.isOpen()) return;

    ProductRecord record;
    int index = 0;                      // record number in the file
    file.rewind();
    while (file.readNext(record)) {
        Product p(record);              // constructor that takes a record
        if (!p.isValid() || findByCode(p.productCode) != nullptr || isFull()) {
            skippedCount++;
        } else {
            items[count].copyFrom(p);
            recordNo[count] = index;    // remember where it lives on disk
            count++;
            loadedCount++;
        }
        index++;
    }
}

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

    // Phase 2: write to the file FIRST. If that fails, nothing is added, so
    // memory and file never disagree.
    int newRecord = file.append(p.toRecord());
    if (newRecord < 0)
        return ADD_FILE_ERROR;

    items[count].copyFrom(p);   // copy the object into the next free slot
    recordNo[count] = newRecord;
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

// ---------------------------------------------------------------------------
// READ FROM THE FILE (Phase 2)
// ---------------------------------------------------------------------------
int ProductStore::searchFile(const std::string& rawCode, Product& found) {
    std::string key = Product::normalizeCode(rawCode);
    ProductRecord record;
    int index = file.findByCode(key, record);   // seekg to each record in turn
    if (index >= 0)
        found.copyFrom(Product(record));
    return index;
}

bool ProductStore::readRecord(int recordIndex, Product& found) {
    ProductRecord record;
    if (!file.readAt(recordIndex, record))      // one seekg, straight to the record
        return false;
    found.copyFrom(Product(record));
    return true;
}

// ---------------------------------------------------------------------------
// UPDATE (Phase 2): find the product's slot, rewrite its record in the file
// with seekp, then copy the new values into the array.
// ---------------------------------------------------------------------------
UpdateResult ProductStore::updateProduct(const Product& changed) {
    if (!changed.isValid())
        return UPDATE_INVALID;

    const Product* found = findByCode(changed.productCode);
    if (found == nullptr)
        return UPDATE_NOT_FOUND;

    // POINTER SUBTRACTION: (address of the found object) - (address of
    // items[0]) gives how many objects apart they are, which is its index.
    int slot = found - items;

    if (!file.writeAt(recordNo[slot], changed.toRecord()))
        return UPDATE_FILE_ERROR;

    items[slot].copyFrom(changed);
    return UPDATE_OK;
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
        case ADD_FILE_ERROR: return "Error: could not save the product to the data file.";
    }
    return "Unknown result.";
}

std::string updateResultMessage(UpdateResult result) {
    switch (result) {
        case UPDATE_OK:         return "Product updated and saved to the data file.";
        case UPDATE_INVALID:    return "Error: the new values are not valid. Nothing was changed.";
        case UPDATE_NOT_FOUND:  return "Error: no product with this code.";
        case UPDATE_FILE_ERROR: return "Error: could not write the change to the data file.";
    }
    return "Unknown result.";
}
