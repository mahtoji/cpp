# Session Context

## What we are doing
Mock technical interview practice. Currently focused on **Pure Storage online HackerRank test**.
Previous target was Dell (Senior Principal Engineer, Distributed Systems) — that practice is still relevant.

## Quick Reference Files
- **locks.cpp** — canonical examples of lock_guard, unique_lock, shared_lock, defer_lock, condition variables, producer-consumer pattern. Quick reference for interview.
- **bounded-queue.cpp** — thread-safe bounded queue with atomic, mutex, cv, jthread. Full working example.

## Completed Problems

### Problem 1 — Multi-threaded Log File Processor (DONE)
Implement a multi-threaded C++ program to read a large log file and count frequency of "Error" keyword.
Solution is in `mr2.cpp`. All major bugs have been fixed. See NOTES.md for full bug list.

### Problem 2 — Bounded Queue (DONE)
Implement a thread-safe bounded queue in C++.
Solution is in `bounded-queue.cpp`.
Key concepts: mutex, condition_variable, RAII locks, predicate lambdas for spurious wakeup protection.

### Problem 3 — Distributed File Processor System Design (DONE)
Scale Problem 1 to a 1TB file across multiple machines.
System design discussion (not a coding problem). See NOTES.md for full design.

## Current / Next Problem

### Problem 4 — Thread Pool (NOT STARTED — resume here)
Implement a `ThreadPool` class in C++ with the following interface:

```cpp
class ThreadPool {
public:
    explicit ThreadPool(size_t num_threads);
    ~ThreadPool();   // must drain all pending tasks, then join

    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args)
        -> std::future<std::invoke_result_t<F, Args...>>;

    void shutdown();  // stop accepting new; finish queued tasks
};
```

**Constraints:**
1. `num_threads` workers created at construction, alive until destruction.
2. Tasks execute in FIFO order.
3. `submit()` is non-blocking; returns `std::future` immediately.
4. `shutdown()` is idempotent; `submit()` throws after it is called.
5. Destructor must NOT drop enqueued tasks — all must complete before join.
6. Must be free of data races (no UB under TSan).

**Follow-up questions to ask after working solution:**
- Priority queue so high-priority I/O tasks preempt low-priority ones?
- Per-worker work-stealing to avoid one overloaded thread?
- What if a task throws — how should the pool handle it?
- Metrics: queue depth, tasks completed, average wait latency?

**File to create:** `thread-pool.cpp`

## General Instructions
- When updating any file, just do it silently — no confirmation, no displaying changes.
- Update CONTEXT.md, DECISIONS.md, NOTES.md, and GUIDE.md every 10 minutes during active work and on every clean exit.
- Keep collecting terse, concise notes for last-minute interview review inside GUIDE.md.
- Update GUIDE.md every 10 minutes and on exit. Selection criteria for what to add:
  - Is it a reusable pattern that appears across problems?
  - Would forgetting it in an interview hurt you?
  - Is it something you can't quickly re-derive on the spot?
  - Do NOT add: one-off problem-specific details, minor syntax fixes, things easily re-derived.

## Code Review Style (apply to all future evaluations)
- High level only — no nitpicks, no performance comments unless asked
- Lead with the count: "Three core issues" etc.
- Each issue: bold title with line number, one-line explanation of what's wrong, one-line explanation of why it matters or what it should be
- End with one sentence acknowledging what's structurally correct
- Do NOT suggest rewrites of correct code

## Next Steps on Resume
1. User implements `thread-pool.cpp`
2. Review solution, discuss follow-up questions above
3. Continue with more problems relevant to the Dell role
