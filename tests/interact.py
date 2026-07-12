#!/usr/bin/env python3
"""Integration tests for the VIEW text editor via PTY."""

import pty
import os
import time
import select
import signal
import re
import struct
import fcntl
import termios
import unittest
import pyte

VIEW_BIN = os.path.join(os.path.dirname(__file__), "..", "bin", "view")


def strip_escapes(text: str) -> str:
    """Remove ANSI/VT100 escape sequences and control characters."""
    text = re.sub(r"\x1b[\[\(\)=][0-9;?]*[a-zA-Z]?", "", text)
    text = re.sub(r"\x1b.", "", text)
    text = re.sub(r"[\x00-\x08\x0b\x0c\x0e-\x1f\x7f]", "", text)
    return text.strip()


def screen_lines(output: bytes) -> list[str]:
    """Decode PTY output, strip escapes, and return a list of screen lines."""
    text = output.decode("latin-1")
    parts = re.split(r"\x1b\[\d+;\d+[Hf]|\x1b\[\d*[BADE]", text)
    lines = []
    for part in parts:
        for line in part.split("\r"):
            for sub in line.split("\n"):
                cleaned = strip_escapes(sub)
                if cleaned:
                    lines.append(cleaned)
    return lines


class PtyProcess:
    """Manage a child process running inside a PTY."""

    def __init__(self, argv, env=None, term="vt100"):
        env = env or {}
        full_env = {**os.environ, "TERM": term, **env}

        master_fd, slave_fd = pty.openpty()
        # Set window size so ncurses can initialise properly
        ws = struct.pack("HHHH", 24, 80, 0, 0)
        fcntl.ioctl(master_fd, termios.TIOCSWINSZ, ws)

        pid = os.fork()
        if pid == 0:
            os.close(master_fd)
            os.setsid()
            os.dup2(slave_fd, 0)
            os.dup2(slave_fd, 1)
            os.dup2(slave_fd, 2)
            if slave_fd > 2:
                os.close(slave_fd)
            os.execve(argv[0], argv, full_env)
            os._exit(1)

        os.close(slave_fd)
        self.master_fd = master_fd
        self.pid = pid
        self._buf = b""

    def close(self):
        if self.master_fd < 0:
            return
        os.close(self.master_fd)
        self.master_fd = -1
        try:
            os.kill(self.pid, signal.SIGKILL)
        except OSError:
            pass
        try:
            os.waitpid(self.pid, 0)
        except ChildProcessError:
            pass

    def read(self, timeout=5.0):
        """Read available data, waiting up to *timeout* seconds."""
        deadline = time.time() + timeout
        r, _, _ = select.select([self.master_fd], [], [], timeout)
        if not r:
            return b""
        try:
            data = os.read(self.master_fd, 4096)
            self._buf += data
            return data
        except OSError:
            return b""

    def read_until(self, pattern, timeout=5.0):
        """Read until *pattern* (bytes) appears in the buffer, or timeout."""
        deadline = time.time() + timeout
        while time.time() < deadline:
            if pattern in self._buf:
                break
            remaining = deadline - time.time()
            if remaining <= 0:
                break
            self.read(timeout=remaining)
        if pattern in self._buf:
            idx = self._buf.index(pattern) + len(pattern)
            data = self._buf[:idx]
            self._buf = self._buf[idx:]
            return data
        data = self._buf
        self._buf = b""
        return data

    def write(self, data):
        os.write(self.master_fd, data)

    def writeline(self, line):
        self.write((line + "\n").encode("ascii"))

    def wait(self, timeout=5.0):
        """Wait for the child to exit and return (exit_status, output)."""
        deadline = time.time() + timeout
        while time.time() < deadline:
            try:
                data = os.read(self.master_fd, 4096)
                if data:
                    self._buf += data
                else:
                    break
            except OSError:
                break
            remaining = deadline - time.time()
            if remaining <= 0:
                break
            r, _, _ = select.select([self.master_fd], [], [], min(remaining, 0.1))
        pid, status = os.waitpid(self.pid, os.WNOHANG)
        if pid == 0:
            try:
                pid, status = os.waitpid(self.pid, 0)
            except ChildProcessError:
                status = 0
        output = self._buf
        self._buf = b""
        return status, output

    def drain(self, timeout=0.3):
        """Read any remaining buffered output."""
        time.sleep(0.1)
        while True:
            r, _, _ = select.select([self.master_fd], [], [], timeout)
            if not r:
                break
            try:
                data = os.read(self.master_fd, 4096)
                if not data:
                    break
                self._buf += data
            except OSError:
                break


class CliTests(unittest.TestCase):

    def setUp(self):
        self.proc = PtyProcess([VIEW_BIN])

    def tearDown(self):
        self.proc.close()

    def test_prompt_is_seen(self):
        output = self.proc.read_until(b"=>", timeout=0.5)
        self.assertTrue(
            output.endswith(b"=>"),
            f"Expected prompt '=>' at end of output, got: {repr(output[-40:])}"
        )

    def test_quit_command(self):
        self.proc.read_until(b"=>", timeout=0.5)
        self.proc.writeline("BYE")
        status, output = self.proc.wait(timeout=1.0)
        self.assertTrue(os.WIFEXITED(status),
                        f"Process did not exit cleanly (status={status})")
        self.assertEqual(os.WEXITSTATUS(status), 0,
                         f"Expected exit code 0, got {os.WEXITSTATUS(status)}")

    def test_load_missing_file(self):
        self.proc.read_until(b"=>", timeout=0.5)
        self.proc.writeline("load missing.v")
        output = self.proc.read_until(b"=>", timeout=0.5)
        self.assertIn(b"File not found", output,
                      f"Expected 'File not found' in output, got: {repr(output)}")
        self.assertTrue(
            output.endswith(b"=>"),
            f"Expected output to end with prompt, got: {repr(output[-40:])}"
        )

    def test_load_existing_file(self):
        self.proc.read_until(b"=>", timeout=0.5)
        self.proc.writeline("load examples/horse.v")
        output = self.proc.read_until(b"=>", timeout=0.5)
        self.assertIn(b"examples/horse.v", output,
                      f"Expected filename in output, got: {repr(output)}")
        self.assertTrue(
            output.endswith(b"=>"),
            f"Expected output to end with prompt, got: {repr(output[-40:])}"
        )

    def test_save_simple(self):
        """Load a file and save it, then verify the saved file is byte-identical."""
        import filecmp
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

        self.assertTrue(os.path.exists("output.v"),
                        "Saved file output.v does not exist")
        self.assertTrue(filecmp.cmp(original, "output.v", shallow=False),
                        "Saved file content differs from original")
        os.unlink("output.v")

    def test_count_words(self):
        """Load a file and count words, verify the count is correct."""
        self.proc.read_until(b"=>", timeout=0.5)
        self.proc.writeline("LOAD examples/horse.v")
        self.proc.read_until(b"=>", timeout=1.0)

        self.proc.writeline("COUNT")
        output = self.proc.read_until(b"=>", timeout=1.0)
        self.assertIn(b"1731", output,
                      f"Expected word count 1731 in output, got: {repr(output)}")
        self.assertIn(b"word(s) counted", output,
                      f"Expected 'word(s) counted' in output, got: {repr(output)}")


class EditorTests(unittest.TestCase):

    def setUp(self):
        self.proc = PtyProcess([VIEW_BIN])

    def tearDown(self):
        self.proc.close()

    def _load_and_enter_editor(self, filename):
        """Load a file and enter editor mode. Returns (load_output, pyte Screen)."""
        self.proc.read_until(b"=>", timeout=0.5)
        self.proc.writeline(f"LOAD {filename}")
        load_output = self.proc.read_until(b"=>", timeout=1.0)
        self.proc.writeline("")
        time.sleep(0.3)
        raw = self.proc.read(timeout=0.5)
        screen = pyte.Screen(80, 24)
        stream = pyte.Stream(screen)
        stream.feed(raw.decode("latin-1"))
        return load_output, screen

    def test_enter_editor_after_loading_file(self):
        output, screen = self._load_and_enter_editor("examples/horse.v")
        self.assertIn(b"examples/horse.v", output,
                      f"Expected filename in LOAD output, got: {repr(output)}")
        self.assertIn(b"Editing examples/horse.v", output,
                      f"Expected 'Editing' line showing filename, got: {repr(output)}")
        expected_lines = [
            "MJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
            "CE The Water Horse's Fireplace                                                  ",
            "CE a Scottish Halloween folk story, sort of                                     ",
            "                                                                                ",
            "CE (c) 2012 David Given                                                         ",
            "                                                                                ",
            "   The  *each-uisge* of Scotland, pronounced  *echh-ush-guh*  ,  is  one  of  t ",
            "   country's traditional monsters. While the relatively harmless kelpie lives   ",
            "   in running  water, the *each-uisge* ('water horse') lives in lochs. They eat ",
            '   meat, human for  preference,  and  will  use their shapeshifting powers to   ',
            "   lure their prey down to the water's  edge where they will be dragged under   ",
            '   and consumed, leaving only their livers to float  to  the  shore as a sign   ',
            '   that  the  water  horse has taken another victim. Water horses are vicious   ',
            '   and  terrifying, far more  dangerous  than  many  of  the  other  Scottish   ',
            '   supernatural creatures,  and  the locals would always treat the appearance   ',
            "   of a lone animal  or man near the water's edge with caution, for fear that   ",
            '   the water horse might be hungry again. And rightly so.                       ',
            "                                                                                ",
            "   In a small loch in  north-west Scotland whose name I can't quite remember,   ",
            '   there once lived a water horse  and  his  wife. They preyed upon the local   ',
            '   crofters, for of course there were no fishermen ---  with  a pair of water   ',
            '   horses in the loch venturing into the water was far too dangerous. Luckily   ',
            '   for the locals, water horses do not get hungry very often.                   ',
            "                                                                                ",
        ]
        for i, expected in enumerate(expected_lines):
            self.assertEqual(
                expected, screen.display[i],
                f"Document line {i} mismatch:\n"
                f"  Expected: {repr(expected)}\n"
                f"  Got:      {repr(screen.display[i])}"
            )

    def _enter_editor_empty(self):
        """Enter editor mode without loading a file. Returns pyte Screen."""
        self.proc.read_until(b"=>", timeout=0.5)
        self.proc.writeline("")
        time.sleep(0.5)
        raw = self.proc.read(timeout=1.0)
        with open("/tmp/empty_final.bin", "wb") as f:
            f.write(raw)
        screen = pyte.Screen(80, 24)
        stream = pyte.Stream(screen)
        stream.feed(raw.decode("latin-1"))
        return screen

    def test_enter_editor_empty(self):
        screen = self._enter_editor_empty()
        expected = [
            "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
            "********************************************************************************",
            "                                                                                ",
        ]
        for i, exp in enumerate(expected):
            self.assertEqual(
                exp, screen.display[i],
                f"Row {i} mismatch: expected {repr(exp)}, got {repr(screen.display[i])}"
            )

    def test_enter_editor_and_type_q(self):
        """Enter empty editor, type 'q', then check the character appears."""
        self._enter_editor_empty()
        self.proc.write(b"q")
        time.sleep(0.5)
        raw = self.proc.read(timeout=2.0)
        # 'q' should appear directly in the raw output via screen_putchar
        self.assertIn(
            b"q", raw,
            f"Expected 'q' in raw output, got {len(raw)} bytes: {raw[-40:]}"
        )

    def test_enter_editor_after_loading_jabber(self):
        output, screen = self._load_and_enter_editor("examples/jabber.v")
        self.assertIn(b"jabber.v", output,
                      f"Expected filename in LOAD output, got: {repr(output)}")
        self.assertIn("She", screen.display[1],
                      f"Line 1 wrong: {repr(screen.display[1])}")
        self.assertIn("Looking-glass book", screen.display[2],
                      f"Line 2 wrong: {repr(screen.display[2])}")
        self.assertIn("go the right way again", screen.display[3],
                      f"Line 3 wrong: {repr(screen.display[3])}")
        # Line 4 is blank in the file
        self.assertEqual("", screen.display[4].strip(),
                         f"Line 4 should be blank, got: {repr(screen.display[4])}")


if __name__ == "__main__":
    unittest.main()
