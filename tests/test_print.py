#!/usr/bin/env python3
"""Print-format tests for VIEW: enter editor, type text, SCREEN, verify."""

import unittest

from utils import *
from process import PtyProcess


class PrintTests(unittest.TestCase):

    def setUp(self):
        self.proc = PtyProcess([VIEW_BIN])

    def tearDown(self):
        self.proc.close()

    def test_type_text_and_screen(self):
        """Type 'test' in the editor, ESC, SCREEN, verify output."""
        self.proc.read_until(b"=>", timeout=0.5)
        self.proc.writeline("")
        self.proc.drain()

        self.proc.write(b"test\x1b")
        self.proc.read_until(b"=>", timeout=3.0)

        self.proc.writeline("SCREEN")
        output = self.proc.read_until(b"=>", timeout=3.0)

        # Strip the command echo and trailing prompt
        start = output.find(b"SCREEN")
        if start >= 0:
            output = output[start + 6:].lstrip(b"\r\n")
        prompt_pos = output.rfind(b"=>")
        if prompt_pos >= 0:
            output = output[:prompt_pos]

        # Split on NL; take first non-empty line
        lines = [l.rstrip(b"\r") for l in output.split(b"\n") if l.strip()]

        self.assertGreaterEqual(len(lines), 1)
        self.assertEqual(lines[0], b"test")


if __name__ == "__main__":
    unittest.main()
