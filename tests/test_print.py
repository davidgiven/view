#!/usr/bin/env python3
"""Print-format tests for VIEW: enter editor, type text, SCREEN, verify."""

import unittest

from utils import *
from process import PtyProcess


def _command(name, arg):
    """Build the key sequence to enter a format command in the editor.
    name — two-letter format command (e.g. "CE")
    arg  — the text that follows the command.
    """
    return CTRL_O + b"C" + name.encode("ascii") + CTRL_M + arg.encode("ascii")


class PrintTests(unittest.TestCase):

    def setUp(self):
        self.proc = PtyProcess([VIEW_BIN])

    def tearDown(self):
        self.proc.close()

    def _type_and_screen(self, keys_to_type, expected_lines):
        """Type keys into the editor, exit with Escape, SCREEN, compare output."""
        self.proc.read_until(b"=>", timeout=0.5)
        self.proc.writeline("")
        self.proc.drain()

        self.proc.write(keys_to_type + b"\x1b")
        self.proc.read_until(b"=>", timeout=3.0)

        self.proc.writeline("SCREEN")
        output = self.proc.read_until(b"=>", timeout=3.0)

        start = output.find(b"SCREEN")
        if start >= 0:
            output = output[start + 6:].lstrip(b"\r\n")
        prompt_pos = output.rfind(b"=>")
        if prompt_pos >= 0:
            output = output[:prompt_pos]

        lines = [l.rstrip(b"\r") for l in output.split(b"\n") if l.strip()]
        self.assertEqual(lines, [s.encode("ascii") for s in expected_lines])

    def test_type_text_and_screen(self):
        """Type 'test' in the editor, ESC, SCREEN, verify output."""
        self._type_and_screen(b"test", ["test"])

    def test_ce_centers_text(self):
        """Type CE format command + text, SCREEN should show centered output."""
        self._type_and_screen(
            _command("CE", "Hello"),
            ["                                  Hello"],
        )

    def test_lj_left_justifies_text(self):
        """Type LJ format command + text, SCREEN should show left-justified output."""
        self._type_and_screen(
            _command("LJ", "Hello"),
            ["Hello"],
        )

    def test_rj_right_justifies_text(self):
        """Type RJ format command + text, SCREEN should show right-justified output."""
        self._type_and_screen(
            _command("RJ", "Hello"),
            ["                                                                     Hello"],
        )

    def test_lj_with_indented_ruler(self):
        """Set ruler left stop (>) then LJ text."""
        keys = CTRL_O + CTRL_S + b"     >" + CTRL_M + _command("LJ", "Hello")
        self._type_and_screen(keys, ["Hello"])

    def test_lj_with_left_margin(self):
        """Set LM indentation, then LJ text — output indented accordingly."""
        keys = _command("LM", "10") + CTRL_M + _command("LJ", "Hello")
        self._type_and_screen(keys, ["          Hello"])


if __name__ == "__main__":
    unittest.main()
