# Agent notes for the View word processor

This is an in-progress translation of the Acornsoft View word processor
for the BBC Micro from 6502 machine code into C.

## Formatting

Run `clang-format -i <file>` after every file edit. The project
root has a `.clang-format` config.

## Code conventions

- Use Javadoc-style (`/** ... */`) comments above functions, with `@param`
  / `@return` tags where relevant.
- Whenever making changes to a function which has a Javadoc comment, check
  to see whether the comment needs updating, and do so if required.

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

## Key files

| File | Purpose |
|---|---|
| `src/view.c` | Main application logic (6502 translation) |
| `src/cli_stdio.c` | CLI stdio input/output |
| `src/screen_ncurses.c` | ncurses backend for putchar/getchar/clear |
| `tests/interact.py` | Integration tests |
| `FORMAT.md` | Document file format (.v) |
| `CALLGRAPH.md` | Auto-generated call graph of `view.c` — update when editing call sites |
