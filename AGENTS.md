# Agent notes for the View word processor

This is an in-progress translation of the Acornsoft View word processor
for the BBC Micro from 6502 machine code into C.  The original 6502
assembly is preserved as comments above each translated block and is
the authoritative reference.  **Never modify the 6502 assembly
comments** — they are needed for correctness verification.

## Code conventions

- Global `uint8_t` registers `a`, `x`, `y`, `sp`, `flags` simulate the
  6502 processor state.
- The `flags` variable packs processor status bits:
  `FLAG_C=0x01`, `FLAG_Z=0x02`, `FLAG_N=0x80`.
- The `ram[65536]` array simulates the 64 KB address space.
- Every `lda`, `ldx`, `ldy` instruction in 6502 sets Z and N flags.
  The C translation often omits the flag update; if a subsequent
  branch depends on Z/N, you must add an explicit flags assignment
  after the variable assignment.
- `cmp` is translated as a macro-like expression that updates `flags`.
- `bcc`/`bcs`/`beq`/`bne`/`bmi`/`bpl` read `flags` to decide the
  branch.

## Build and test

```
make test          # builds bin/view and runs all tests
make -j4 bin/view  # compile only
```

The test runner renders a unit test (`bin/render_number`) then runs
integration tests under `TERM=vt100`:

```
TERM=vt100 python3 tests/interact.py
```

## Test infrastructure

`tests/interact.py` uses a PTY (`PtyProcess` class) to drive
`bin/view` as a child process.  Tests send CLI commands and read the
PTY output.  `pyte` decodes VT100 escape sequences into a screen
buffer (`Screen.display[row]` gives the 80-character string for each
screen row).

## Editor screen layout

- Row 0: status bar (drawn by `display_status_word` + `sub_ca651`)
- Row 1+: document content

## Key files

| File | Purpose |
|---|---|
| `src/view.c` | Main application logic (6502 translation) |
| `src/cli_stdio.c` | CLI stdio input/output |
| `src/screen_ncurses.c` | ncurses backend for putchar/getchar/clear |
| `tests/interact.py` | Integration tests |
| `FORMAT.md` | Document file format (.v) |
| `CALLGRAPH.md` | Auto-generated call graph of `view.c` — update when editing call sites |
