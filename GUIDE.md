# Interview Guide — Quick Review

## Table of Contents
- [Iterator Categories](#iterator-categories)
- [STL Container Operations](#stl-container-operations----why-list-has-its-own-sort-and-remove)
- [STL Member Access — [] vs at() vs data()](#stl-member-access----vs-at-vs-data)
- [Operator Precedence Trap — Ternary with <<](#operator-precedence-trap----ternary-with-)
- [Vector vs Deque Structure](#vector-vs-deque-structure)
- [std::set — Interview API](#stdset----interview-api)
- [STL Gotchas](#stl-gotchas)
- [Quick Reference Files](#quick-reference-files)
- [Functors, Lambdas, Function Pointers](#functors-lambdas-function-pointers----evolution)
- [Bit Manipulation — LSB, MSB, Runs](#bit-manipulation----lsb-msb-left--right-consecutive-run)
- [Concurrency](#concurrency) — see cpp_concurrency_notes.md
- [NAT (Network Address Translation)](#nat-network-address-translation)
- [Edit Distance (Recursive)](#edit-distance-recursive)
- [Infix to Postfix (Shunting Yard)](#infix-to-postfix-shunting-yard)
- [Integer Sizes](#integer-sizes-64-bit-linux)
- [explicit Keyword](#explicit-keyword)
- [Armstrong / Pure Numbers](#armstrong--pure-numbers)
- [Circle Drawing (Bresenham)](#circle-drawing-bresenham----no-trig-integer-points-only)
- [Insert Iterators](#insert-iterators)
- [pow() Precision Trap](#pow-precision-trap)
- [float vs double](#float-vs-double)
- [Virtual Functions](#virtual-functions)
- [Buddy System](#buddy-system-segment-tree-with-andor-propagation)
- [Geometry: Fix-Derive-Lookup Pattern](#geometry-fix-derive-lookup-pattern)
- [Floating Point Representation](#floating-point-representation)

---

## Iterator Categories

```
Input/Output → Forward → Bidirectional → Random Access → Contiguous
```

| Category | Direction | Read | Write | Jump | Examples |
|---|---|---|---|---|---|
| **Input** | forward only | yes | no | no | `istream_iterator` |
| **Output** | forward only | no | yes | no | `ostream_iterator` |
| **Forward** | forward only | yes | yes | no | `forward_list`, `unordered_map` |
| **Bidirectional** | both | yes | yes | no | `list`, `map`, `set` |
| **Random Access** | both | yes | yes | yes (+n, -n) | `vector`, `deque`, `array` |
| **Contiguous** | both | yes | yes | yes | `vector`, `array`, `string` |

For Forward/Bidirectional/Random Access: Examples column lists *containers* — that container's `::iterator` belongs to that category (e.g. `list::iterator` is bidirectional). For Input/Output: the example is the stand-alone iterator type itself.

Two kinds: **container iterators** (from `begin()`/`end()`, use `auto`) vs **stand-alone adapters** (`istream_iterator`, `back_insert_iterator`, `reverse_iterator` etc.).

### LeetCode Priority

| Priority | Iterator/Function | Why |
|---|---|---|
| **Must know** | `begin/end`, range-for | Used everywhere |
| **Must know** | `lower_bound/upper_bound` | Binary search, sorted containers |
| **Must know** | Erase-while-iterating pattern | Array/string problems |
| **Must know** | `find`, `distance` | Search + index |
| **High** | `next/prev/advance` | Linked list, sliding window |
| **High** | `back_inserter` | With `copy`/`transform` |
| **Medium** | `rbegin/rend` | Reverse traversal |
| **Medium** | Map `.lower_bound()` member | O(log n) vs free function O(n) |
| **Low** | Stream iterators | Competitive input tricks |

---

## STL Container Operations — Why list has its own sort and remove

| Operation | vector/deque | list |
|---|---|---|
| `erase`/`insert` | member — shifts elements O(n) | member — relinks pointers O(1) |
| `sort` | use `std::sort` — needs random access iterators ✓ | `std::sort` won't work — no random access; use `list::sort()` (merge sort) |
| `remove`/`remove_if` | use `std::remove` + `erase` — shifts elements | use `list::remove()` — relinks pointers, actually erases |
| `reverse` | use `std::reverse` — copies/swaps elements | use `list::reverse()` — relinks pointers, no copying |

- **`sort`** — list lacks random access iterators, `std::sort` can't work → list provides merge sort
- **`reverse`, `remove`, `remove_if`** — std algorithms would work but are inefficient (shift/copy); list's own versions relink pointers — O(1) per node, no copying

---

## STL Member Access — `[]` vs `at()` vs `data()`

- `v[i]` — no bounds check, undefined behavior if out of range. Use when index is known valid.
- `v.at(i)` — bounds check, throws `std::out_of_range` if out of range. Use for external/user input.
- `v.data()` — returns raw `T*` pointer to internal array. **Only for C interop** — never needed in interviews.

---

## Operator Precedence Trap — Ternary with `<<`

`<<` has higher precedence than `?:` — always wrap ternary in parentheses:
```cpp
cout << (vx.empty()) ? "True" : "False" << endl;    // ✗ — parses wrong
cout << ((vx.empty()) ? "True" : "False") << endl;  // ✓
```

---

## Vector vs Deque Structure

```
vector: [1][2][3][4][5]              — one contiguous block
deque:  [ptr0][ptr1][ptr2]           — array of pointers to equal-sized chunks
           ↓     ↓     ↓
        [1,2,3][4,5,6][7,8,9]
```
- Vector: fast `[]`, fast `push_back`, slow `push_front`, has `data()`
- Deque: fast `[]`, fast `push_back` AND `push_front`, no `data()` (not contiguous)
- Deque `[]`: two steps — which chunk (i / chunk_size), offset within chunk (i % chunk_size)

**STL Adaptors — `stack`, `queue`, `priority_queue` wrap a container:**
```cpp
stack<int>               // uses deque internally by default
stack<int, vector<int>>  // can swap underlying container
```

---

## std::set — Interview API

```
begin, end, rbegin, rend
insert, erase, clear
empty, size             ← NO resize (size determined by inserts)
count, find, contains   ← contains is C++20 only
lower_bound, upper_bound
```

- **`count(x)`** — returns 0 or 1, use for clean existence check: `if (s.count(x))`
- **`find(x)`** — returns iterator, use when you need to erase or pass to algorithm
- **`end()` is never dereferenceable** — undefined behavior, always guard: `if (it != s.end())`
- **`equal_range(x)`** — returns `{lower_bound(x), upper_bound(x)}` as a pair. Useless for `set` (no duplicates). Use for `multiset`/`multimap` to get range of equal elements:
```cpp
multiset<int> ms = {1, 2, 2, 2, 3};
auto [lo, hi] = ms.equal_range(2);
for (auto it = lo; it != hi; ++it) cout << *it;  // 2 2 2
```

---

## STL Gotchas

**`min` takes only two args — use initializer list for multiple values:**
```cpp
min(1, 3)               // two args only
min({1, 3, 4, 9, 10})  // initializer_list overload — use this for multiple values
```
Both are separate overloaded templates in `<algorithm>` — same name, different signatures. Compiler picks the right one at compile time. Function overloading applies to templates just like regular functions.

**`lower_bound` and `upper_bound` require a sorted range — use binary search internally, O(log n). Unsorted input = undefined result.**

**`binary_search` only returns true/false — prefer `lower_bound` instead:**
```cpp
auto it = lower_bound(v.begin(), v.end(), x);
if (it != v.end() && *it == x) { /* found — it points to element */ }
```
`lower_bound` gives position AND existence check — strictly supersedes `binary_search`.

**`lower_bound` and `upper_bound`:**
```
v = {1, 2, 2, 2, 3}  searching for 2
lower_bound = index 1  (first 2)
upper_bound = index 4  (first 3)
```
Together they give the range of all matching elements — `[lo, hi)` = all 2s. `hi - lo` = count.

**`count` works on single elements only — for substrings use `find` in a loop:**
```cpp
size_t p = s.find(sub);
while (p != string::npos) {
    count++;
    p = s.find(sub, p + sub.size());  // non-overlapping
    // p = s.find(sub, p + 1);        // overlapping
}
```

**`unique` does not erase — use the returned iterator:**
```cpp
auto last = unique(v.begin(), v.end());  // moves dupes to end, returns new end
copy(v.begin(), last, ...);              // use last, not v.end() — tail is garbage
```

---

## Quick Reference Files
- **locks.cpp** — lock_guard, unique_lock, shared_lock, defer_lock, condition variables, producer-consumer pattern
- **bounded-queue.cpp** — thread-safe bounded queue with atomic, mutex, cv, jthread — full working example

---

## Functors, Lambdas, Function Pointers — Evolution

| Style | Since | Carries State? | Syntax |
|---|---|---|---|
| Function pointer | Always | No | `sort(v, cmp)` — name decays to pointer |
| Functor (struct with `operator()`) | Always | Yes | `sort(v, Cmp())` — must construct object |
| Lambda | C++11 | Yes (captures) | `sort(v, [](int x, int y){ return x < y; })` |
| `std::function` | C++11 | Yes | `function<bool(int,int)> f = lambda; sort(v, f)` |

```cpp
vector<int> v = {1, 3, 2, 4, 5, 2, 5, 56, 9};

bool cmp(int x, int y) { return x < y; }          // function pointer
sort(v.begin(), v.end(), cmp);

struct Cmp {
    bool operator()(int x, int y) { return x < y; }
};
sort(v.begin(), v.end(), Cmp());                   // functor — Cmp() constructs object

function<bool(int,int)> f = [](int x, int y){ return x < y; };
sort(v.begin(), v.end(), f);                       // std::function wrapping lambda
```

Lambdas are compiler-generated functors — anonymous struct with `operator()` under the hood. `std::function` wraps all three into one uniform type.

**Why `cmp` vs `Cmp()`:** `cmp` is already callable (name decays to pointer). `Cmp` is a class — `Cmp()` constructs a temporary object whose `operator()` is the callable.

**Duck Typing:** `sort`'s third parameter is `template<typename Compare>` — same signature for all three. Compiler just checks `comp(a, b)` compiles. No inheritance needed. Same for all STL algorithms.

**`less<T>` and `greater<T>`:** functor templates from `<functional>` — `less<int>()` sorts ascending, `greater<int>()` descending. For custom classes, define `operator<` and `operator>`:
```cpp
struct Dog {
    string name; int age;
    bool operator<(const Dog& o) const { return age < o.age; }
    bool operator>(const Dog& o) const { return age > o.age; }
};
sort(dogs.begin(), dogs.end(), less<Dog>());     // ascending by age
sort(dogs.begin(), dogs.end(), greater<Dog>());  // descending by age
sort(dogs.begin(), dogs.end());                  // also works — uses operator< directly
```
Minimum needed for default `sort`: just `operator<`.

---

## Bit Manipulation — LSB, MSB, Left & Right Consecutive Run

| Goal | How | Result on `0110 1100` |
|---|---|---|
| LSB position | `__builtin_ctz(n)` | `2` |
| LSB value | `n & (-n)` | `0000 0100` |
| MSB position | `31 - __builtin_clz(n)` | `6` |
| MSB value | `1 << (31 - __builtin_clz(n))` | `0100 0000` |
| Rightmost run | ctz → shift right → trailing-1s trick → shift back | `0000 1100` |
| Leftmost run | clz → shift left → clz(~norm) = length → mask → shift back | `0110 0000` |

```cpp
unsigned int n = 0b01101100;

// LSB
int          lsb_pos = __builtin_ctz(n);
unsigned int lsb_val = n & -n;

// MSB
int          msb_pos = 31 - __builtin_clz(n);
unsigned int msb_val = 1u << msb_pos;

// Rightmost consecutive run
int          r_shift = __builtin_ctz(n);
unsigned int r_norm  = n >> r_shift;
unsigned int r_run   = ((r_norm ^ (r_norm + 1)) >> 1) << r_shift;

// Leftmost consecutive run
int          l_shift = __builtin_clz(n);
unsigned int l_norm  = n << l_shift;
int          run_len = __builtin_clz(~l_norm);
unsigned int l_run   = (~0u << (32 - run_len)) >> l_shift;
```

- Builtins map to single x86 instructions — `__builtin_clz` → `LZCNT/BSR`, `__builtin_ctz` → `BSF`, `__builtin_popcount` → `POPCNT`. One CPU cycle each.
- `__builtin_clz(0)` is undefined — always guard with `if (n != 0)`.

---

## Concurrency

See **cpp_concurrency_notes.md** for full concurrency reference:
locks, condition variables, semaphores, packaged_task, spinlock, event callback, move/forward, variadic templates.

---

## NAT (Network Address Translation)

**SNAT (Source NAT):** internal host going OUT — rewrites source IP:port to public IP:port.
**DNAT (Destination NAT):** external host coming IN — rewrites destination IP:port to internal server.
**Static NAT:** fixed 1-to-1 mapping, ports unchanged, server reachable directly.
**PAT (Port Address Translation):** many hosts share one public IP, differentiated by port.

FortiGate NAT table entry: `internal IP | int port | external IP | ext port | protocol`
— used to reverse-translate the response back to the original sender.

---

## Edit Distance (Recursive)

```cpp
int EditDist(string s1, string s2, int l1, int l2) {
    if (l1 == 0) return l2;   // insert all of s2
    if (l2 == 0) return l1;   // delete all of s1
    if (s1[l1-1] == s2[l2-1]) return EditDist(s1, s2, l1-1, l2-1);
    return 1 + min({
        EditDist(s1, s2, l1-1, l2-1),  // replace
        EditDist(s1, s2, l1-1, l2),    // delete
        EditDist(s1, s2, l1, l2-1)     // insert
    });
}
```

---

## Infix to Postfix (Shunting Yard)

1. Operand → push to output
2. `(` → push to operator stack
3. `)` → pop to output until `(`
4. Operator → pop higher/equal priority operators to output first, then push
5. End → drain operator stack to output

Priority: `* /` = 2, `+ -` = 1

---

## Integer Sizes (64-bit Linux)

| Type        | Size    | Notes                                            |
|-------------|---------|--------------------------------------------------|
| `int`       | 32 bits |                                                  |
| `long`      | 64 bits | 32 bits on Windows — not portable                |
| `long long` | 64 bits | Guaranteed 64-bit on all platforms — prefer this |

No 128-bit standard type — use `__int128` (GCC only) or big integer library.

---

## `explicit` Keyword

Prevents silent construction from a plain value:
```cpp
explicit ThreadPool(int n);

ThreadPool p = 4;   // ERROR — must be explicit
ThreadPool p(4);    // OK
startWork(4);       // ERROR — no silent conversion
startWork(ThreadPool(4)); // OK
```
Use on any single-argument constructor to prevent accidental construction of expensive objects.

---

## Armstrong / Pure Numbers

A number where sum of `digit^n` (n = number of digits) equals the number itself.
- Zero and single digits: always pure
- Negative numbers: never pure (sum of digits^n is always positive)
- Count is **finite** — because `9^n * n` grows much slower than `10^(n-1)` (smallest n-digit number). Beyond ~39 digits no Armstrong numbers can exist in base 10.
- Largest base-10 Armstrong number fits in `long long` — no big integer needed.

---

## Circle Drawing (Bresenham) — No trig, integer points only

Two things to remember:
1. Compute one octant (while `sx <= sy`), reflect each point to all 8 octants
2. Each step: try `sy` and `sy-1`, pick whichever gives `sx²+sy²` closer to `r²`, carry `sy` forward

```cpp
// 8-way reflection of point (x,y):
{x,y}, {y,x}, {-y,x}, {x,-y}, {-x,-y}, {-y,-x}, {y,-x}, {-x,y}

// core loop:
int sx=1, sy=r;
while (sx <= sy) {
    // try sy and sy-1, pick closer to r²
    sy = chosen_y;
    sx++;
}
// add axis points separately: (0,r),(r,0),(0,-r),(-r,0)
```

---

## Insert Iterators

| Container                                       | Use                                      |
|-------------------------------------------------|------------------------------------------|
| `vector`, `deque`, `list`                       | `back_inserter(c)` — calls `push_back`   |
| `set`, `map`, `unordered_set`, `unordered_map`  | `inserter(c, c.end())` — calls `insert`  |
| `list`, `deque` (front)                         | `front_inserter(c)` — calls `push_front` |

Rule: **sequential → `back_inserter`. sorted/hashed → `inserter`.**

```cpp
vector<int> v;
copy(src.begin(), src.end(), back_inserter(v));

set<int> s;
copy(src.begin(), src.end(), inserter(s, s.end()));
```

---

## pow() Precision Trap

`pow()` returns `double` — truncating to `int` can give wrong answer:
```cpp
(int)pow(9, 3)  →  728  // should be 729
```
Always use integer power for integer problems:
```cpp
int ipow(int base, int exp) {
    int result = 1;
    while (exp--) result *= base;
    return result;
}
```

---

## float vs double

| Type     | Size    | Precision           |
|----------|---------|---------------------|
| `float`  | 32 bits | ~7 decimal digits   |
| `double` | 64 bits | ~15 decimal digits  |

`double` is **larger** and more precise than `float`. Default floating point literals in C++ are `double`. Use `float` only when memory is tight (e.g. large arrays, GPU).

---

## Virtual Functions

- Each object carries a hidden `vptr` → points to its class's vtable → vtable holds function pointers
- **Dynamic dispatch** (virtual): which function is called determined at runtime by the object's actual type
- **Static dispatch** (non-virtual): determined at compile time by the pointer's declared type
- Cost: one extra pointer dereference — near zero but avoidable in tight loops
- `virtual void foo() = 0` → pure virtual → class is abstract, cannot be instantiated

```
object                vtable
+--------+           +------------------+
| vptr   |  ------>  | &Derived::foo()  |
| data   |           | &Derived::bar()  |
+--------+           +------------------+
```

---

## Buddy System (Segment Tree with AND/OR propagation)

Complete binary tree over `n` leaf nodes (n = next power of 2). Each leaf = one memory slot.

**Build:** BFS — pair leaves into parents, pair parents into grandparents, until one root remains.

**setBit(offset, len):** set leaves in range to `1`, walk up from each leaf:
```cpp
node->flag = node->left->flag && node->right->flag;  // parent=1 only if both children=1
```

**clearBit(offset, len):** set leaves in range to `0`, walk up from each leaf:
```cpp
node->flag = node->left->flag || node->right->flag;  // parent=0 only if both children=0
```

Next power of 2:
```cpp
int len = 1;
while (len < sz) len <<= 1;
```

---

## Geometry: Fix-Derive-Lookup Pattern

When counting/finding shapes in a point set — anchor on minimum points that define the shape, derive remaining points mathematically, check existence in a hash set.

**Anti-pattern:** reduce points to distances globally → loses which points distances belong to → false matches across unrelated point groups.

**Problem 1:** Given 4 points, return true if they form a square.
- Compute distances² from one point to the other 3, sort: `d1 <= d2 <= d3`
- Check: `d1 == d2` (equal sides) and `d1 + d2 == d3` (Pythagorean theorem)
- Guard: `d1 == 0` → return false (degenerate case)

**Problem 2:** Given n points, return number of squares that can be formed.
- Fix every pair as one side — O(n²) pairs
- Derive other two corners by rotating side vector 90° counterclockwise:
```cpp
dx = p2.x - p1.x,  dy = p2.y - p1.y
p3 = (p2.x - dy,  p2.y + dx)
p4 = (p1.x - dy,  p1.y + dx)
```
- Check if p3 and p4 exist in `unordered_set` of all points — O(1)
- Each square counted 4 times (once per side) → divide final count by 4
- Complexity: O(n²)

**Key trick:** rotating vector `(dx, dy)` by 90° counterclockwise gives `(-dy, dx)`.

---

## Floating Point Representation

A number has two parts: **whole** (left of decimal point) and **fractional** (right).
- Whole part: always exact as long as it fits in available bits
- Fractional part: exact only if denominator is a power of 2

**Why?** Binary can only represent fractions whose denominator is a power of 2. Any other prime factor (3, 5, 7...) causes infinite recurring binary digits — truncated at the mantissa boundary.

```
0.5  = 1/2   → denominator 2¹  → exact ✓
0.25 = 1/4   → denominator 2²  → exact ✓
0.1  = 1/10  → 10 = 2×5, 5 not power of 2 → NOT exact ✗
0.2  = 1/5   → 5 not power of 2 → NOT exact ✗
1/3  = 0.333...→ recurs in decimal too → NOT exact ✗
```

**Storage limits (IEEE 754):**

| Type     | Mantissa bits | Max safe integer     |
|----------|---------------|----------------------|
| `float`  | 23 bits       | 2²³ = ~8 million     |
| `double` | 52 bits       | 2⁵² = ~4 quadrillion |

**Never compare floats with `==`** — use `abs(a - b) < epsilon` instead.

**Coding test — is a decimal exactly representable in binary?**
Multiply fractional part by 2 repeatedly:
- Fractional part hits 0 → exact
- Same fractional part seen twice → recurring → not exact

```
0.5:  0.5×2 = 1.0  → frac = 0        → EXACT ✓

0.1:  0.1×2 = 0.2  → frac = 0.2
      0.2×2 = 0.4  → frac = 0.4
      0.4×2 = 0.8  → frac = 0.8
      0.8×2 = 1.6  → frac = 0.6
      0.6×2 = 1.2  → frac = 0.2  ← seen before → NOT exact ✗
```

