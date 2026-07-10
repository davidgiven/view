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
        self.assertIn(b"1300", output,
                      f"Expected word count 1300 in output, got: {repr(output)}")
        self.assertIn(b"word(s) counted", output,
                      f"Expected 'word(s) counted' in output, got: {repr(output)}")


class EditorTests(unittest.TestCase):

    def setUp(self):
        self.proc = PtyProcess([VIEW_BIN])

    def tearDown(self):
        self.proc.close()

    def test_enter_editor_after_loading_file(self):
        self.proc.read_until(b"=>", timeout=0.5)
        self.proc.writeline("LOAD examples/horse.v")
        output = self.proc.read_until(b"=>", timeout=1.0)
        self.assertIn(b"examples/horse.v", output,
                      f"Expected filename in LOAD output, got: {repr(output)}")
        self.assertIn(b"Editing examples/horse.v", output,
                      f"Expected 'Editing' line showing filename, got: {repr(output)}")
        self.proc.writeline("")
        time.sleep(0.3)
        output = self.proc.read(timeout=0.5)
        term = pyte.Screen(80, 24)
        stream = pyte.Stream(term)
        stream.feed(output.decode("latin-1"))
        # Check that screen line 1 (second line, 0-indexed) contains the document content
        line1 = term.display[1]
        self.assertIn("The Water Horse's", line1,
                      f"Expected 'The Water Horse's' on screen line 1, got: {repr(line1)}")


if __name__ == "__main__":
    unittest.main()
