# C++ Concurrency — Interview Quick Review

## Table of Contents
- [Include Headers](#include-headers)
- [When to Use What](#when-to-use-what)
- [Locks & Mutual Exclusion](#locks--mutual-exclusion)
- [Condition Variables](#condition-variables)
- [Semaphores](#semaphores)
- [packaged_task, future, async](#packaged_task-future-async)
- [move vs forward](#move-vs-forward)
- [T&& — Two Meanings](#t----two-meanings)
- [Variadic Templates](#variadic-templates)
- [Function Pointers](#function-pointers)
- [Spinlock & Mutex from Scratch](#spinlock--mutex-from-scratch)
- [Event Callback Pattern](#event-callback-pattern)

---

## Include Headers

```
#include <mutex>              — lock_guard, unique_lock, mutex
#include <shared_mutex>       — shared_lock, shared_mutex
#include <condition_variable> — condition_variable
#include <thread>             — thread, jthread
#include <future>             — future, promise, async
#include <semaphore>          — binary_semaphore, counting_semaphore
```

---

## When to Use What

| Scenario | Use | Typical Problem |
|---|---|---|
| Simple critical section | `lock_guard<mutex>` | Protect shared counter, shared data structure |
| Need manual unlock | `unique_lock<mutex>` + `defer_lock` | Lock two mutexes in order, unlock before long operation |
| Wait for state change | `unique_lock<mutex>` + `condition_variable` | Producer-consumer, event notification |
| Many readers, rare writes | reader: `shared_lock<shared_mutex>` writer: `unique_lock<shared_mutex>` | Read-heavy cache/config — readers run concurrently, writer excludes all |
| Signal between threads | `binary_semaphore` | One thread signals another to proceed |
| Limit concurrency to N | `counting_semaphore<N>` | Thread pool, connection pool, rate limiting |

**shared_mutex vs condition_variable:**
- `shared_mutex` — access control: many readers OR one writer
- `condition_variable` — waiting for state change (e.g. queue full/empty)

---

## Locks & Mutual Exclusion

`unique_lock` with `defer_lock` — construct without locking, lock manually later:
```cpp
unique_lock<mutex> ul(m, defer_lock);
ul.lock();
ul.unlock();
```

`jthread` ctor takes a callable — `jthread jth(worker)` where `worker` is lambda/functor/function pointer.

`jthread` is move-only — use `emplace_back`, not `push_back`:
```cpp
vector<jthread> jv;
jv.emplace_back(worker);            // ✓ constructs jthread(worker) in-place, no temporary
jv.emplace_back(jthread(worker));   // ✓ explicit but creates a temporary then moves in
jv.push_back(jthread(worker));      // ✓ temporary is rvalue, push_back T&& overload moves it
jv.push_back(move(jt));             // ✓ explicit move of named jthread (lvalue → rvalue)
jv.push_back(worker);               // ✗ tries to copy jthread (no copy ctor)
```

lvalue = has a name/address. rvalue = temporary, no name.
`jthread(worker)` is an rvalue — `push_back`'s `T&&` overload accepts and moves it without explicit `move()`.

Lambdas at global scope cannot capture — use `[]`, globals are accessible directly:
```cpp
auto w = []() { unique_lock<shared_mutex> wl(sm); };  // ✓
auto w = [&]() { ... };  // ✗ — not allowed at global scope
```

---

## Condition Variables

Always pass a **callable** (lambda or function pointer, no `()`) — not a bool. Bool is evaluated once and frozen:
```cpp
cv.wait(ul, []{ return !queue.empty(); });  // ✓ lambda
cv.wait(ul, isNotEmpty);                    // ✓ function pointer — no ()
cv.wait(ul, !queue.empty());                // ✗ bool evaluated once
cv.wait(ul, isNotEmpty());                  // ✗ () calls it, passes bool
```

`cv.wait` requires lock to be **held** going in — unlocks internally while sleeping, re-locks on wakeup.

After doing work: `ul.unlock()` → `notify_all()` → `ul.lock()` before next `cv.wait`:
```cpp
while (true) {
    cv.wait(ul, pred);      // lock must be held
    // do work
    ul.unlock();
    cv.notify_all();
    ul.lock();              // re-lock before next cv.wait
}
```

**Producer-consumer — two global cvs, one global mutex, local unique_locks. Works for any number of producers and consumers:**
```cpp
// producer:
cv_producer.wait(ul, [](){ return beg != (end+1) % MAX; });
// ... add item ...
ul.unlock(); cv_consumer.notify_all(); ul.lock();

// consumer:
cv_consumer.wait(ul, [](){ return beg != end; });
// ... remove item ...
ul.unlock(); cv_producer.notify_all(); ul.lock();
```

---

## Semaphores

```cpp
binary_semaphore s(1);         // 0 or 1 — like a mutex but release() can come from any thread
counting_semaphore<N> s(N);    // limit concurrency to N threads
s.acquire();                   // decrement, block if 0
s.release();                   // increment, wake a waiter
```

Use semaphore (not cv) when **counting** resources/slots. Use cv when waiting on **arbitrary state**.

Example — enforce ordering (lc1114):
```cpp
binary_semaphore S{0}, T{0};
void f() { cout << "F"; S.release(); }
void s() { S.acquire(); cout << "S"; T.release(); }
void t() { T.acquire(); cout << "T"; }
```

**Semaphore-based producer-consumer:**
- Two semaphores: `slots_to_fill{MAX}` and `slots_to_empty{0}`
- Pattern: `acquire()` outside lock → work inside lock → `release()` outside lock
- Use semaphore when just counting slots/items (simpler than cv)
- Any condition that can't be expressed as a simple resource count needs cv — because cv re-evaluates an arbitrary predicate, while a semaphore can only count:
  - "Wait until the queue has at least 3 items"
  - "Wait until a specific item type is in the queue"
  - "Wait until queue is non-empty AND a flag is set"
  - "Wait until queue is non-empty OR timeout has occurred"
  - `cv.wait_for(lock, timeout, pred)` — built-in timeout support

---

## async — Key Points

```cpp
int x = 5000;
future<int> t = async([](int& a) -> int { a = 2*a; return 2*a; }, ref(x));
// x may NOT be doubled here yet — async may not have completed
cout << t.get() << endl;  // t.get() is the only sync point
cout << x << endl;        // now x is guaranteed doubled
```

- Default launch policy: implementation-defined (may be deferred or concurrent)
- `launch::async` — forces new thread
- `launch::deferred` — forces lazy (runs only when `t.get()` is called)
- `t.get()` blocks until done — only synchronization point
- Exceptions thrown in async task are re-thrown at `t.get()`:
```cpp
int TestDrive(int& x) {
    throw runtime_error{"random runtime error"};
    return x + 1;
}
future<int> r = async(launch::async, TestDrive, ref(y));
try {
    r.get();
} catch (runtime_error ex) {
    cout << ex.what();
}
```

---

## std::function as callable container

```cpp
vector<function<void(int)>> callbacks;
callbacks.push_back([](int x){ cout << x; });
```

---

## packaged_task, future, async

| Primitive | Starts automatically? | Who runs it? |
|---|---|---|
| `thread(f)` | Yes — on construction | The new thread |
| `async(f)` | Yes — immediately | The runtime |
| `packaged_task(f)` | No — dormant | Whoever calls `task()` |

```cpp
packaged_task<void()> task(f);
future<void> r = task.get_future();  // BEFORE running
task();                               // run it (or move to a thread)
r.get();                              // block until done
```

`packaged_task` is **move-only** — move it out of a container before calling it.

Template syntax — use function **signature**, not `std::function`:
```cpp
packaged_task<void(int)> task(dowork);  // ✓ signature
future<void> r = task.get_future();
thread t(move(task), 3);
r.get(); t.join();
```

---

## move vs forward

```cpp
std::move(x)         // always casts to rvalue — signals "I'm done with this"
std::forward<T>(x)   // preserves original value category (lvalue stays lvalue, rvalue stays rvalue)
```

| Situation | Use |
|---|---|
| You own the variable and are done with it | `std::move` |
| Forwarding a template `T&&` parameter | `std::forward<T>` |

---

## T&& — Two Meanings

```cpp
void foo(int&& x);       // concrete type → rvalue reference — only binds to temporaries
template<typename T>
void bar(T&& x);         // deduced type  → forwarding reference — binds to anything
```

Reference collapsing: any `&` in the chain wins except `&& &&` which stays `&&`.

---

## Variadic Templates

```cpp
template<typename F, typename... Args>
auto submit(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>;
```

- `typename... Args` — pack of zero or more types
- `Args&&... args` — forwarding references for each
- `std::invoke_result_t<F, Args...>` — deduces return type of `F(Args...)`
- `std::forward<Args>(args)...` — expands and forwards the pack

---

## Function Pointers

```cpp
function<void()> cb = []{ ... };   // modern — accepts lambdas, captures, member functions
void (*cb)()       = myFunc;       // old C-style — plain functions only, no captures
```

- `function<void()>` — class type, wraps any callable
- `void (*)()` — raw pointer, plain functions only, zero overhead
- `void()` — signature (not a usable type directly)

Always prefer `std::function` in modern C++.

---

## Spinlock & Mutex from Scratch

```cpp
class SpinLock {
    atomic_flag flag = ATOMIC_FLAG_INIT;
public:
    void lock()   { while (flag.test_and_set(memory_order_acquire)) { ; } }
    void unlock() { flag.clear(memory_order_release); }
};

class Mutex {
    SpinLock s;
    bool flag = false;
public:
    void lock() {
        while (true) {
            s.lock();
            if (!flag) { flag = true; s.unlock(); return; }
            s.unlock();
            this_thread::sleep_for(1ms);
        }
    }
    void unlock() { s.lock(); flag = false; s.unlock(); }
};
```

- `atomic_flag::test_and_set()` — atomically sets to true, returns old value. If old=false you got the lock.
- `memory_order_acquire` on lock, `memory_order_release` on unlock — prevents CPU reordering.
- Plain `bool` doesn't work — two threads can both read false before either writes true.
- Spinlock: burns CPU, use for nanosecond-length critical sections. Mutex with sleep: yields CPU.

---

## Event Callback Pattern

```cpp
class EventHandler {
    bool fired = false;
    vector<function<void()>> callbacks;
    mutex m;

    void register_callback(function<void()> cb) {
        bool call_now = false;
        {
            lock_guard<mutex> lg(m);
            if (fired) call_now = true;
            else callbacks.push_back(cb);
        }
        if (call_now) cb();   // call OUTSIDE lock to avoid deadlock
    }

    void event_fired() {
        vector<function<void()>> toCall;
        {
            lock_guard<mutex> lg(m);
            fired = true;
            toCall = callbacks;
        }
        for (auto& f : toCall) f();   // call OUTSIDE lock
    }
};
```

Key rule: **never invoke callbacks while holding a lock** — deadlock risk if callback tries to register.
