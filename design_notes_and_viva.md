# Design Notes and Viva Preparation: Phase 1

These notes explain **why** each design decision was made, so every part of
the code can be explained and defended.

---

## 1. Where each required concept is used

| Concept (syllabus unit) | Where in the code | What it does there |
|---|---|---|
| Class and objects (I) | `Product`, `ProductStore` | Product = one shelf item; ProductStore = the shop's list |
| Static data member (I) | `Product::totalAdded` in `Product.h`, defined in `Product.cpp` | One counter shared by all products |
| Static const members (I) | `Product::MAX_PRODUCTS`, `CODE_MAX_LEN`, … | Limits that belong to the class, not to any one object |
| Static member functions (I) | `Product::isValidCode()`, `isValidPhone()`, `normalizeCode()`, … | Validate input **before** any object exists |
| Inline functions (I) | Getters, `needsReorder()`, `reorderCost()`, `getCount()`, `isDigitChar()` | Small, frequently called functions |
| Friend function (I) | `displayProduct(const Product&)` | Prints private fields without being a member |
| Friend class (I) | `friend class ProductStore;` in `Product.h` | Lets the store update the private static counter |
| Enumeration (I) | `enum AddResult` in `ProductStore.h` | Named results instead of magic numbers 0/1/2/3 |
| Reference variables, call by reference (I) | `toUpperCase(std::string&)`, `parseInt(..., int& value)`, `addProductMenu(ProductStore&)` | Function changes the caller's variable |
| Call by const reference (I) | `addProduct(const Product&)`, `isValidCode(const std::string&)` | No copy is made and the argument can't be changed |
| Call by value (I) | `isValidQty(int qty)`, `setCurrentStock(int)` | Small values are cheap to copy |
| User-defined functions (I) | `trim`, `parseInt`, `parseMoney`, menu functions | Break the program into small, testable parts |
| I/O streams (I) | `cin`, `cout`, `getline`, `setw`, `setprecision`, `fixed` | Menu, input and aligned tables |
| `this` pointer (II) | Every setter: `this->name = trim(name);` and `hasSameKey()` | Tells the member apart from a parameter with the same name |
| Array of objects (II) | `Product items[50]` in `ProductStore`; `Product demo[5]` in `main.cpp` | Stores all products |
| Pointer to object (II) | `const Product* findByCode()`; `const Product* p = &items[i];` | Returns or points to an object inside the array |
| Pointer arithmetic (II) | `findByCode()`: `ptr = items; end = items + count; ptr++` | Walks through the array |
| Null pointer (II) | `return nullptr;` when not found; `if (found == nullptr)` | Represents "no product" safely |
| `->` and `*` operators (II) | `ptr->getProductCode()`, `displayProduct(*found)` | Access through a pointer / dereference |
| `std::string` and modifiers (II) | `erase()` in `trim()`, `append()`/`insert()` in tests, `substr()`, `length()`, `[]` | All text fields and validation |

---

## 2. Design decisions and why

### Why is `Product` the main entity, not `Supplier`?
The owner's question is always about a **product**: *"Is rice running low? How much
should I order? Whom do I call?"* The reorder decision depends on product fields
(stock, level, quantity). Supplier details are simply stored inside each product.

### Why `productCode` as the unique key and not `name`?
Names are long and inconsistent ("Basmati Rice 5kg" vs "basmati rice 5 kg").
A short code is what shops already write on shelf labels, and it's easy to type
and compare. Two different products can have similar names, but never the same code.

### Why convert the code to UPPERCASE?
Without it, `rc-101` and `RC-101` would be stored as two "different" products,
which is exactly the kind of duplicate the project must prevent. Normalising once,
in `Product::normalizeCode()`, makes every comparison consistent.

### Why a fixed array `Product items[50]`?
1. Phase 1 explicitly requires an **array of objects**.
2. A fixed array is what Unit II teaches, so it is the right tool here.
3. A small shop needs a few dozen tracked products, so 50 is enough, and the
   limit gives a clear boundary case to test (50 accepted, 51st rejected).

### Why `setAll()` (Phase 1), and how constructors use it (Phase 2)
In Phase 1 constructors were not allowed yet, so `setAll()` filled a product by
calling each setter. In Phase 2 the parameterised constructor simply calls
`setAll()`, so the checking rules are still written in only one place.

### Why does `findByCode()` return a pointer?
"Not found" is a normal result. A pointer can express it with `nullptr`. If the
function returned a `Product` or a `Product&`, we'd have to invent a fake
"empty" product for the not-found case. The pointer also avoids copying the
object, and it points to the **real** product in the array.

### Why is the validation in static functions?
Validation doesn't depend on any particular object. `Product::isValidCode("RC-101")`
can be called **before** creating or storing anything, for example to reject a bad
code as soon as the user types it. Setters and `isValid()` reuse the same static
functions, so each rule is written in only one place.

### Why is `totalAdded` private, and why is `ProductStore` a friend class?
If the counter were public, any code could write `Product::totalAdded = 999;`.
Keeping it private and letting only the friend `ProductStore` change it means the
counter goes up **only** when a product is actually stored.

### Why is `displayProduct` a friend function and not a member?
Both would work. It is a friend because Phase 1 asks for a friend function, and
it's a natural fit: printing is a *view* of a product, not part of its data rules.
It shows the key idea: a non-member that can still read private members because
the class allowed it.

### Why use `this->`?
The setter parameters have the same names as the data members
(`setName(const std::string& name)`). Inside the function, plain `name` refers to
the **parameter**. `this->name` refers to the object's member. Without `this->`,
`name = name;` would assign the parameter to itself and the object would never change.

### Why read every input with `getline()` and check it by hand?
With `cin >> number`, typing `abc` puts `cin` into a fail state, and the program
can loop forever unless it is reset. Typing `12abc` silently stores 12. Reading the
whole line and then checking it with `parseInt()` / `parseMoney()`:
- rejects `abc`, `12abc`, `1.5` and `-5` with a clear message,
- never breaks `cin`,
- keeps every rule visible in our own code.

### Why store the phone as a string?
A phone number is an identifier, not a quantity. It is never added or multiplied,
it may start with 0, and `9876543210` is larger than the maximum `int` (2147483647).

### Why `int` for stock and `double` for cost?
Stock is counted in whole units, while cost has paise. `parseMoney()` accepts at
most 2 decimals, so values such as `4.555` are rejected.

### Why is the duplicate check done twice?
1. **In the menu**, right after the code is typed, so the user doesn't fill in seven
   more fields only to be rejected at the end.
2. **Inside `addProduct()`**, because the store must protect itself no matter who
   calls it (the menu, the demo loader or the test program).

### Why does a blank line cancel?
The user always has a way out of the add screen, and no half-filled product is
ever stored.

### Procedural vs object-oriented: how does this project show the difference?
In a procedural version, the data would be loose arrays (`codes[50]`,
`stocks[50]`, …) and any function could change them. Here, the data and its rules
live together in `Product`. The fields are private, and the only way to change them
is through setters that validate.

---

## 3. Likely viva questions with short answers

**Q1. What is a static data member? Where is yours?**
A member with only one copy for the whole class, shared by all objects.
`Product::totalAdded` counts products added. It is declared in the class and
defined once in `Product.cpp` with `int Product::totalAdded = 0;`.

**Q2. Why must a static data member be defined outside the class?**
The declaration inside the class doesn't allocate memory. The definition in one
`.cpp` file does. (A `static const int` with a value given in the class, such as
`MAX_PRODUCTS`, is the exception.)

**Q3. Can a static member function use `this`?**
No. It isn't called on any object, so there is no `this`. That's why
`isValidCode()` takes the code as a parameter.

**Q4. What is an inline function? Is it always inlined?**
It's a request to the compiler to put the function's body at the call site, which
avoids call overhead for small functions. Functions defined inside the class body
are inline automatically. The compiler may ignore the request (for example, for
large or recursive functions).

**Q5. What is a friend function? Does it break encapsulation?**
It's a non-member function that the class allows to read its private members.
It breaks encapsulation only in a controlled way: the **class itself** decides who
its friends are, and outside code cannot make itself a friend.

**Q6. Friend function vs friend class?**
A friend function gets access for one function. A friend class gets access for
all of that class's member functions. Here `ProductStore` is a friend class.
Friendship isn't mutual and isn't inherited.

**Q7. What is the `this` pointer?**
A hidden pointer passed to every non-static member function. It holds the address
of the object the function was called on. Its type in a const function is
`const Product*`.

**Q8. Explain the pointer arithmetic in `findByCode()`.**
`items` decays to the address of `items[0]`. `items + count` is one past the last
used slot. `ptr++` moves forward by `sizeof(Product)` bytes, to the next object.
The loop stops when `ptr` reaches `end`.

**Q9. What is a null pointer? What are dangling and wild pointers?**
- A **null pointer** points to nothing (`nullptr`). We return it when a code isn't found and always check for it before using the pointer.
- A **wild pointer** is never initialised. Every pointer in this code is initialised when it is declared.
- A **dangling pointer** points to memory that has been freed. This program never uses `new`/`delete`, and the array lives as long as `store` in `main()`, so the pointers it returns stay valid.

**Q10. Pointer vs reference?**
A reference is an alias that must be initialised and can't be null or reseated. A
pointer can be null and can be changed to point somewhere else. That's why
`findByCode` returns a pointer (it may be null), while `addProductMenu(ProductStore&)`
takes a reference (a store is always there).

**Q11. Call by value vs address vs reference: give an example from your code.**
- Value: `isValidQty(int qty)` gets a copy.
- Reference: `parseInt(..., int& value)` writes the result back into the caller's variable.
- Address: `findByCode()` hands back the **address** of an object, which the caller uses through `->`.

**Q12. How do you prevent duplicate keys?**
The code is normalised (trimmed, uppercased). `findByCode()` searches the used part
of the array, and if it finds a match, the add is rejected with `ADD_DUPLICATE`.
This happens both in the menu and inside `addProduct()`.

**Q13. What happens if a user types letters for the stock?**
`parseInt()` checks every character and finds a non-digit, so it returns false. The
program prints "Invalid: enter a whole number…" and asks again. `cin` never enters
a fail state because input is read as a string.

**Q14. Why did you use `enum`?**
`return ADD_DUPLICATE;` is self-explaining. `return 2;` isn't. The `switch` in
`addResultMessage()` also lists every case clearly.

**Q15. Which string modifiers did you use?**
- `erase()` in `trim()`
- `append()` and `insert()` in `makeCode()` in the tests
- `substr()` to shorten long names in tables
- `operator[]` to change characters in `toUpperCase()`

**Q16. What is the time complexity of search?**
It checks the products one by one, at most 50 of them, which is instant at this size.

**Q17. How did you test it?**
The `tests/test_phase1.cpp` program runs 57 automatic checks, split into normal,
invalid and boundary cases. These include exactly 50 products followed by a 51st,
code lengths 2/3/10/11, and stock equal to the reorder level. Three scripted input
files in `/data` replay full menu sessions, and their outputs are saved in `/tests`.


---

# Phase 2 (CA2): constructors, destructor and file handling

## 4. Where each Phase 2 concept is used

| Concept (Unit III) | Where in the code | What it does there |
|---|---|---|
| Default constructor | `Product::Product()` | Makes a blank product; runs 50 times for `Product items[50]` |
| Parameterised constructor | `Product(code, name, ..., cost)` | Builds a full product; used by `loadDemoProducts()` and tests |
| Copy constructor | `Product(const Product& other)` | `Product before(*found);` keeps the old values during an update |
| Constructor overloading | 4 `Product` constructors | The compiler picks one by the arguments given |
| Initializer lists | Every constructor, e.g. `: count(0), loadedCount(0), skippedCount(0), file(path)` | Members get their values as they are created |
| Constructor with default argument | `ProductFile(const string& path = "data/products.dat")`, same for `ProductStore` | Normal program uses the default; tests pass their own file |
| Destructor | `ProductFile::~ProductFile()` | Closes the file automatically when the object is destroyed |
| Structure and file operation | `struct ProductRecord` | Fixed-size shape of one product on disk |
| Opening modes | `ios::out | ios::app | ios::binary` (create), `ios::in | ios::out | ios::binary` (use), `ios::trunc` (tests) | Create without erasing, read and write, start a test file empty |
| Binary read / write | `file.read((char*)&record, RECORD_SIZE)`, `file.write(...)` | Moves one whole record at a time |
| Sequential access | `rewind()` + `readNext()` in `loadFromFile()` | Reads every record in order at start-up |
| Random access, read | `seekg(index * RECORD_SIZE)` in `readAt()` | Jumps straight to one record |
| Random access, write | `seekp(index * RECORD_SIZE)` in `writeAt()` | Rewrites one record in place (update) |
| `tellg` | `sizeInBytes()` | File size = position at the end |
| Classes and file operations | `ProductFile` class | All file work in one class; the rest of the program never touches `fstream` |

## 5. Phase 2 design decisions and why

### Why a separate `ProductRecord` struct instead of writing `Product` directly?
A `std::string` object does not hold its letters inside itself; it holds the
address of memory where the letters are. `file.write((char*)&product, ...)` would
save that address, which means nothing after a restart. Copying each string into
a fixed-size `char` array gives a record with no addresses in it, and every
record has exactly the same size (128 bytes).

### Why fixed-size records?
Because then record n always starts at byte `n * 128`. That one multiplication
is what lets `seekg` and `seekp` jump straight to any record. With
variable-length records (for example one line of text per product), the only
way to find record n would be to read all the records before it.

### Why pad the char arrays with `'\0'`?
`copyToCharArray()` writes `'\0'` into every unused byte. Without that, the
file would contain whatever leftover bytes were in memory, which makes the
file different every time for the same data and is harder to check (see the
hex dump in `tests/products_dat_hexdump.txt`).

### Why a separate `ProductFile` class?
All `fstream` code (open, modes, seek, read, write, close) is in one place. The
store only says "append this record" or "write record 3". If the file format
changes, only one class changes. It also lets the constructor and destructor
manage opening and closing.

### Why open the file twice in the constructor?
Opening an `fstream` with `ios::in | ios::out` fails if the file does not exist.
So the constructor first opens an `ofstream` with `ios::app`, which creates the
file if it is missing and never erases existing data, closes it, and then opens
the real `fstream` for reading and writing.

### Why does the destructor close the file?
The destructor runs automatically when the object is destroyed: at the end of
`main()`, or at the end of a block in the tests. So the file is always closed,
even if the code that used it forgot. Each write also calls `flush()`, so a
crash would not lose a saved product.

### Why write to the file first, then to memory?
In `addProduct()` and `updateProduct()` the file is written first. If the write
fails, memory is not changed either, so what the user sees and what is saved
never disagree.

### Why keep the array as well as the file?
The array (Phase 1) makes listing, the reorder list and the duplicate check fast
and simple. The file is the permanent copy. On start-up the file is read once
into the array. Search (menu 3) and "view record" (menu 8) read the file
directly, to show `seekg` working.

### Why `recordNo[50]`?
If a damaged record is skipped on load, slot 2 in the array may really be
record 3 in the file. `recordNo` remembers each slot's real record number, so an
update always rewrites the right bytes (test F6).

### Why a copy constructor in the update?
`Product before(*found);` makes a separate copy of the old values before
anything changes, so the program can show "before" and "after". A second copy,
`changed`, is edited and checked by the setters. The stored product is only
replaced once the new values are valid and saved.

### Why `copyFrom()` instead of `a = b`?
Once a class has its own copy constructor, the compiler's automatic `=` is
marked as deprecated. Writing my own `operator=` would be operator overloading,
which is a later unit. A plain member function, `copyFrom()`, does the same job
clearly. It also shows the difference between a constructor and a normal
function: the copy constructor creates a **new** object, while `copyFrom()`
changes an object that **already exists**.

### Why can't the product code be updated?
The code is the key used to find the record. Changing it would make the old
code disappear and could create a duplicate. To "rename" a product the owner
would add a new one (a later phase may add delete).

## 6. Likely Phase 2 viva questions with short answers

**Q1. What is a constructor? How is it different from a normal function?**
A member function with the same name as the class and no return type. It runs
automatically when an object is created, and it cannot be called again on an
existing object. A normal function (like `copyFrom` or `setAll`) has a return
type and runs only when you call it.

**Q2. Which constructors does `Product` have?**
- default `Product()`
- parameterised `Product(code, name, ..., cost)`
- copy constructor `Product(const Product&)`
- `Product(const ProductRecord&)`, which builds a product from a file record

Having several is constructor overloading.

**Q3. When is the copy constructor called?**
When a new object is made from an existing one: `Product before(*found);`,
`Product copy = p;`, passing a `Product` by value, or returning one by value
(the compiler may skip that copy as an optimisation).

**Q4. Why must the copy constructor take a reference?**
If it took `Product other` by value, making that copy would need the copy
constructor, which would need another copy, and so on forever. A reference
passes the original without copying it.

**Q5. What is an initializer list and why use it?**
The part after the colon in a constructor:
`ProductStore::ProductStore(const string& path) : count(0), ..., file(path)`.
It gives members their values when they are created instead of assigning later.
It is required for members that need constructor arguments, like `file(path)`.

**Q6. What is a constructor with default arguments?**
`ProductFile(const string& path = "data/products.dat")`. Writing `ProductFile f;`
uses the default file, and `ProductFile f("tests/x.dat");` uses another.

**Q7. What does your destructor do and when does it run?**
`~ProductFile()` closes the file if it is open. It runs automatically when the
object is destroyed, for example when `main()` ends or a block in the tests ends.
A class has only one destructor, with no parameters and no return type.

**Q8. Why binary mode and not text mode?**
In text mode some systems change line-ending bytes while reading or writing,
which would corrupt numbers stored as raw bytes. Binary mode copies bytes
exactly, and it lets every record be the same size.

**Q9. Difference between seekg and seekp? tellg?**
`seekg` moves the **get** (read) position, `seekp` moves the **put** (write)
position. `tellg` returns the current read position, used after
`seekg(0, ios::end)` to get the file size.

**Q10. How do you find record n?**
`seekg(n * RECORD_SIZE, ios::beg)` then `read`. `RECORD_SIZE` is
`sizeof(ProductRecord)` = 128 bytes, so record 3 starts at byte 384.

**Q11. How does search work?**
For i = 0 to count-1: `seekg(i * 128)`, read one record, compare its code.
Return i on a match, -1 otherwise. It reads one record at a time, never the
whole file at once.

**Q12. How does update work, and why doesn't the file grow?**
Find the product, copy it, change one field through its setter, then
`seekp(recordNo * 128)` and write 128 bytes over the old record. The same
bytes are replaced, so the file size stays the same (test U3).

**Q13. Sequential vs random access in your project?**
- Sequential: `loadFromFile()` reads records one after another from the start with `readNext()`.
- Random: search, update and "view record" jump straight to a record with `seekg` or `seekp`.

**Q14. Which file opening modes did you use?**
- `ios::out | ios::app | ios::binary`: create the file without erasing it.
- `ios::in | ios::out | ios::binary`: read and write the data file.
- `ios::out | ios::trunc | ios::binary`: empty a test file.

**Q15. What happens if the file is damaged?**
- Extra bytes that don't make a whole record are ignored, because the record count is size / 128.
- A record with invalid values, or a duplicate code, is skipped and counted.
- If the file can't be opened at all, the program warns and refuses to add.

Tests F1 to F8 cover these cases.

**Q16. Why do you call `clear()` before seeking?**
After reading past the end, the stream sets its end-of-file flag, and while a
flag is set every later operation fails. `clear()` resets the flags.

**Q17. Why `(char*)&record` in read and write?**
`read` and `write` work with bytes (`char*`). The cast tells the stream to
treat the structure's memory as a block of bytes of size `RECORD_SIZE`.

**Q18. How do you prove the data survives a restart?**
- Test P1 destroys a store (its destructor closes the file) and builds a new one on the same file. All 3 products come back.
- Test U7 does the same after an update.
- In the demo, run 2 (`output_phase2_run2.txt`) is a fresh run of the program that shows the products and the updated stock from run 1.
