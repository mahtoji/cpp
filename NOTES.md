# Detailed Notes

## C++ Concepts Covered

### Initialization: `{}` vs `()`
- `{}` brace init — prevents narrowing, works on all types, modern C++11+
- `()` paren init — classic, allows narrowing
- Exception: use `()` for std containers when specifying size (`vector<int> v(3)` = 3 elements, `vector<int> v{3}` = 1 element)

### Smart Pointers
- `unique_ptr` — single owner, move-only, zero overhead. Use `make_unique`.
- `shared_ptr` — multiple owners, reference counted, freely copyable. Use `make_shared` (one allocation vs two).
- `weak_ptr` — non-owning, breaks circular references. Must `.lock()` to access.
- Raw `new/delete` — avoid; leaks on exception, manual lifetime management.

### shared_mutex + shared_lock
- `shared_lock` for readers (multiple simultaneous)
- `unique_lock` for writers (exclusive)
- Requires `shared_mutex`, not plain `mutex`

### Condition Variables
- Use `unique_lock` (not `lock_guard`) — `cv.wait()` needs to unlock/relock
- Always use predicate lambda to guard against spurious wakeups
- `notify_one` wakes one waiter, `notify_all` wakes all

### Semaphores
- `binary_semaphore` = `counting_semaphore<1>`
- `counting_semaphore<N>` — limits concurrency to N threads
- Unlike mutex, `release()` can be called from a different thread than `acquire()`

### std::future / std::packaged_task (needed for Problem 4)
- `packaged_task<R()>` wraps a callable; calling it stores the result/exception
- `get_future()` returns the associated `std::future<R>` — call BEFORE moving the task
- `std::future<R>::get()` blocks until result is ready, rethrows stored exception
- Type erasure trick: store `packaged_task` inside `std::function<void()>` lambda

### std::invoke_result_t (needed for Problem 4)
- `std::invoke_result_t<F, Args...>` — deduces return type of `F(Args...)`
- Used in `submit()` return type: `std::future<std::invoke_result_t<F, Args...>>`

---

## mr2.cpp — Bug History

All bugs found and fixed across multiple review rounds:

1. `getOffset()` used undefined `offset`/`nextoffset` — should be `nextOffset`
2. Threads opened their own local `fstream` but design needed shared handle
3. Operator precedence: `nextOffset = getOffset() < fileSize` assigned bool — needed `(nextOffset = getOffset()) < fileSize`
4. `nextOffset` read outside mutex — fixed with local `myoffset`
5. `s.clear()` called before `CountErrors` — counted empty string
6. `s` not trimmed to actual bytes read — fixed with `resize(f.gcount())` + `resize(READSIZE)`
7. `vt.join()` — vector has no `join()`, needed `vt[i].join()`
8. Unnamed `fstream` in `main` — temporary destroyed immediately
9. Missing `#include` headers: `<fstream>`, `<vector>`, `<mutex>`, `<numeric>`, `<string>`

---

## Problem 2 — Distributed Design Details

### Architecture
- Single 1TB file on S3
- S3 byte-range reads allow parallel access without splitting file
- Master node assigns byte ranges to workers
- Master/Standby pair for fault tolerance
- Shared state in S3: assigned ranges, completed ranges, heartbeats

### Election
- Both nodes write heartbeat to S3 and read the other's heartbeat
- Lower node ID = Master when both alive
- Sole surviving node promotes itself to Master

### Election — detailed
- Both nodes start with random backoff before posting heartbeat
- First up sees no master in shared state, writes its ID as master
- Second up sees master ID already set, accepts slave role
- Before every master operation, node checks shared state confirms it is still master
- Split brain: both may briefly think they are master, but pre-operation check resolves it — S3 last write wins, loser self-corrects to slave

### Master/Standby alternatives considered
| Option | Tradeoff |
|---|---|
| 3-node RAFT, no shared storage | Most robust, but complex |
| 2-node active-standby, no shared storage | Simpler, needs direct node communication |
| 2-node active-standby + S3 shared state | Simple, decoupled, S3 lacks atomic ops |
| External coordinator (etcd/ZooKeeper) | Clean atomic ops, production-grade, adds dependency. etcd IS RAFT — just pre-built. |

### Worker Design
- Identical nodes, each given offset + fixed slice size
- Worker fetches byte range from S3, counts "Error", reports count to master
- Master writes count to shared S3 state keyed by offset
- Master acknowledges to worker; worker then requests next slice
- On no-ack: worker retries — idempotent, safe

### Boundary Problem (same as mr2.cpp)
- Every worker except offset 0 reads (offset-4) and slice size (1024+4)
- Overlap catches any "Error" straddling slice boundary
- Previous worker would have missed it; this worker catches it in overlap region

### Fault Tolerance — Worker Failures
- Timer starts after last slice is handed out
- On timer expiry, master sweeps all slices, reassigns incomplete ones
- Repeats until all slices complete or retry limit (e.g. 10-20) reached
- If master dies mid-wait, standby takes over, reads state from S3, continues sweep
- Better alternative: per-slice timeout rather than global timer

### Aggregation
- Each worker reports count for its offset+slice to master
- Master writes to S3 keyed by offset
- Once all slices complete, master sums all counts, writes final total to S3
- Master death during summation: standby redoes summation from S3 — cheap and idempotent

### Result Verification
- S3 byte-range checksums — always do this, cheap, catches data corruption
- Re-run with different slice sizes — elegant, specifically targets boundary bugs
- Spot checks mid-job — pragmatic when full re-run too expensive
- Known test data — standard SW validation, catches logic bugs before production

---

## Interview Assessment

### Strengths
- Structured thinking — identified divide & conquer, fault tolerance, coordination upfront
- Pragmatic tradeoffs — accepted duplicate work on failover, bounded retries, chose simple master/standby over RAFT
- Connected distributed design back to coding problem (boundary overlap)
- Idempotency as a correctness tool — used consistently throughout

### Areas to sharpen
- Mention MapReduce/real-world analogues proactively — naming them signals breadth
- Call out S3 eventual consistency / lack of atomic ops earlier — shows deeper AWS knowledge
