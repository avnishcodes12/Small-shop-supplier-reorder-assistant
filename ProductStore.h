// ProductStore.h
// Keeps every product in memory in a fixed-size ARRAY OF OBJECTS (Phase 1)
// and, from Phase 2, also saves every product in a binary file so the data
// survives a restart.
//
// Phase 1: Create (add) and Read (find / list).
// Phase 2: load from file on start, write every add to the file, search the
//          file with seekg, and update a record in place with seekp.

#ifndef PRODUCT_STORE_H
#define PRODUCT_STORE_H

#include "Product.h"
#include "ProductFile.h"
#include <string>

// ENUMERATION: names for every possible result of an add attempt.
enum AddResult {
    ADD_OK,          // stored in memory and in the file
    ADD_INVALID,     // one or more fields failed validation
    ADD_DUPLICATE,   // productCode already exists
    ADD_FULL,        // array already holds MAX_PRODUCTS products
    ADD_FILE_ERROR   // Phase 2: the record could not be written to the file
};

// Phase 2: results of an update attempt.
enum UpdateResult {
    UPDATE_OK,
    UPDATE_INVALID,     // the new values break a rule
    UPDATE_NOT_FOUND,   // no product with that code
    UPDATE_FILE_ERROR   // the record could not be rewritten in the file
};

class ProductStore {
private:
    Product items[Product::MAX_PRODUCTS];   // ARRAY OF OBJECTS (50 slots)
    int recordNo[Product::MAX_PRODUCTS];    // file record number of each slot
    int count;                              // how many slots are in use
    int loadedCount;                        // products read from the file at start
    int skippedCount;                       // damaged or duplicate records skipped
    ProductFile file;                       // the binary data file

    void loadFromFile();                    // sequential read, called by the constructor

public:
    // CONSTRUCTOR WITH A DEFAULT ARGUMENT: opens the data file and loads it.
    ProductStore(const std::string& path = "data/products.dat");

    // ---- CREATE ----
    AddResult addProduct(const Product& p);

    // ---- READ (memory) ----
    // Returns a POINTER to the stored object, or nullptr when not found.
    const Product* findByCode(const std::string& rawCode) const;
    void displayAll() const;
    int  displayReorderList() const;        // returns how many need reorder
    int  countNeedingReorder() const;

    // ---- READ (file, Phase 2) ----
    // Searches the FILE with seekg. Fills "found" and returns the record
    // number, or -1 if no record has that code.
    int  searchFile(const std::string& rawCode, Product& found);
    // Reads one record directly by its number (random access).
    bool readRecord(int recordIndex, Product& found);

    // ---- UPDATE (Phase 2) ----
    // Rewrites the stored product that has the same code as "changed",
    // in memory and in the file (seekp). The code itself never changes.
    UpdateResult updateProduct(const Product& changed);

    // ---- Information (inline) ----
    int  getCount()        const { return count; }
    bool isFull()          const { return count >= Product::MAX_PRODUCTS; }
    bool isEmpty()         const { return count == 0; }
    int  getLoadedCount()  const { return loadedCount; }
    int  getSkippedCount() const { return skippedCount; }
    bool isFileOpen()      const { return file.isOpen(); }
    std::string getFilePath() const { return file.getPath(); }
    int  fileRecordCount()  { return file.recordCount(); }
    long fileSizeInBytes()  { return file.sizeInBytes(); }
};

// Turn a result into a message for the user.
std::string addResultMessage(AddResult result);
std::string updateResultMessage(UpdateResult result);

#endif
