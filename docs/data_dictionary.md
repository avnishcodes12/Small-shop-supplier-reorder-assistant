# Data Dictionary: Small Shop Supplier Reorder Assistant (Phase 1)

**Main entity:** `Product`  **Unique key:** `productCode`
**Storage:** in memory only, in `ProductStore::items[50]` (an array of objects)

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
