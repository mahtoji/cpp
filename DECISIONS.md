# Design Decisions

## Problem 2 — Distributed File Processor

| Decision | Choice | Reason |
|---|---|---|
| File storage | Single 1TB S3 object | More general; use byte-range reads like Athena |
| Work partitioning | Master hands out byte ranges to workers | Simple coordination, central control |
| Master coordination | byte-range reads via S3 Range header | Avoids downloading full file |
| Master/Standby election | Lower node ID = Master when both up | Simple, deterministic, no split brain on startup |
| Shared state storage | S3 object | Accessible to both master and standby without direct communication |
| State tracked | Completed ranges, assigned ranges, heartbeats | Enough for standby to take over cleanly |
| Leader election protocol | Simple primary/standby (RAFT dropped) | RAFT needs 3+ nodes for quorum; keeping it simple for now |

## Open / Unresolved (Problem 2)
- Split brain: how to prevent both nodes promoting to master simultaneously
- Heartbeat interval and failover timeout tuning
- How often master flushes state to S3 and what happens on crash between flushes

## Problem 4 — Thread Pool (design decisions TBD — user hasn't implemented yet)
Key decisions the user will need to make:
- Task queue type: reuse bounded-queue or use `std::queue` with mutex+cv directly?
- How to erase the return type: `std::function<void()>` wrapping a `packaged_task`
- Shutdown flag: `bool stop_` under the task queue mutex, checked alongside empty condition
- Worker loop exit condition: `stop_ && queue_.empty()` (drain before exit)
