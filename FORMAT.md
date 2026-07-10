# View Document (.v) File Format

This document describes the on-disk and in-memory format used by the
View word processor for Acorn BBC Micro, as implemented here.

## Overview

A `.v` file is a **plain binary** format with **no compression, no
encoding, and no magic number**.  The on-disk bytes are an exact
image of the document region in memory from `page` up to (but not
including) `top`.

| Property          | Value                               |
|-------------------|-------------------------------------|
| Line endings      | `0x0d` (CR) only — no `0x0a` (LF)  |
| File terminator   | `0x00` (null byte)                  |
| Character set     | 8-bit ASCII-compatible              |
| Reserved bytes    | `0x80` (format command), `0x81` (ruler) |
| Minimum document  | `0x0d 0x00` (empty line + null)     |

## Line Types

Each line in the document is a sequence of bytes terminated by
`0x0d`.  The first byte determines the line type:

### Text lines (first byte < `0x80`)

Normal text content.  Bytes are printable ASCII plus control
characters (see below).

### Format command lines (first byte `0x80`)

```
0x80  <cmd1>  <cmd2>  [arguments…]  0x0d
```

`<cmd1><cmd2>` is a two-letter mnemonic (uppercase).  Arguments
follow until the `0x0d` terminator.

| Mnemonic | Name              | Description                                  |
|----------|-------------------|----------------------------------------------|
| `CE`     | Center            | Centre the current line                      |
| `RJ`     | Right Justify     | Right-justify the current line               |
| `DF`     | Define Footer     | Set footer text                              |
| `DH`     | Define Header     | Set header text                              |
| `DM`     | Define Marker     | Set a marker position                        |
| `EM`     | Evaluate          | Evaluate expression into register            |
| `PE`     | Page Eject        | Force a page break at this point             |
| `TM`     | Top Margin        | Set top margin (lines)                       |
| `BM`     | Bottom Margin     | Set bottom margin (lines)                    |
| `PL`     | Page Length       | Set page length (lines)                      |
| `TS`     | Two-Sided         | Enable/disable two-sided printing            |
| `FO`     | Fold              | Enable/disable case folding                  |
| `HE`     | Header            | Enable/disable header printing               |
| `HT`     | Header Text       | Set header text (alternative to `DH`)        |
| `HM`     | Header Margin     | Set header margin (lines)                    |
| `FM`     | Footer Margin     | Set footer margin (lines)                    |
| `LM`     | Left Margin       | Set left margin (columns)                    |
| `LS`     | Line Spacing       | Set line spacing (1 or 2)                    |
| `OP`     | Odd Page          | Odd-page formatting overrides                |
| `EP`     | Even Page         | Even-page formatting overrides               |
| `LJ`     | Line Justify      | Enable/disable line justification            |
| `PB`     | Page Break        | Insert a page break                          |

Argument types are parsed by context: decimal numbers, register
references (`|` + letter), boolean (`ON` / `OFF` / `1` / `0`), or
raw text.

### Ruler lines (first byte `0x81`)

```
0x81  <characters…>  0x0d
```

Each byte in the ruler body defines one screen column:

| Byte | Meaning                       |
|------|-------------------------------|
| `.`  | Empty column                  |
| `*`  | Tab stop                      |
| `>`  | Left margin                   |
| `<`  | Right margin                  |

Rulers nest via a stack.  When a ruler line is encountered during
document traversal the current ruler is saved, the new ruler is
loaded, and when the next text line finishes the previous ruler is
restored.

## Control Characters

Inside text lines the following bytes have special meaning:

| Byte | Name        | Rendering                              |
|------|-------------|----------------------------------------|
| `0x09` | TAB       | Advance to next tab stop               |
| `0x0b` | VT        | Indent to left margin                  |
| `0x0d` | CR        | Line terminator (not displayed)        |
| `0x10` | (padding) | Displayed as space; used as buffer fill|
| `0x1a` | SUB       | **Soft space** — displayed as space;   |
|       |             | inserted/removed during justification  |
| `0x1c` |           | **Highlight 1** toggle (e.g. italic)   |
| `0x1d` |           | **Highlight 2** toggle (e.g. bold)     |
| `0x7c` | `\|`      | Register reference prefix (commands)   |

All other bytes (`0x20`–`0x7e`) are printable ASCII.

## On Disk

Files are read and written as raw binary with no transformation.
Loading reads bytes with `fopen(…, "rb")` and `fgetc()`, storing
them one-by-one into RAM.  Saving writes bytes with
`fopen(…, "wb")` and `fputc()`, appending a final `0x00` null
terminator.  There is **no checksum, no header, no footer** beyond
the null terminator.

## In Memory

The document occupies a contiguous region of the 64 KB RAM from
`page` to `top`:

```
page ─→ [ line data … 0x0d ] [ line data … 0x0d ] … [ line data … 0x0d ] 0x00
                                                                          ^── top
```

A new / empty document contains just `0x0d 0x00`.

The editing line is staged into a 137-byte working buffer at `ptr1`.
When a line is loaded from the document (via `unpack_line_into_buffer`),
the first 3 bytes of the buffer hold the command-prefix overhead
(`0x80` + two command letters if applicable).  The renderable
content starts at offset 0 for text lines or offset 3 for command
lines.
