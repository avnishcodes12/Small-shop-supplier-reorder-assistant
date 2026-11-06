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

### Why do the data members have starting values, and why `setAll()`?
`Product items[50];` creates 50 objects at once. Giving each data member a
starting value in the class (`int currentStock = 0;`, strings start empty) means
every one of those 50 objects starts blank, and `isValid()` is false for it.
Constructors belong to a later unit, so instead of a parameterised constructor
the class has `setAll()`, which fills every field by calling each setter in turn
and returns true only if all of them were valid.

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
