# Phase 1 Test Cases

Automated: `tests/test_phase1.cpp` (57 checks, all passing).
Manual and scripted: the three files in `/data`. Recorded outputs are in
`tests/output_normal.txt`, `tests/output_invalid.txt` and `tests/output_boundary.txt`.

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
