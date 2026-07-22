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

    def _type_and_screen(self, keys_to_type, expected_lines, keep_empty=False):
        """Type keys into the editor, exit with Escape, SCREEN, compare output.
        If keep_empty is True, empty lines are included in the comparison.
        """
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

        if keep_empty:
            lines = [l.rstrip(b"\r") for l in output.split(b"\n")]
            # strip trailing empty lines
            while lines and lines[-1] == b"":
                lines.pop()
        else:
            lines = [l.rstrip(b"\r") for l in output.split(b"\n") if l.strip()]
        expected = [s.encode("ascii") if isinstance(s, str) else s for s in expected_lines]
        self.assertEqual(lines, expected)

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

    def test_ls_doubles_line_spacing(self):
        """Type two lines, set LS 2, type two more — verify extra blank line."""
        keys = b"123" + CTRL_M + b"456" + CTRL_M
        keys += _command("LS", "2") + CTRL_M
        keys += b"123" + CTRL_M + b"456"
        self._type_and_screen(
            keys,
            ["123", "456", "123", "", "", "456"],
            keep_empty=True,
        )

    def _page_layout_setup(self):
        """Return keys to set up a small test page (PL 5, TM 1, HM 1, BM 1, FM 1)."""
        keys = _command("TM", "1") + CTRL_M
        keys += _command("DH", "/lefth/middleh/righth/") + CTRL_M
        keys += _command("PL", "5") + CTRL_M
        keys += _command("DF", "/leftf/middlef/rightf/") + CTRL_M
        keys += _command("HM", "1") + CTRL_M
        keys += _command("BM", "1") + CTRL_M
        keys += _command("FM", "1") + CTRL_M
        return keys

    def test_page_layout(self):
        """Set TM, DH, PL, DF, HM, BM, FM and verify every output line."""
        keys = self._page_layout_setup()
        keys += _command("LJ", "Body")
        self._type_and_screen(
            keys,
            [b"lefth                            middleh                            righth",
             b"", b"Body", b"",
             b"leftf                            middlef                            rightf"],
            keep_empty=True,
        )

    def _page_eject_test(self, cmd, expected_lines):
        """Run a page-eject test for the given format command with full expected output."""
        keys = self._page_layout_setup()
        keys += b"A" + CTRL_M
        keys += _command(cmd, "") + CTRL_M
        keys += b"B"
        self._type_and_screen(keys, expected_lines, keep_empty=True)

    def test_pe_page_eject(self):
        """Insert PE (page eject) and verify it breaks between pages."""
        self._page_eject_test("PE", [
            b"lefth                            middleh                            righth",
            b"", b"A", b"",
            b"leftf                            middlef                            rightf",
            b"", b"",
            b"lefth                            middleh                            righth",
            b"", b"", b"",
            b"leftf                            middlef                            rightf",
            b"", b"",
            b"lefth                            middleh                            righth",
            b"", b"B", b"",
            b"leftf                            middlef                            rightf",
        ])

    def test_op_odd_page_eject(self):
        """Insert OP (odd page eject) — B goes on the next odd page (3)."""
        self._page_eject_test("OP", [
            b"lefth                            middleh                            righth",
            b"", b"A", b"",
            b"leftf                            middlef                            rightf",
            b"", b"",
            b"lefth                            middleh                            righth",
            b"", b"", b"",
            b"leftf                            middlef                            rightf",
            b"", b"",
            b"lefth                            middleh                            righth",
            b"", b"B", b"",
            b"leftf                            middlef                            rightf",
        ])

    def test_ep_even_page_eject(self):
        """Insert EP (even page eject) — B goes on the next even page (4)."""
        self._page_eject_test("EP", [
            b"lefth                            middleh                            righth",
            b"", b"A", b"",
            b"leftf                            middlef                            rightf",
            b"", b"",
            b"lefth                            middleh                            righth",
            b"", b"", b"",
            b"leftf                            middlef                            rightf",
            b"", b"",
            b"lefth                            middleh                            righth",
            b"", b"", b"",
            b"leftf                            middlef                            rightf",
            b"", b"",
            b"lefth                            middleh                            righth",
            b"", b"B", b"",
            b"leftf                            middlef                            rightf",
        ])


if __name__ == "__main__":
    unittest.main()
