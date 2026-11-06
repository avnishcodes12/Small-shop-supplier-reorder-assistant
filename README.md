# Small Shop Supplier Reorder Assistant

**Course:** CAP455 · **Assessment:** CA1 · **Roll no.:** A17
**Student:** Lavnish Kumar Singh, School of Computer Science and Engineering, LPU

A menu-driven C++ console program for a small shop owner. It keeps a list
of the products the shop sells, along with the supplier for each one. It
tells the owner which products have fallen to their reorder level, how
much to order, the estimated cost and which supplier to call.

## Repository layout

```
/docs    proposal, data dictionary, UML class diagram, flowchart, design notes
/src     source code (Product, ProductStore, Utils, main)
/data    sample data and scripted input files for the demo
/tests   automated test program, manual test cases, recorded outputs
README.md
```

## Build and run

Any standard C++ compiler works (g++, clang++, MinGW, Code::Blocks, Dev-C++).
The code uses C++11 (only for `nullptr`).

**Main program**
```
g++ -std=c++11 -Wall -o reorder src/main.cpp src/Product.cpp src/ProductStore.cpp src/Utils.cpp
./reorder            (Windows: reorder.exe)
```

**Automated tests**
```
g++ -std=c++11 -Wall -Isrc -o test_phase1 tests/test_phase1.cpp src/Product.cpp src/ProductStore.cpp src/Utils.cpp
./test_phase1
```
Expected last line: `Passed: 57   Failed: 0`

**Scripted demo with sample data** (input redirection; the program itself
does no file handling)
```
./reorder < data/demo_normal_input.txt
./reorder < data/demo_invalid_input.txt
./reorder < data/demo_boundary_input.txt
```

With a Makefile-capable system you can also run `make`, `make test` or `make demo`.

## Features by phase

### Phase 1 (CA1): in-memory Create and Read
| Menu | Feature |
|---|---|
| 1 | **Add product** with field-by-field validation; duplicate product codes rejected straight away |
| 2 | **View all products**, with products that need reordering marked `*REORDER*` |
| 3 | **Search by product code** (case-insensitive, ignores extra spaces) |
| 4 | **Reorder list**: products with stock ≤ reorder level, order quantity, estimated cost, supplier to call, and the total |
| 5 | **Summary**: products stored / capacity, total added (static counter), count needing reorder |
| 6 | **Load demo products**: 5 ready-made products. Running it twice shows the duplicate check |

Rules enforced:
- **Unique key:** `productCode` must be 3 to 10 characters (letters, digits or `-`) and is stored in uppercase, so `rc-101` and `RC-101` count as the same code.
- **Name and supplier name:** 1 to 40 printable characters.
- **Supplier phone:** exactly 10 digits.
- **Stock and reorder level:** 0 to 100000.
- **Reorder quantity:** 1 to 100000.
- **Unit cost:** more than 0 and at most 1000000, with up to 2 decimals.
- **Capacity:** at most 50 products.
- **Cancel:** leaving any field blank cancels the add.

C++ concepts used (Units I and II only):
- class and objects
- static data member and static member functions
- inline functions
- friend function (`displayProduct`) and friend class (`ProductStore`)
- `this` pointer
- array of objects
- pointer to objects, pointer arithmetic and `nullptr`
- `std::string` and its modifiers
- `enum`
- reference variables and call by reference
- user-defined functions
- I/O stream formatting

### Phase 2 onwards
To be added as the next phases are released.

## Suggested commit history
Commit as you build rather than all at once, for example:
1. `Set up repo layout and README`
2. `Add Product entity class with validation`
3. `Add ProductStore with array of objects and duplicate-key check`
4. `Add menu-driven main program`
5. `Add automated tests and sample data`
6. `Add data dictionary, UML and flowchart`
