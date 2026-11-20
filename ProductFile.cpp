// ProductFile.cpp  (Phase 2) - binary file operations with fixed-size records.

#include "ProductFile.h"

// ---------------------------------------------------------------------------
// Constructor: open the file. The parameter has the same name as the member,
// and the initializer list path(path) copies the parameter into the member.
// ---------------------------------------------------------------------------
ProductFile::ProductFile(const std::string& path) : path(path) {
    // Step 1: make sure the file exists. Opening an ofstream in
    // ios::app mode creates the file if it is missing, and never erases
    // what is already there.
    std::ofstream create(path.c_str(), std::ios::out | std::ios::app | std::ios::binary);
    create.close();

    // Step 2: open it for both reading (ios::in) and writing (ios::out) in
    // binary mode. ios::in | ios::out keeps the existing data (no truncation).
    file.open(path.c_str(), std::ios::in | std::ios::out | std::ios::binary);
}

// Destructor: close the file if it is open.
ProductFile::~ProductFile() {
    if (file.is_open())
        file.close();
}

// ---------------------------------------------------------------------------
// Size information
// ---------------------------------------------------------------------------
long ProductFile::sizeInBytes() {
    if (!file.is_open()) return 0;
    file.clear();                       // reset any end-of-file flag first
    file.seekg(0, std::ios::end);       // jump to the end of the file
    long bytes = file.tellg();          // position at the end = file size
    return bytes;
}

// Only whole records count. If a crash left a few extra bytes at the end,
// the division ignores them.
int ProductFile::recordCount() {
    return sizeInBytes() / RECORD_SIZE;
}

// ---------------------------------------------------------------------------
// RANDOM ACCESS
// ---------------------------------------------------------------------------

// Read record number "index" straight away, without reading the ones before.
bool ProductFile::readAt(int index, ProductRecord& record) {
    if (!file.is_open() || index < 0 || index >= recordCount()) return false;
    file.clear();
    file.seekg((long)index * RECORD_SIZE, std::ios::beg);   // move the GET pointer
    file.read((char*)&record, RECORD_SIZE);                // read the raw bytes
    if (!file) {                        // fewer bytes than expected
        file.clear();
        return false;
    }
    return true;
}

// Write record number "index" in place. Writing at index == recordCount()
// adds a new record at the end. This is how UPDATE works: only that one
// record's bytes are replaced, nothing else in the file moves.
bool ProductFile::writeAt(int index, const ProductRecord& record) {
    if (!file.is_open() || index < 0 || index > recordCount()) return false;
    file.clear();
    file.seekp((long)index * RECORD_SIZE, std::ios::beg);   // move the PUT pointer
    file.write((const char*)&record, RECORD_SIZE);
    file.flush();                       // push the bytes to disk now
    if (!file) {
        file.clear();
        return false;
    }
    return true;
}

// Add a record after the last whole record. Returns its record number.
int ProductFile::append(const ProductRecord& record) {
    int index = recordCount();
    if (!writeAt(index, record)) return -1;
    return index;
}

// SEARCH WITH seekg: jump to each record in turn, read only its bytes and
// compare the code. Returns the record number, or -1 if not found.
int ProductFile::findByCode(const std::string& code, ProductRecord& record) {
    int n = recordCount();
    for (int i = 0; i < n; ++i) {
        if (readAt(i, record) && std::string(record.code) == code)
            return i;
    }
    return -1;
}

// ---------------------------------------------------------------------------
// SEQUENTIAL ACCESS
// ---------------------------------------------------------------------------
void ProductFile::rewind() {
    if (!file.is_open()) return;
    file.clear();
    file.seekg(0, std::ios::beg);
}

// Read the record at the current GET position and move on to the next one.
// Returns false at the end of the file (or if only part of a record is left).
bool ProductFile::readNext(ProductRecord& record) {
    if (!file.is_open()) return false;
    file.read((char*)&record, RECORD_SIZE);
    if (file.gcount() != RECORD_SIZE) {    // end of file, or a partial record
        file.clear();
        return false;
    }
    return true;
}
