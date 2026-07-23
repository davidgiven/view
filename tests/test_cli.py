#!/usr/bin/env python3
"""CLI integration tests for VIEW (command-line mode)."""

import os
import unittest
import filecmp

import re
from utils import *
from process import PtyProcess


class CliTests(unittest.TestCase):

    def setUp(self):
        self.proc = PtyProcess([VIEW_BIN])

    def tearDown(self):
        self.proc.close()

    def test_prompt_is_seen(self):
        output = self.proc.read_until(b"=>", timeout=0.5)
        self.assertTrue(
            output.endswith(b"=>"),
            f"Expected prompt '=>' at end of output, got: {repr(output[-40:])}",
        )

    def test_quit_command(self):
        self.proc.read_until(b"=>", timeout=0.5)
        self.proc.writeline("BYE")
        status, output = self.proc.wait(timeout=1.0)
        self.assertTrue(
            os.WIFEXITED(status),
            f"Process did not exit cleanly (status={status})",
        )
        self.assertEqual(
            os.WEXITSTATUS(status),
            0,
            f"Expected exit code 0, got {os.WEXITSTATUS(status)}",
        )

    def test_load_missing_file(self):
        self.proc.read_until(b"=>", timeout=0.5)
        self.proc.writeline("load missing.v")
        output = self.proc.read_until(b"=>", timeout=0.5)
        self.assertIn(
            b"File not found",
            output,
            f"Expected 'File not found' in output, got: {repr(output)}",
        )
        self.assertTrue(
            output.endswith(b"=>"),
            f"Expected output to end with prompt, got: {repr(output[-40:])}",
        )

    def test_load_existing_file(self):
        self.proc.read_until(b"=>", timeout=0.5)
        self.proc.writeline("load examples/horse.v")
        output = self.proc.read_until(b"=>", timeout=0.5)
        self.assertIn(
            b"examples/horse.v",
            output,
            f"Expected filename in output, got: {repr(output)}",
        )
        self.assertTrue(
            output.endswith(b"=>"),
            f"Expected output to end with prompt, got: {repr(output[-40:])}",
        )

    def test_save_simple(self):
        """Load a file and save it, then verify the saved file is byte-identical."""
        here = os.path.dirname(__file__)
        original = os.path.join(here, "..", "examples", "horse.v")

        self.proc.read_until(b"=>", timeout=0.5)
        self.proc.writeline("LOAD examples/horse.v")
        output = self.proc.read_until(b"=>", timeout=1.0)
        self.assertIn(b"examples/horse.v", output)

        self.proc.writeline("SAVE output.v")
        self.proc.read_until(b"=>", timeout=1.0)

        self.proc.writeline("BYE")
        status, _ = self.proc.wait(timeout=1.0)
        self.assertTrue(os.WIFEXITED(status))
        self.assertEqual(os.WEXITSTATUS(status), 0)

        self.assertTrue(
            os.path.exists("output.v"), "Saved file output.v does not exist"
        )
        self.assertTrue(
            filecmp.cmp(original, "output.v", shallow=False),
            "Saved file content differs from original",
        )
        os.unlink("output.v")

    def test_count_words(self):
        """Load a file and count words, verify the count is correct."""
        self.proc.read_until(b"=>", timeout=0.5)
        self.proc.writeline("LOAD examples/horse.v")
        self.proc.read_until(b"=>", timeout=1.0)

        self.proc.writeline("COUNT")
        output = self.proc.read_until(b"=>", timeout=1.0)
        self.assertIn(
            b"1730",
            output,
            f"Expected word count 1730 in output, got: {repr(output)}",
        )
        self.assertIn(
            b"word(s) counted",
            output,
            f"Expected 'word(s) counted' in output, got: {repr(output)}",
        )

    def test_new_clears_document(self):
        """Load a file, COUNT, NEW, COUNT — verify document is cleared."""
        self.proc.read_until(b"=>", timeout=0.5)
        self.proc.writeline("LOAD examples/horse.v")
        self.proc.read_until(b"=>", timeout=1.0)

        self.proc.writeline("COUNT")
        output = self.proc.read_until(b"=>", timeout=1.0)
        self.assertIn(
            b"1730",
            output,
            f"Expected 1730 word(s) before NEW, got: {repr(output)}",
        )
        self.assertIn(
            b"word(s) counted",
            output,
        )

        self.proc.writeline("NEW")
        self.proc.read_until(b"=>", timeout=1.0)

        self.proc.writeline("COUNT")
        output = self.proc.read_until(b"=>", timeout=1.0)
        self.assertIn(
            b"0 word(s) counted",
            output,
            f"Expected 0 word(s) after NEW, got: {repr(output)}",
        )

    def test_screen_shows_lines(self):
        """Load a file and run SCREEN, then verify the first ten output lines."""
        self.proc.read_until(b"=>", timeout=0.5)
        self.proc.writeline("LOAD examples/horse.v")
        self.proc.read_until(b"=>", timeout=1.0)

        self.proc.writeline("SCREEN")
        output = self.proc.read_until(b"=>", timeout=3.0)

        # Strip the command echo (SCREEN\r\n or SCREEN\n) and trailing prompt
        start = output.find(b"SCREEN")
        if start >= 0:
            output = output[start + 6:].lstrip(b"\r\n")
        prompt_pos = output.rfind(b"=>")
        if prompt_pos >= 0:
            output = output[:prompt_pos]

        # Split on NL to get lines; take first 10 non-empty, stripping trailing CR
        lines = [l.rstrip(b"\r") for l in output.split(b"\n") if l.strip()]
        first_ten = lines[:10]

        expected = [
            b"                       The Water Horse's Fireplace",
            b'                 a Scottish Halloween folk story, sort of',
            b'                           (c) 2012 David Given',
            b'The  *each-uisge*  of  Scotland, pronounced  *echh-ush-guh*,  is  one  of  the',
            b"country's traditional monsters. While the relatively harmless kelpie lives",
            b"in running water, the  *each-uisge* ('water horse') lives in lochs. They eat",
            b'meat, human for preference, and  will  use  their  shapeshifting powers to',
            b"lure their prey down to the water's edge where  they will be dragged under",
            b'and consumed, leaving only their livers to float to the  shore  as  a sign',
            b'that  the  water  horse has taken another victim. Water horses are vicious',
        ]

        self.assertEqual(first_ten, expected)


if __name__ == "__main__":
    unittest.main()
