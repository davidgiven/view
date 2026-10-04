Acornsoft VIEW
==============

# What?

This is a port of VIEW to the
[CP/M-65](https://github.com/davidgiven/cpm65). It'll run on any system with
about 20kb+ of memory and a SCREEN driver (which is not all of them). Also, a
CTRL key.

# How?

Copy the `VIEW.COM` from [the release
page](https://github.com/davidgiven/view/releases/tag/dev) onto the CP/M-65 disk
of your choice and run it!

Then, [read the manual](ViewGuide.pdf). You're unlikely to get anywhere without
it. Although, bear in mind that the keys of the CP/M version match those of
C-VIEW, not the original BBC Micro VIEW.

**Note!** You won't be able to load the example files. See the next section...

# Interoperability with the BBC Micro VIEW

So... the files aren't _quite_ compatible. BBC Micro VIEW uses the file size on
disk to determine, um, the size of the file. Unfortunately, CP/M doesen't have
byte-accurate file sizes; you can only get the size in units of 128 bytes. So
traditionally CP/M uses an end-of-file byte to mark the end of the actual data.
I've had to add this to the CP/M version of VIEW.

To convert from BBC Micro VIEW to CP/M VIEW: add a `\0` byte to the end of the
file.

To convert from CP/M VIEW to BBC Micro VIEW: truncate the file at the first `\0`
in the file (it'll be in the last 128 bytes).

# Interoperability with other programs

VIEW files are `\r` terminated text files with embedded control codes (`mac` format).

`\[` (escape, 0x1b) indicates padding spaces used for justification, and can be ignored.

0x1c and 0x1d indicate toggling of highlight 1 and highlight 2, respectively.

A 0x80 at the beginning of a line indicates that the line has a command. The next two characters are the command; the line proper starts immediately afterwards.

A 0x81 at the beginning of a line indicates that the line is a ruler. There are then two 0x23 (`'.'`) characters before the ruler starts.

Data ends at a `\0`.

# Who?

VIEW was written by Mark Colton, a British software developer and racing driver,
who was responsible for the development of the entire VIEW family, which was
later developed into View Professional on the BBC Micro which in turn became, on
the Acorn Archimedes family of computers, the Pipedream and Fireworkz combined
word processor/spreadsheets. (All now [open source on
Github](https://github.com/skswales)).

He was killed in a racing accident in 1995.

The decompilation is owned by me, David Given, and I hereby declare that all my
work is CC0 licensed. You may contact me at <dg@cowlark.com>, or visit my website
at <http://www.cowlark.com>.  There may or may not be anything interesting there.

# License

The copyright holder for VIEW is lost to time. Acornsoft was folded into Acorn
Computers, whose assets were then passed around from company to company until
nobody (not even the owner) knows who the owner is. That means that this code is
not actually legally entitled to be here; but, on the other hand, that means
you're unlikely to be sued for using it.

If anyone finds out they own this, please let me know and I'll remove it. Also,
tell people who you are!

[_The Water Horse's Fireplace_] (<https://cowlark.com/2012-10-31-water-horse>) is,
however, written by me and is available under the [Creative Commons BY-NC-ND (by
attribution, non-commercial, no derivatives)
license](https://creativecommons.org/licenses/by-nc-nd/4.0/).
