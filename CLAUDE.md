Read CONTEXT.md before starting any work.

# Token Usage — Active Context Policy
- Only use the last 3 exchanges as live context.
- Do NOT pre-load CONTEXT.md or NOTES.md every turn — load them only when a question requires prior history.
- When the user says "new problem, starting fresh", treat it as a clean slate.
- CONTEXT.md and NOTES.md are the source of truth for prior work — reach for them on demand, not proactively.
- **Exception: the 10-minute snapshot rule still applies** — keep writing to CONTEXT.md and NOTES.md regularly so information is never lost and is available on demand.

# Every 10 Minutes — Periodic Tasks
During active work, every 10 minutes do ALL of the following:
1. Save a full snapshot across all three context files (see Context Preservation below).
2. Check battery level: `cat /sys/class/power_supply/BAT0/capacity` — if below 5%, warn the user immediately with a prominent message.

# Context Preservation
- Every 10 minutes during active work, and on every clean exit (user says goodbye/done/exit), save a full snapshot across all three context files:
  - **CONTEXT.md** — current problem(s), status, and next steps / what to do when resuming
  - **DECISIONS.md** — all design decisions made, alternatives considered, and why they were rejected
  - **NOTES.md** — detailed implementation notes: data structures, algorithms, concurrency choices, protocols, bugs found and fixed (with explanations), and open/unresolved questions
- The goal is that any future session reading these three files can fully resume without any loss of context.
- On every clean exit, also overwrite `claude.resume.sh` with the current session's resume command: `claude --resume <current-session-id>`, so the user can run `./claude.resume.sh` to resume this exact session.
