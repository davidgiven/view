#!/usr/bin/env python3
"""Verify that view outputs its expected program banner on startup."""

import pty
import os
import time
import select
import signal
import re
import unittest

VIEW_BIN = os.path.join(os.path.dirname(__file__), "..", "bin", "view")


def strip_escapes(text: str) -> str:
    """Remove ANSI/VT100 escape sequences."""
    return re.sub(r"\x1b\[[0-9;]*[a-zA-Z]", "", text).replace("\x1b", "")


class PtyProcess:
    """Manage a child process running inside a PTY."""

    def __init__(self, argv, env=None, term="vt100"):
        env = env or {}
        full_env = {**os.environ, "TERM": term, **env}

        master_fd, slave_fd = pty.openpty()
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


class TestBanner(unittest.TestCase):

    def setUp(self):
        self.proc = PtyProcess([VIEW_BIN])

    def tearDown(self):
        self.proc.close()

    def test_prompt_is_seen(self):
        output = self.proc.read_until(b"=>\n", timeout=3.0)
        self.assertTrue(
            output.endswith(b"=>\n") or output.endswith(b"=>\r\n"),
            f"Expected prompt '=>\\n' at end of output, got: {repr(output[-40:])}"
        )

    def test_quit_command(self):
        self.proc.read_until(b"=>\n", timeout=3.0)
        self.proc.writeline("BYE")
        status, output = self.proc.wait(timeout=3.0)
        self.assertTrue(os.WIFEXITED(status),
                        f"Process did not exit cleanly (status={status})")
        self.assertEqual(os.WEXITSTATUS(status), 0,
                         f"Expected exit code 0, got {os.WEXITSTATUS(status)}")

    def test_load_missing_file(self):
        self.proc.read_until(b"=>\n", timeout=3.0)
        self.proc.writeline("load missing.v")
        output = self.proc.read_until(b"=>\n", timeout=3.0)
        self.assertIn(b"File not found", output,
                      f"Expected 'File not found' in output, got: {repr(output)}")
        self.assertTrue(
            output.endswith(b"=>\n") or output.endswith(b"=>\r\n"),
            f"Expected output to end with prompt, got: {repr(output[-40:])}"
        )


if __name__ == "__main__":
    unittest.main()
