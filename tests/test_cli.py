#!/usr/bin/env python3
"""CLI integration tests for VIEW (command-line mode)."""

import os
import unittest
import filecmp

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
            b"1731",
            output,
            f"Expected word count 1731 in output, got: {repr(output)}",
        )
        self.assertIn(
            b"word(s) counted",
            output,
            f"Expected 'word(s) counted' in output, got: {repr(output)}",
        )


if __name__ == "__main__":
    unittest.main()
