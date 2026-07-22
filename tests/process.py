"""PTY process management for VIEW editor tests."""

import atexit
import pty
import os
import time
import select
import signal
import struct
import fcntl
import termios


class PtyProcess:
    """Manage a child process running inside a PTY."""

    _instances = set()

    def __init__(self, argv, env=None, term="vt100"):
        env = env or {}
        full_env = {**os.environ, "TERM": term, **env}

        master_fd, slave_fd = pty.openpty()
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
        self._closed = False
        self._instances.add(self)

    def close(self):
        if self._closed:
            return
        self._closed = True
        self._instances.discard(self)
        if self.master_fd >= 0:
            try:
                os.close(self.master_fd)
            except OSError:
                pass
            self.master_fd = -1
        if self.pid > 0:
            try:
                os.kill(self.pid, signal.SIGKILL)
            except OSError:
                pass
            try:
                os.waitpid(self.pid, 0)
            except ChildProcessError:
                pass
            self.pid = -1

    def read(self, timeout=5.0):
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
        idle = 0.0
        while True:
            if pattern in self._buf:
                break
            r, _, _ = select.select([self.master_fd], [], [], 0.05)
            if not r:
                idle += 0.05
                if idle >= timeout:
                    break
                continue
            idle = 0.0
            try:
                data = os.read(self.master_fd, 4096)
                self._buf += data
            except OSError:
                break
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
            r, _, _ = select.select(
                [self.master_fd], [], [], min(remaining, 0.1)
            )
        pid, status = os.waitpid(self.pid, os.WNOHANG)
        if pid == 0:
            try:
                pid, status = os.waitpid(self.pid, 0)
            except ChildProcessError:
                status = 0
        output = self._buf
        self._buf = b""
        return status, output

    def drain(self, timeout=0.05):
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


def _cleanup_all():
    for proc in list(PtyProcess._instances):
        proc.close()

atexit.register(_cleanup_all)