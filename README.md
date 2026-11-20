# Small Shop Supplier Reorder Assistant

**Course:** CAP455 · **Roll no.:** A17 · **Current phase:** CA2 (Persist and Search)
**Student:** Avnish Kumar Singh, School of Computer Applications, LPU

A menu-driven C++ console program for a small shop owner. It keeps a list of
the products the shop sells, along with the supplier for each one. It tells
the owner which products have fallen to their reorder level, how much to order,
the estimated cost and which supplier to call. From Phase 2 the products are
saved in a binary file, so they are still there when the program is run again.

## Repository layout

```
/docs    proposal (CA1), data dictionary, UML, flowcharts, design and viva notes
/src     source code
/data    products.dat (the binary data file) and scripted demo inputs
/tests   test programs, test-case table, recorded outputs and hex dump
README.md
```

## Build and run

Any standard C++ compiler works (g++, clang++, MinGW, Code::Blocks, Dev-C++).
Always **run the program from the project folder**, because the data file
path is `data/products.dat`.

**Main program**
```
g++ -std=c++11 -Wall -o reorder src/main.cpp src/Product.cpp src/ProductFile.cpp src/ProductStore.cpp src/Utils.cpp
./reorder            (Windows: reorder.exe)
```

**Automated tests**
```
g++ -std=c++11 -Wall -Isrc -o test_phase1 tests/test_phase1.cpp src/Product.cpp src/ProductFile.cpp src/ProductStore.cpp src/Utils.cpp
g++ -std=c++11 -Wall -Isrc -o test_phase2 tests/test_phase2.cpp src/Product.cpp src/ProductFile.cpp src/ProductStore.cpp src/Utils.cpp
./test_phase1        expected last line: Passed: 57   Failed: 0
./test_phase2        expected last line: Passed: 48   Failed: 0
```
The tests use their own files (`tests/test_phase1.dat`, `tests/test_phase2.dat`)
and never touch `data/products.dat`. With `make` you can also run `make`,
`make test` and `make clean`.

**Restart demo** (data surviving a restart)
```
./reorder < data/demo_phase2_run2_input.txt
```
`data/products.dat` already holds the 6 products saved by
`demo_phase2_run1_input.txt` (RC-101's stock was updated from 8 to 28 in that
run). Run 2 starts fresh, loads them, and shows the updated stock.

To start from an empty file, delete `data/products.dat`. The program creates
a new empty one the next time it runs.

## Features by phase

### Phase 1 (CA1): in-memory Create and Read
| Menu | Feature |
|---|---|
| 1 | **Add product** with field-by-field validation; duplicate product codes refused straight away |
| 2 | **View all products**; products that need reordering are marked `*REORDER*` |
| 3 | **Search by product code** (case-insensitive, extra spaces ignored) |
| 4 | **Reorder list**: products at or below their reorder level, quantity, estimated cost, supplier to call, total |
| 5 | **Summary** |
| 6 | **Load demo products**: 5 ready-made products; running it twice shows the duplicate check |

Concepts (Units I and II): class and objects, static data member and static
member functions, inline functions, friend function and friend class, `this`,
array of objects, pointers and pointer arithmetic, `nullptr`, `string` and its
modifiers, enum, references and call by value / reference / address.

### Phase 2 (CA2): persist and search
| Menu | Feature |
|---|---|
| (start) | All saved products are **loaded from `data/products.dat`** (sequential read) |
| 1, 6 | Every new product is **appended** to the file as a fixed-size 128-byte record |
| 3 | **Search** now reads the file with `seekg`, record by record, and shows the record number and byte position |
| 5 | Summary also shows the data file, record count, record size and file size |
| 7 | **Update a product** (stock, reorder level, reorder quantity, unit cost or supplier phone). Only that record is rewritten, in place, with `seekp`. Shows before and after. |
| 8 | **View a record by its number** using `seekg` (random access) |

Rules: the product code (the key) can never be changed by an update; every new
value goes through the same setters as Phase 1; damaged, partial or duplicate
records in the file are skipped on load and reported.

Concepts added (Unit III):
- default, parameterised and copy constructors
- initializer lists
- a constructor with a default argument
- a destructor that closes the file
- file opening modes (`in`, `out`, `app`, `binary`, `trunc`)
- binary `read` / `write` of a structure
- sequential access (`readNext`)
- random access (`seekg`, `seekp`, `tellg`)

## Suggested commits for Phase 2
1. `Add constructors, copy constructor and copyFrom to Product`
2. `Add ProductRecord struct and ProductFile with binary read/write`
3. `Load products from file on start and save every add`
4. `Add search with seekg and update with seekp`
5. `Add Phase 2 tests and restart demo evidence`
6. `Update UML, data dictionary and viva notes for Phase 2`
