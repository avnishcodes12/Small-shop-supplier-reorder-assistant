# Data Dictionary: Small Shop Supplier Reorder Assistant (Phases 1 and 2)

**Main entity:** `Product`  **Unique key:** `productCode`
**Storage:** in memory in `ProductStore::items[50]` (an array of objects), and from Phase 2 also on disk in the binary file `data/products.dat` (section 5)

## 1. Entity: Product

| # | Field | C++ type | Size / range | Required | Validation rule | Example |
|---|---|---|---|---|---|---|
| 1 | `productCode` | `std::string` | 3 to 10 chars | Yes, **primary key** | Letters A to Z, digits 0 to 9 or `-`; first character must be a letter or digit; spaces trimmed; stored in UPPERCASE; must not already exist | `RC-101` |
| 2 | `name` | `std::string` | 1 to 40 chars | Yes | Printable characters only, not blank after trimming | `Basmati Rice 5kg` |
| 3 | `supplierName` | `std::string` | 1 to 40 chars | Yes | Same rule as `name` | `Sharma Traders` |
| 4 | `supplierPhone` | `std::string` | exactly 10 chars | Yes | Digits only. Stored as a string, not an int: a phone number is never used in arithmetic, and 10 digits can exceed the range of `int` | `9876543210` |
| 5 | `currentStock` | `int` | 0 to 100000 | Yes | Whole number; no letters, decimals or negatives | `8` |
| 6 | `reorderLevel` | `int` | 0 to 100000 | Yes | Whole number. The product needs a reorder when `currentStock <= reorderLevel` | `10` |
| 7 | `reorderQty` | `int` | 1 to 100000 | Yes | Whole number, at least 1 (ordering 0 units makes no sense) | `20` |
| 8 | `unitCost` | `double` | 0.01 to 1000000.00 | Yes | Greater than 0, at most 2 decimal places, no minus sign | `410.00` |

### Class-level (static) members
| Member | Type | Value / meaning |
|---|---|---|
| `totalAdded` | `static int` (private) | Number of products successfully added. One copy is shared by all objects; only `Product` and its friend `ProductStore` can change it |
| `MAX_PRODUCTS` | `static const int` | 50, the capacity of the array |
| `CODE_MIN_LEN`, `CODE_MAX_LEN` | `static const int` | 3 and 10 |
| `TEXT_MAX_LEN` | `static const int` | 40 |
| `PHONE_LEN` | `static const int` | 10 |
| `QTY_MAX` | `static const int` | 100000 |

### Derived values (calculated, not stored)
| Name | Formula | Meaning |
|---|---|---|
| `needsReorder()` | `currentStock <= reorderLevel` | The product should be reordered now |
| `reorderCost()` | `reorderQty × unitCost` | Estimated cost of one reorder |

## 2. Container: ProductStore

| Field | C++ type | Meaning |
|---|---|---|
| `items` | `Product[50]` | Array of objects holding the products. Slots `0 … count-1` are in use |
| `count` | `int` (0 to 50) | Number of products stored; also the index of the next free slot |

## 3. Enumeration: AddResult

| Value | Meaning | Message shown |
|---|---|---|
| `ADD_OK` | Stored successfully | "Product added successfully." |
| `ADD_INVALID` | A field is missing or invalid | "Error: product has invalid or missing fields." |
| `ADD_DUPLICATE` | `productCode` already exists | "Error: a product with this code already exists." |
| `ADD_FULL` | 50 products already stored | "Error: store is full (maximum 50 products)." |

## 4. Key rules
1. **Uniqueness:** no two products share a `productCode`. The check happens twice: once as soon as the code is typed, and again inside `addProduct()`.
2. **Case-insensitive key:** codes are trimmed and converted to uppercase before they are stored or compared, so `rc-101`, `RC-101` and ` Rc-101 ` all mean the same product.
3. **No partial records:** a product is copied into the array only after every field is valid. A blank entry cancels the whole add.

## 5. Phase 2: the data file `data/products.dat`

| Property | Value |
|---|---|
| File type | Binary (`ios::binary`), opened with `ios::in | ios::out` |
| Record type | `struct ProductRecord` |
| Record size | 128 bytes, the same for every record (`ProductFile::RECORD_SIZE`) |
| Record n starts at | byte `n * 128` (record 0 at byte 0) |
| Number of records | file size / 128 (extra bytes at the end are ignored) |
| Order | the order products were added; an update rewrites a record in its place |

### Record layout (`ProductRecord`)
| Bytes | Field | C++ type | Notes |
|---|---|---|---|
| 0 to 10 | `code` | `char[11]` | up to 10 characters + `'\0'`, rest padded with `'\0'` |
| 11 to 51 | `name` | `char[41]` | up to 40 characters + `'\0'` |
| 52 to 92 | `supplierName` | `char[41]` | up to 40 characters + `'\0'` |
| 93 to 103 | `supplierPhone` | `char[11]` | 10 digits + `'\0'` |
| 104 to 107 | `currentStock` | `int` | 4 bytes |
| 108 to 111 | `reorderLevel` | `int` | 4 bytes |
| 112 to 115 | `reorderQty` | `int` | 4 bytes |
| 116 to 119 | (padding) | | added by the compiler so the `double` starts at a multiple of 8 |
| 120 to 127 | `unitCost` | `double` | 8 bytes |

Byte positions are for g++/clang on a 64-bit system; another compiler could
add different padding, but `sizeof(ProductRecord)` is always used, so the
program stays correct on the compiler that wrote the file.

### Store bookkeeping (Phase 2)
| Member | Type | Meaning |
|---|---|---|
| `recordNo[50]` | `int` array | file record number of each slot in `items`, so an update knows where to `seekp` even if a damaged record was skipped on load |
| `loadedCount` | `int` | products read from the file when the program started |
| `skippedCount` | `int` | damaged, partial or duplicate records ignored on load |
| `file` | `ProductFile` | the open data file (closed by its destructor) |

### Fields an update may change
`currentStock`, `reorderLevel`, `reorderQty`, `unitCost`, `supplierPhone`.
The key `productCode` can never change, because it is how the record is found.
