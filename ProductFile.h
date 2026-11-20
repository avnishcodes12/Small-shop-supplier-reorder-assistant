// ProductFile.h  (Phase 2)
// Looks after the binary data file. Every product is stored as one
// fixed-size ProductRecord, one after another:
//
//   byte 0                 record 0
//   byte 1 x RECORD_SIZE   record 1
//   byte 2 x RECORD_SIZE   record 2   ...
//
// Because every record has the same size, record n can be reached directly
// with seekg (to read) or seekp (to write) at byte n * RECORD_SIZE.

#ifndef PRODUCT_FILE_H
#define PRODUCT_FILE_H

#include "Product.h"
#include <fstream>
#include <string>

class ProductFile {
private:
    std::fstream file;     // one stream used for both reading and writing
    std::string path;      // where the file lives, e.g. "data/products.dat"

public:
    // Size of one record in bytes, the same for every record.
    static const int RECORD_SIZE = sizeof(ProductRecord);

    // CONSTRUCTOR WITH A DEFAULT ARGUMENT: ProductFile f; opens the normal
    // data file, ProductFile f("tests/x.dat"); opens another one.
    // It opens the file (creating it if it does not exist yet).
    ProductFile(const std::string& path = "data/products.dat");

    // DESTRUCTOR: runs automatically when the object goes out of scope and
    // closes the file, so everything written is saved to disk.
    ~ProductFile();

    bool isOpen() const { return file.is_open(); }   // inline
    std::string getPath() const { return path; }     // inline

    int  recordCount();          // how many whole records the file holds
    long sizeInBytes();          // file size in bytes

    // RANDOM ACCESS
    bool readAt(int index, ProductRecord& record);          // seekg + read
    bool writeAt(int index, const ProductRecord& record);   // seekp + write
    int  append(const ProductRecord& record);    // write at the end, returns its index or -1
    int  findByCode(const std::string& code, ProductRecord& record);  // seekg search, index or -1

    // SEQUENTIAL ACCESS (used when the program starts)
    void rewind();                               // go back to byte 0
    bool readNext(ProductRecord& record);        // read the next record in order
};

#endif
