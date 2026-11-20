# Test Cases

| Phase | Automated tests | Result |
|---|---|---|
| Phase 1 (CA1) | `tests/test_phase1.cpp` | 57 checks, all passing |
| Phase 2 (CA2) | `tests/test_phase2.cpp` | 48 checks, all passing |

# Phase 1 Test Cases

Automated: `tests/test_phase1.cpp` (57 checks, all passing; from Phase 2 it uses
its own test file `tests/test_phase1.dat`).
Scripted: `data/demo_phase1_*_input.txt`. Outputs recorded in Phase 1 are in
`tests/phase1_evidence/`.

## Normal cases
| ID | Input | Expected result | Status |
|---|---|---|---|
| N1 | Add RC-101, Basmati Rice 5kg, Sharma Traders, 9876543210, 8, 10, 20, 410 | "Product added successfully." | Pass |
| N2 | Add code typed as `oil-01` | Stored as `OIL-01` | Pass |
| N3 | View all (menu 2) after 3 adds | Table of 3 products; RC-101 and SUG-01 marked `*REORDER*` | Pass |
| N4 | Search `rc-101` | RC-101 found (case-insensitive) | Pass |
| N5 | Reorder list (menu 4) | RC-101 (Rs. 8200.00) and SUG-01 (Rs. 1320.00); total Rs. 9520.00 | Pass |
| N6 | Summary (menu 5) | Stored 3/50, total added 3, need reorder 2 | Pass |
| N7 | Load demo products (menu 6) | 5 products added; reorder total Rs. 11020.00 | Pass |

## Invalid cases
| ID | Input | Expected result | Status |
|---|---|---|---|
| I1 | Menu choice `9`, `abc` | "Invalid choice. Enter a number from 0 to 6." | Pass |
| I2 | Code `AB` / `RC 101` / `RC_101` / `-RC1` | "Invalid: code must be 3-10 characters..." and asked again | Pass |
| I3 | Code `rc-101` when RC-101 exists | "Duplicate: product RC-101 already exists." | Pass |
| I4 | Load demo products twice | Second run: every code rejected as duplicate | Pass |
| I5 | Name of 41 characters | "Invalid: use 1-40 printable characters." | Pass |
| I6 | Phone `98765`, `98765abcde` | "Invalid: phone must be exactly 10 digits." | Pass |
| I7 | Stock `abc`, `-5` | "Invalid: enter a whole number from 0 to 100000." | Pass |
| I8 | Reorder quantity `0` | "Invalid: enter a whole number from 1 to 100000." | Pass |
| I9 | Cost `0`, `-410`, `4.555` | "Invalid: enter an amount above 0..." | Pass |
| I10 | Blank line in any field | "Add cancelled." and nothing stored | Pass |
| I11 | Search `XYZ-999` | "No product found with code XYZ-999." | Pass |
| I12 | A blank `Product` object passed straight to `addProduct` | `ADD_INVALID` | Pass |

## Boundary cases
| ID | Input | Expected result | Status |
|---|---|---|---|
| B1 | View all / reorder list on an empty store | "No products added yet." / "Nothing to order." | Pass |
| B2 | Code `ABC` (3 characters, the minimum) | Accepted | Pass |
| B3 | Code `ABCDEFGHIJ` (10 characters, the maximum) | Accepted | Pass |
| B4 | Code `ABCDEFGHIJK` (11 characters) | Rejected | Pass |
| B5 | Name of exactly 40 characters | Accepted | Pass |
| B6 | Name of 41 characters | Rejected | Pass |
| B7 | Stock 0, level 0 | Accepted; needs reorder (0 ≤ 0) | Pass |
| B8 | Stock, level and quantity 100000; cost 1000000 | Accepted; table stays aligned | Pass |
| B9 | Stock 100001 | Rejected | Pass |
| B10 | Stock = reorder level (10 = 10) | Needs reorder | Pass |
| B11 | Cost `0.01` (the minimum) / `4.5` | Accepted; shown as 0.01 / 4.50 | Pass |
| B12 | Add 50 products, then a 51st | 50 accepted; 51st gets "store is full" | Pass |
| B13 | Search first (P001) and last (P050) slot | Both found | Pass |

# Phase 2 Test Cases (CA2: constructors, destructor, file persistence, search and update)

Automated: `tests/test_phase2.cpp` (48 checks, all passing).
Scripted sessions (input shown in the transcript):
- `data/demo_phase2_run1_input.txt` → `tests/output_phase2_run1.txt` (empty file, add, update, search, view record)
- `data/demo_phase2_run2_input.txt` → `tests/output_phase2_run2.txt` (**restart**: everything from run 1 is still there)
- `data/demo_phase2_invalid_input.txt` → `tests/output_phase2_invalid.txt` (wrong inputs in update, search and record view)
- `tests/products_dat_hexdump.txt`: the bytes of `data/products.dat`, showing 6 fixed-size 128-byte records

## Constructors and destructor
| ID | Test | Expected result | Status |
|---|---|---|---|
| C1 | `Product blank;` | Default constructor: empty code, stock 0, `isValid()` false | Pass |
| C2 | `Product("RC-101", ..., 410.00)` | Parameterised constructor fills every field | Pass |
| C3 | Parameterised constructor with code `rc-101` | Stored as `RC-101` (setters still run) | Pass |
| C4 | Parameterised constructor with phone `123` | Object is invalid | Pass |
| C5 | `Product copy(p);` | Copy constructor copies all 8 fields | Pass |
| C6 | Change the copy's stock to 99 | Original keeps 8 | Pass |
| C7 | Product → `toRecord()` → `Product(record)` | Same values come back | Pass |
| C8 | `toRecord()` text fields | Ended and padded with `'\0'` | Pass |
| C9 | `copyFrom()` | Copies into an existing object | Pass |
| C10 | `RECORD_SIZE` | Equals `sizeof(ProductRecord)` (128 bytes) | Pass |
| D1 | `ProductFile f(path);` | Constructor opens (creates) the file | Pass |
| D2 | Append one record, let `f` go out of scope | Destructor closes the file; file is 128 bytes | Pass |

## Persistence (restart) and reading records
| ID | Test | Expected result | Status |
|---|---|---|---|
| P1 | Add 3 products, destroy the store, build a new store on the same file | 3 products loaded | Pass |
| P2 | Compare SUG-01 after restart | Every field identical | Pass |
| P3 | Add RC-101 again after restart | `ADD_DUPLICATE` | Pass |
| P4 | File size after 3 records | 3 × 128 = 384 bytes | Pass |
| P5 | Add a 4th product | Appended as record 4 | Pass |
| S1 | `searchFile("RC-101")` | Record index 0 | Pass |
| S2 | `searchFile("SUG-01")` | Record index 2 (last) | Pass |
| S3 | `searchFile("  oil-01 ")` | Record index 1 | Pass |
| S4 | `searchFile("XYZ-999")` | -1 (not found) | Pass |
| S5 | `readRecord(1)` | OIL-01 (direct `seekg`) | Pass |
| S6, S7 | `readRecord(-1)`, `readRecord(3)` on 3 records | Refused | Pass |

## Update with seekp
| ID | Test | Expected result | Status |
|---|---|---|---|
| U1 | Change OIL-01 stock to 3 | `UPDATE_OK` | Pass |
| U2 | Memory after update | Stock 3 | Pass |
| U3 | File after update | Still 3 records (rewritten in place, not appended) | Pass |
| U4 | Update code NEW-01 (not stored) | `UPDATE_NOT_FOUND` | Pass |
| U5 | Update with a blank product | `UPDATE_INVALID` | Pass |
| U6 | `setCurrentStock(-5)` | Refused, value unchanged | Pass |
| U7 | Restart after update | OIL-01 stock is 3 in memory and in the file | Pass |
| U8 | Other records after update | Unchanged | Pass |

## Damaged and missing files
| ID | Test | Expected result | Status |
|---|---|---|---|
| F1 | 10 junk bytes after 3 records | Junk ignored, 3 products load | Pass |
| F2, F3 | Add a product after that | Junk overwritten, file is exactly 4 × 128 bytes | Pass |
| F4 | File with a bad-phone record and a duplicate record | Both skipped, `skippedCount` = 2 | Pass |
| F5 | Good record after the bad ones | Still loads | Pass |
| F6 | Update after skipped records | Writes to the correct record number (3) | Pass |
| F7, F8 | Path in a folder that does not exist | File reported as not open; add returns `ADD_FILE_ERROR` | Pass |

## Boundary cases
| ID | Test | Expected result | Status |
|---|---|---|---|
| B1, B2 | Empty file | 0 products; search and read find nothing | Pass |
| B3 | Save 50 products | All 50 written | Pass |
| B4 | 51st product | `ADD_FULL`, file still 50 records | Pass |
| B5 | Restart with 50 records | All 50 load | Pass |
| B6 | Update first record to stock 0 and last to 100000 | Both `UPDATE_OK` | Pass |
| B7 | Read record 0 and record 49 | New values | Pass |
| B8 | Search P050 | Record index 49 | Pass |

## Manual (menu) cases, Phase 2
| ID | Input | Expected result | Status |
|---|---|---|---|
| M1 | Run 1: menu 7, `rc-101`, field 1, stock `28` | Before 8 / After 28 shown, "saved to the data file" | Pass |
| M2 | Run 1: menu 3, `sug-01` | "record 4, starting at byte 384" | Pass |
| M3 | Run 2 (restart): menu 2 | 6 products, RC-101 stock 28 | Pass |
| M4 | Run 2: menu 6 | All 5 demo products refused as duplicates | Pass |
| M5 | Menu 7 with `xyz-999` | "No product found" | Pass |
| M6 | Field choice `9`, then stock `-5`, `abc`, `100001` | Each refused, asked again | Pass |
| M7 | Menu 8 with record `0`, `7` (only 6 exist), `abc` | Each refused, asked again | Pass |
| M8 | Menu 7, field choice `0` | "Update cancelled", nothing written | Pass |
