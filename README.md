Acornsoft VIEW
==============

# What?

VIEW is a word processor developed by Acorn for their BBC Micro series of
6502-based computers. It was most commonly found on the BBC Master, where it was
bundled on ROM alongside its companion spreadsheet, ViewSheet.

It's a basic but functional semi-WYSIWYG word processor supporting
justification, tabination, page formatting with headers and footers, multiple
rulers, editing of files bigger than will fit in memory (crudely),
search-and-replace, word count, text styles, and so on. It also features
loadable printer drivers so that it will work with a variety of printers.

This project contains a copy of the original ROM for version B3.0, a
mostly-symbolified disassembly of it using ZornsLemma's
[py8dis](https://github.com/ZornsLemma/py8dis]), and a port of it to my own
[CP/M-65](https://github.com/davidgiven/cpm65) operating system (which does also
run on the BBC Micro, for maximum recursion).

You might want to [read the manual](ViewGuide.pdf). You're unlikely to get
anywhere without it.

# How?

The disassembly is controlled by the `view.py` Python script, which contains
information about what symbols are defined where. (These names were made up
based on analysis of the code.) py8dis then uses this, plus the original ROM, to
emit an assembler file, `view-b3.0.asm`. This is then reassembled with
[beebasm](https://github.com/stardot/beebasm) to produce a bit-for-bit identical
ROM image. No attempt has been made to fix the bugs.

The same process was done to produce the CP/M-65 version, using
[llvm-mos](https://llvm-mos.org/wiki/Welcome) syntax, which was then heavily
hand-edited to change the OS interface layer to work with CP/M. This is possible
because VIEW was written to only use Acorn's MOS system interface, which is
highly abstract and hardware-independent.

See [README-CPM.md](README-CPM.md) for more information on the CP/M-65 version.


# Who?

VIEW was written by Mark Colton, a British software developer and racing driver,
who was responsible for the development of the entire VIEW family, which was
later developed into View Professional on the BBC Micro which in turn became, on
the Acorn Archimedes family of computers, the Pipedream and Fireworkz combined
word processor/spreadsheets. (All now [open source on
Github](https://github.com/skswales)).

He was killed in a racing accident in 1995.

The decompilation is owned by me, David Given, and I hereby declare that all my
work is CC0 licensed. You may contact me at dg@cowlark.com, or visit my website
at http://www.cowlark.com.  There may or may not be anything interesting there.

# License

The copyright holder for VIEW is lost to time. Acornsoft was folded into Acorn
Computers, whose assets were then passed around from company to company until
nobody (not even the owner) knows who the owner is. That means that this code is
not actually legally entitled to be here; but, on the other hand, that means
you're unlikely to be sued for using it.

If anyone finds out they own this, please let me know and I'll remove it. Also,
tell people who you are!