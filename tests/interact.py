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

VIEW_BIN = os.path.join(
    os.path.dirname(__file__), "..", "bin", "view_for_testing"
)

# Control codes used in editor commands
CTRL_A  = b"\x01"; CTRL_B  = b"\x02"; CTRL_C  = b"\x03"; CTRL_D  = b"\x04"
CTRL_E  = b"\x05"; CTRL_F  = b"\x06"; CTRL_G  = b"\x07"; CTRL_H  = b"\x08"
CTRL_I  = b"\x09"; CTRL_J  = b"\x0a"; CTRL_K  = b"\x0b"; CTRL_L  = b"\x0c"
CTRL_M  = b"\x0d"; CTRL_N  = b"\x0e"; CTRL_O  = b"\x0f"; CTRL_P  = b"\x10"
CTRL_Q  = b"\x11"; CTRL_R  = b"\x12"; CTRL_S  = b"\x13"; CTRL_T  = b"\x14"
CTRL_U  = b"\x15"; CTRL_V  = b"\x16"; CTRL_W  = b"\x17"; CTRL_X  = b"\x18"
CTRL_Y  = b"\x19"; CTRL_Z  = b"\x1a"

# Screen key codes (from screen.h)
KEY_UP    = b"\x8b"
KEY_DOWN  = b"\x8a"
KEY_LEFT  = b"\x88"
KEY_RIGHT = b"\x89"


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
        raw = self._drain_editor(1)
        screen = pyte.Screen(80, 24)
        stream = pyte.Stream(screen)
        stream.feed(raw.decode("latin-1"))
        return load_output, screen

    def test_enter_editor_after_loading_file(self):
        output, screen = self._load_and_enter_editor("examples/horse.v")
        self.assertIn(
            b"examples/horse.v",
            output,
            f"Expected filename in LOAD output, got: {repr(output)}",
        )
        self.assertIn(
            b"Editing examples/horse.v",
            output,
            f"Expected 'Editing' line showing filename, got: {repr(output)}",
        )
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
            "   meat, human for  preference,  and  will  use their shapeshifting powers to   ",
            "   lure their prey down to the water's  edge where they will be dragged under   ",
            "   and consumed, leaving only their livers to float  to  the  shore as a sign   ",
            "   that  the  water  horse has taken another victim. Water horses are vicious   ",
            "   and  terrifying, far more  dangerous  than  many  of  the  other  Scottish   ",
            "   supernatural creatures,  and  the locals would always treat the appearance   ",
            "   of a lone animal  or man near the water's edge with caution, for fear that   ",
            "   the water horse might be hungry again. And rightly so.                       ",
            "                                                                                ",
            "   In a small loch in  north-west Scotland whose name I can't quite remember,   ",
            "   there once lived a water horse  and  his  wife. They preyed upon the local   ",
            "   crofters, for of course there were no fishermen ---  with  a pair of water   ",
            "   horses in the loch venturing into the water was far too dangerous. Luckily   ",
            "   for the locals, water horses do not get hungry very often.                   ",
        ]
        self._assert_screen_lines(screen, expected_lines)

    def _assert_screen_lines(self, screen, expected):
        """Assert that screen.display matches *expected* (a list of row strings)."""
        for i, exp in enumerate(expected):
            self.assertEqual(
                exp,
                screen.display[i],
                f"Row {i} mismatch: expected {repr(exp)}, got {repr(screen.display[i])}",
            )

    def _enter_editor_empty(self):
        """Enter editor mode without loading a file. Returns pyte Screen."""
        self.proc.read_until(b"=>", timeout=0.5)
        self.proc.writeline("")
        raw = self._drain_editor(1)
        screen = pyte.Screen(80, 24)
        stream = pyte.Stream(screen)
        stream.feed(raw.decode("latin-1"))
        return screen

    def _test_enter_editor_and_type(self, string_to_type, expected_screen):
        """Enter empty editor, optionally type *string_to_type*, then assert
        the screen matches *expected_screen*."""
        screen = self._enter_editor_empty()
        if string_to_type:
            self.proc.write(string_to_type)
            raw = self._drain_editor(len(string_to_type))
            pyte.Stream(screen).feed(raw.decode("latin-1"))
        self._assert_screen_lines(screen, expected_screen)

    def test_enter_editor_empty(self):
        self._test_enter_editor_and_type(
            None,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "                                                                                ",
                "********************************************************************************",
            ],
        )

    def _drain_editor(self, n_markers=1):
        """Read editor PTY output until *n_markers* (\\x05 bytes) have been
        received, indicating the editor has processed that many input events
        and called getch() again.  Strips the \\x05 markers before returning."""
        data = b""
        for _ in range(n_markers):
            data += self.proc.read_until(b"\x05", timeout=2.0)
        return data.replace(b"\x05", b"")

    def test_enter_editor_and_type_q(self):
        """Enter empty editor, type 'q', then verify the visible screen lines."""
        self._test_enter_editor_and_type(
            b"q",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   q                                                                            ",
                "********************************************************************************",
            ],
        )

    def test_enter_editor_press_enter(self):
        """Enter empty editor, press Enter, expect two empty lines."""
        self._test_enter_editor_and_type(
            b"\r",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "                                                                                ",
                "                                                                                ",
                "********************************************************************************",
            ],
        )

    def test_enter_editor_and_type_qwerty(self):
        """Enter empty editor, type 'qwerty', then check that characters appear."""
        self._test_enter_editor_and_type(
            b"qwerty",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   qwerty                                                                       ",
                "********************************************************************************",
            ],
        )

    def test_enter_editor_and_type_two_lines(self):
        """Enter empty editor, type two lines with a newline between, then check that characters appear."""
        self._test_enter_editor_and_type(
            b"line1" + CTRL_M + b"line2",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   line1                                                                        ",
                "   line2                                                                        ",
                "********************************************************************************",
            ],
        )

    def test_enter_editor_and_cursor_left_right(self):
        self._test_enter_editor_and_type(
            b"text" + KEY_RIGHT + b"1" + KEY_LEFT * 4 + b"23",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   te23 1                                                                       ",
                "********************************************************************************",
            ],
        )

    def test_enter_editor_and_cursor_down_up(self):
        self._test_enter_editor_and_type(
            CTRL_M + KEY_UP + b"text" + KEY_DOWN + b"1" + KEY_UP + b"2" + KEY_UP + b"3",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   text 23                                                                      ",
                "       1                                                                        ",
                "********************************************************************************",
            ],
        )

    def test_enter_editor_and_a_long_line_nonjustified(self):
        self._test_enter_editor_and_type(
            CTRL_O + CTRL_J +
            b"In a small loch in north-west Scotland whose name I can't quite remember, "
            b"there once lived a water horse and his wife. They preyed upon the local "
            b"crofters, for of course there were no fishermen --- with a pair of water "
            b"horses in the loch venturing into the water was far too dangerous. Luckily "
            b"for the locals, water horses do not get hungry very often. ",
            [
                "F  .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   In a small loch in north-west Scotland whose name I can't quite remember,    ",
                "   there once lived a water horse and his wife. They preyed upon the local      ",
                "   crofters, for of course there were no fishermen --- with a pair of water     ",
                "   horses in the loch venturing into the water was far too dangerous.           ",
                "   Luckily for the locals, water horses do not get hungry very often.           ",
                "********************************************************************************",
            ],
        )

    def test_enter_editor_and_a_long_line_justified(self):
        self._test_enter_editor_and_type(
            b"In a small loch in north-west Scotland whose name I can't quite remember, "
            b"there once lived a water horse and his wife. They preyed upon the local "
            b"crofters, for of course there were no fishermen --- with a pair of water "
            b"horses in the loch venturing into the water was far too dangerous. Luckily "
            b"for the locals, water horses do not get hungry very often. ",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   In  a small loch in north-west Scotland whose name I can't quite remember,   ",
                "   there once  lived  a  water horse and his wife. They preyed upon the local   ",
                "   crofters, for of course there  were  no fishermen --- with a pair of water   ",
                "   horses  in  the  loch venturing into the  water  was  far  too  dangerous.   ",
                "   Luckily for the locals, water horses do not get hungry very often.           ",
                "********************************************************************************",
            ],
        )

    def test_enter_editor_and_a_reformat(self):
        self._test_enter_editor_and_type(
            CTRL_O + CTRL_J +
            b"In a small loch in north-west Scotland whose name I can't quite remember, "
            b"there once lived a water horse" +
            KEY_UP + CTRL_V + b"WORD " + CTRL_B,
            [
                "F  .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   In a small loch in north-west WORD Scotland whose name I can't quite         ",
                "   remember, there once lived a water horse                                     ",
                "********************************************************************************",
            ],
        )

    def test_enter_editor_and_split_line(self):
        """Enter empty editor, type 'Hello World', then split at the space between words."""
        self._test_enter_editor_and_type(
            b"Hello World" + KEY_LEFT * 6 + CTRL_Q + b"J",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   Hello                                                                        ",
                "    World                                                                       ",
                "********************************************************************************",
            ],
        )

    def test_enter_editor_and_split_line_at_char(self):
        """Enter empty editor, type 'abcd', then split at middle (after 'b')."""
        self._test_enter_editor_and_type(
            b"abcd" + KEY_LEFT * 2 + CTRL_Q + b"J",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   ab                                                                           ",
                "   cd                                                                           ",
                "********************************************************************************",
            ],
        )

    def test_enter_editor_and_split_line_at_beginning(self):
        """Split at the very beginning of the text — creates an empty first line."""
        self._test_enter_editor_and_type(
            b"abcd" + KEY_LEFT * 4 + CTRL_Q + b"J",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "                                                                                ",
                "   abcd                                                                         ",
                "********************************************************************************",
            ],
        )

    def test_enter_editor_and_split_line_at_end(self):
        """Split at the very end of the text — creates an empty second line."""
        self._test_enter_editor_and_type(
            b"abcd" + CTRL_Q + b"J",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   abcd                                                                         ",
                "                                                                                ",
                "********************************************************************************",
            ],
        )

    def test_enter_editor_and_type_numbers_1_to_10(self):
        """Enter numbers 1 to 10 each on their own line (fits on screen)."""
        keys = b""
        for i in range(1, 11):
            keys += str(i).encode("ascii")
            if i < 10:
                keys += CTRL_M
        self._test_enter_editor_and_type(
            keys,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   1                                                                            ",
                "   2                                                                            ",
                "   3                                                                            ",
                "   4                                                                            ",
                "   5                                                                            ",
                "   6                                                                            ",
                "   7                                                                            ",
                "   8                                                                            ",
                "   9                                                                            ",
                "   10                                                                           ",
                "********************************************************************************",
            ],
        )

    def test_enter_editor_and_type_numbers_1_to_22(self):
        """Enter numbers 1 to 22 each on their own line (fits exactly on screen)."""
        keys = b""
        for i in range(1, 23):
            keys += str(i).encode("ascii")
            if i < 22:
                keys += CTRL_M
        self._test_enter_editor_and_type(
            keys,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   1                                                                            ",
                "   2                                                                            ",
                "   3                                                                            ",
                "   4                                                                            ",
                "   5                                                                            ",
                "   6                                                                            ",
                "   7                                                                            ",
                "   8                                                                            ",
                "   9                                                                            ",
                "   10                                                                           ",
                "   11                                                                           ",
                "   12                                                                           ",
                "   13                                                                           ",
                "   14                                                                           ",
                "   15                                                                           ",
                "   16                                                                           ",
                "   17                                                                           ",
                "   18                                                                           ",
                "   19                                                                           ",
                "   20                                                                           ",
                "   21                                                                           ",
                "   22                                                                           ",
                "********************************************************************************",
            ],
        )

    def test_enter_editor_and_type_numbers_1_to_30(self):
        """Enter numbers 1 to 30 each on their own line — tests scrolling."""
        keys = b""
        for i in range(1, 31):
            keys += str(i).encode("ascii")
            if i < 30:
                keys += CTRL_M
        self._test_enter_editor_and_type(
            keys,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   8                                                                            ",
                "   9                                                                            ",
                "   10                                                                           ",
                "   11                                                                           ",
                "   12                                                                           ",
                "   13                                                                           ",
                "   14                                                                           ",
                "   15                                                                           ",
                "   16                                                                           ",
                "   17                                                                           ",
                "   18                                                                           ",
                "   19                                                                           ",
                "   20                                                                           ",
                "   21                                                                           ",
                "   22                                                                           ",
                "   23                                                                           ",
                "   24                                                                           ",
                "   25                                                                           ",
                "   26                                                                           ",
                "   27                                                                           ",
                "   28                                                                           ",
                "   29                                                                           ",
                "   30                                                                           ",
            ],
        )

    def test_enter_editor_and_join_two_lines(self):
        """Join two adjacent lines."""
        self._test_enter_editor_and_type(
            b"abc" + CTRL_M + b"def" + KEY_UP + CTRL_J,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   abcdef                                                                       ",
                "********************************************************************************",
            ],
        )

    def test_enter_editor_and_join_last_line_noop(self):
        """Joining when on the last line should do nothing."""
        self._test_enter_editor_and_type(
            b"abc" + CTRL_M + b"def" + CTRL_J,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   abc                                                                          ",
                "   def                                                                          ",
                "********************************************************************************",
            ],
        )

    def test_enter_editor_and_join_middle_line(self):
        """Join the first of three lines — the third line shifts up."""
        self._test_enter_editor_and_type(
            b"abc" + CTRL_M + b"def" + CTRL_M + b"ghi" + KEY_UP * 2 + CTRL_J,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   abcdef                                                                       ",
                "   ghi                                                                          ",
                "********************************************************************************",
            ],
        )

    def test_go_to_top_of_file(self):
        """Type numbers 1 to 30, then go to top — viewport should show lines 1-23."""
        keys = b""
        for i in range(1, 31):
            keys += str(i).encode("ascii")
            if i < 30:
                keys += CTRL_M
        keys += CTRL_Q + CTRL_R
        self._test_enter_editor_and_type(
            keys,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   1                                                                            ",
                "   2                                                                            ",
                "   3                                                                            ",
                "   4                                                                            ",
                "   5                                                                            ",
                "   6                                                                            ",
                "   7                                                                            ",
                "   8                                                                            ",
                "   9                                                                            ",
                "   10                                                                           ",
                "   11                                                                           ",
                "   12                                                                           ",
                "   13                                                                           ",
                "   14                                                                           ",
                "   15                                                                           ",
                "   16                                                                           ",
                "   17                                                                           ",
                "   18                                                                           ",
                "   19                                                                           ",
                "   20                                                                           ",
                "   21                                                                           ",
                "   22                                                                           ",
                "   23                                                                           ",
            ],
        )

    def test_go_to_bottom_of_file(self):
        """Type numbers 1 to 30, go to top, then go to bottom — viewport shows lines 19-30 (sub_ca44e recenters cursor at ~row 12), remaining rows blank."""
        keys = b""
        for i in range(1, 31):
            keys += str(i).encode("ascii")
            if i < 30:
                keys += CTRL_M
        keys += CTRL_Q + CTRL_R  # go to top
        keys += CTRL_Q + CTRL_C  # go to bottom
        self._test_enter_editor_and_type(
            keys,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   19                                                                           ",
                "   20                                                                           ",
                "   21                                                                           ",
                "   22                                                                           ",
                "   23                                                                           ",
                "   24                                                                           ",
                "   25                                                                           ",
                "   26                                                                           ",
                "   27                                                                           ",
                "   28                                                                           ",
                "   29                                                                           ",
                "   30                                                                           ",
                "********************************************************************************",
                "                                                                                ",
                "                                                                                ",
                "                                                                                ",
                "                                                                                ",
                "                                                                                ",
                "                                                                                ",
                "                                                                                ",
                "                                                                                ",
                "                                                                                ",
                "                                                                                ",
            ],
        )

    def test_go_to_beginning_and_end_of_line(self):
        """Type 'abc def', go to beginning (CTRL_Q+CTRL_S), overwrite with 'X', go to end (CTRL_Q+CTRL_D), append 'Y'."""
        self._test_enter_editor_and_type(
            b"abc def" + CTRL_Q + CTRL_S + b"X" + CTRL_Q + CTRL_D + b"Y",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   Xbc defY                                                                     ",
                "********************************************************************************",
            ],
        )

    def test_word_left(self):
        """Type 'abc def', go to end, word left (CTRL_A), overwrite with 'X' — X should replace 'd' at start of 'def'."""
        self._test_enter_editor_and_type(
            b"abc def" + CTRL_Q + CTRL_D + CTRL_A + b"X",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   abc Xef                                                                      ",
                "********************************************************************************",
            ],
        )

    def test_word_right(self):
        self._test_enter_editor_and_type(
            b"abc def" + CTRL_Q + CTRL_S + CTRL_F + b"X",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   abc Xef                                                                      ",
                "********************************************************************************",
            ],
        )

    def test_word_left_at_line_start_wraps_to_previous_line(self):
        """Type two lines, go to start of line 2, word left twice — wraps to previous line start."""
        self._test_enter_editor_and_type(
            b"abc" + CTRL_M + b"def" + CTRL_Q + CTRL_S + CTRL_A + CTRL_A + b"X",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   Xbc                                                                          ",
                "   def                                                                          ",
                "********************************************************************************",
            ],
        )

    def test_word_right_at_line_end_wraps_to_next_line(self):
        """Type two lines, go to start of line 1, word right twice — wraps to start of line 2."""
        self._test_enter_editor_and_type(
            b"abc" + CTRL_M + b"def" + KEY_UP + CTRL_Q + CTRL_S + CTRL_F + CTRL_F + b"X",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   abc                                                                          ",
                "   Xef                                                                          ",
                "********************************************************************************",
            ],
        )

    def test_delete_char_at_beginning(self):
        """Type 'abc', go to start, CTRL_G — deletes 'a'."""
        self._test_enter_editor_and_type(
            b"abc" + CTRL_Q + CTRL_S + CTRL_G,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   bc                                                                           ",
                "********************************************************************************",
            ],
        )

    def test_delete_char_in_middle(self):
        """Type 'abc', go to second char, CTRL_G — deletes 'b'."""
        self._test_enter_editor_and_type(
            b"abc" + CTRL_Q + CTRL_S + KEY_RIGHT + CTRL_G,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   ac                                                                           ",
                "********************************************************************************",
            ],
        )

    def test_delete_char_at_end(self):
        """Type 'abc', go to last char, CTRL_G — deletes 'c'."""
        self._test_enter_editor_and_type(
            b"abc" + CTRL_Q + CTRL_D + KEY_LEFT + CTRL_G,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   ab                                                                           ",
                "********************************************************************************",
            ],
        )

    def test_insert_char_at_beginning(self):
        """Type 'abc', go to start, CTRL_H (insert space), then 'X'."""
        self._test_enter_editor_and_type(
            b"abc" + CTRL_Q + CTRL_S + CTRL_H + b"X",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   Xabc                                                                         ",
                "********************************************************************************",
            ],
        )

    def test_insert_char_in_middle(self):
        """Type 'abc', go to second char, CTRL_H (insert space), then 'X'."""
        self._test_enter_editor_and_type(
            b"abc" + CTRL_Q + CTRL_S + KEY_RIGHT + CTRL_H + b"X",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   aXbc                                                                         ",
                "********************************************************************************",
            ],
        )

    def test_insert_char_at_end(self):
        """Type 'abc', go to last char, CTRL_H (insert space), then 'X'."""
        self._test_enter_editor_and_type(
            b"abc" + CTRL_Q + CTRL_D + KEY_LEFT + CTRL_H + b"X",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   abXc                                                                         ",
                "********************************************************************************",
            ],
        )

    def test_insert_line(self):
        """Type 'abc', CTRL_N (insert line), type 'def' — new line inserted above current."""
        self._test_enter_editor_and_type(
            b"abc" + CTRL_N + b"def",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "      def                                                                       ",
                "   abc                                                                          ",
                "********************************************************************************",
            ],
        )

    def test_delete_line(self):
        """Type two lines, go to first line, CTRL_Y — deletes current line."""
        self._test_enter_editor_and_type(
            b"abc" + CTRL_M + b"def" + KEY_UP + CTRL_Y,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   def                                                                          ",
                "********************************************************************************",
            ],
        )

    def test_insert_line_at_beginning(self):
        """Type two lines, go to top, CTRL_N, type 'XYZ' — inserted above first line."""
        self._test_enter_editor_and_type(
            b"abc" + CTRL_M + b"def" + CTRL_Q + CTRL_R + CTRL_N + b"XYZ",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "      XYZ                                                                       ",
                "   abc                                                                          ",
                "   def                                                                          ",
                "********************************************************************************",
            ],
        )

    def test_insert_line_at_end(self):
        """Type two lines, go to bottom, CTRL_N, type 'XYZ' — inserted above last line."""
        self._test_enter_editor_and_type(
            b"abc" + CTRL_M + b"def" + CTRL_Q + CTRL_C + CTRL_N + b"XYZ",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   abc                                                                          ",
                "      XYZ                                                                       ",
                "   def                                                                          ",
                "********************************************************************************",
            ],
        )

    def test_delete_line_at_beginning(self):
        """Type three lines, go to top, CTRL_Y — deletes first line."""
        self._test_enter_editor_and_type(
            b"abc" + CTRL_M + b"def" + CTRL_M + b"ghi" + CTRL_Q + CTRL_R + CTRL_Y,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   def                                                                          ",
                "   ghi                                                                          ",
                "********************************************************************************",
            ],
        )

    def test_delete_to_char(self):
        """Type 'abc.def', go to start, CTRL_T + '.' — deletes forward through '.'."""
        self._test_enter_editor_and_type(
            b"abc.def" + CTRL_Q + CTRL_S + CTRL_T + b".",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   def                                                                          ",
                "********************************************************************************",
            ],
        )

    def test_delete_to_char_not_found(self):
        """Type 'abc', CTRL_T + 'z' — character not found, no change."""
        self._test_enter_editor_and_type(
            b"abc" + CTRL_T + b"z",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   abc                                                                          ",
                "********************************************************************************",
            ],
        )

    def test_edit_command_immediate_exit(self):
        """Type 'Hello', CTRL_O+CTRL_C, CR — enter and exit edit command, no change."""
        self._test_enter_editor_and_type(
            b"Hello" + CTRL_O + CTRL_C + CTRL_M,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   Hello                                                                        ",
                "********************************************************************************",
            ],
        )

    def test_edit_command_set_and_change(self):
        """Set command 'AB', then change to 'CD'."""
        self._test_enter_editor_and_type(
            b"Hello" + CTRL_O + CTRL_C + b"AB" + CTRL_M + CTRL_O + CTRL_C + b"CD" + CTRL_M,
            [
                "MJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "CD Hello                                                                        ",
                "********************************************************************************",
            ],
        )

    def test_edit_command_set_then_immediate_exit(self):
        """Set command 'AB', then enter edit command again and exit immediately — command unchanged."""
        self._test_enter_editor_and_type(
            b"Hello" + CTRL_O + CTRL_C + b"AB" + CTRL_M + CTRL_O + CTRL_C + CTRL_M,
            [
                "MJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "AB Hello                                                                        ",
                "********************************************************************************",
            ],
        )

    def test_edit_command_set_then_delete(self):
        """Set command 'AB', then delete it with CTRL_O+CTRL_D — returns to normal."""
        self._test_enter_editor_and_type(
            b"Hello" + CTRL_O + CTRL_C + b"AB" + CTRL_M + CTRL_O + CTRL_D,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   Hello                                                                        ",
                "********************************************************************************",
            ],
        )

    def test_delete_line_at_end(self):
        """Type three lines, CTRL_Y — deletes last line (cursor already on it)."""
        self._test_enter_editor_and_type(
            b"abc" + CTRL_M + b"def" + CTRL_M + b"ghi" + CTRL_Y,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   abc                                                                          ",
                "   def                                                                          ",
                "********************************************************************************",
            ],
        )

    def test_swap_case(self):
        """Type 'ABC', go to start, CTRL_P three times — uppercase letters become lowercase."""
        self._test_enter_editor_and_type(
            b"ABC" + CTRL_Q + CTRL_S + CTRL_P + CTRL_P + CTRL_P,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   abc                                                                          ",
                "********************************************************************************",
            ],
        )

    def test_swap_case_lowercase_unchanged(self):
        """Lowercase 'abc', CTRL_P — no change (only uppercase→lowercase)."""
        self._test_enter_editor_and_type(
            b"abc" + CTRL_Q + CTRL_S + CTRL_P + CTRL_P + CTRL_P,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   abc                                                                          ",
                "********************************************************************************",
            ],
        )

    def test_swap_case_non_alpha_unchanged(self):
        """Non-alphabetic '123!@#', CTRL_P — no change, cursor moves past."""
        self._test_enter_editor_and_type(
            b"123!@#" + CTRL_Q + CTRL_S + CTRL_P + CTRL_P + CTRL_P + CTRL_P + CTRL_P + CTRL_P,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   123!@#                                                                       ",
                "********************************************************************************",
            ],
        )

    def test_swap_case_mixed(self):
        """Mixed 'A1b2C', CTRL_P — only uppercase letters (A,C) lowercased."""
        self._test_enter_editor_and_type(
            b"A1b2C" + CTRL_Q + CTRL_S + CTRL_P + CTRL_P + CTRL_P + CTRL_P + CTRL_P,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   a1b2c                                                                        ",
                "********************************************************************************",
            ],
        )

    def test_insert_mode_at_beginning(self):
        """Type 'abc', go to start, CTRL_V (insert mode), 'XYZ' — inserted at beginning."""
        self._test_enter_editor_and_type(
            b"abc" + CTRL_Q + CTRL_S + CTRL_V + b"XYZ",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   XYZabc                                                                       ",
                "********************************************************************************",
            ],
        )

    def test_insert_mode_at_middle(self):
        """Type 'abc', go to second char, CTRL_V, 'XYZ' — inserted in middle."""
        self._test_enter_editor_and_type(
            b"abc" + CTRL_Q + CTRL_S + KEY_RIGHT + CTRL_V + b"XYZ",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   aXYZbc                                                                       ",
                "********************************************************************************",
            ],
        )

    def test_insert_mode_at_end(self):
        """Type 'abc', go to end, CTRL_V, 'XYZ' — appended at end."""
        self._test_enter_editor_and_type(
            b"abc" + CTRL_Q + CTRL_D + CTRL_V + b"XYZ",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   abcXYZ                                                                       ",
                "********************************************************************************",
            ],
        )

    def test_insert_mode_toggle_off_at_beginning(self):
        """CTRL_V on, insert 'XYZ', toggle off, type '123' — 123 overwrites."""
        self._test_enter_editor_and_type(
            b"abc" + CTRL_Q + CTRL_S + CTRL_V + b"XYZ" + CTRL_V + b"123",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   XYZ123                                                                       ",
                "********************************************************************************",
            ],
        )

    def test_insert_mode_toggle_off_at_middle(self):
        """Insert mode on, insert 'XY', toggle off, '12' overwrites."""
        self._test_enter_editor_and_type(
            b"abcde" + CTRL_Q + CTRL_S + KEY_RIGHT + KEY_RIGHT + CTRL_V + b"XY" + CTRL_V + b"12",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   abXY12e                                                                      ",
                "********************************************************************************",
            ],
        )

    def test_insert_mode_toggle_off_at_end(self):
        """Insert mode on, append 'XY', toggle off, '12' overwrites past end (appends)."""
        self._test_enter_editor_and_type(
            b"abc" + CTRL_Q + CTRL_D + CTRL_V + b"XY" + CTRL_V + b"12",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   abcXY12                                                                      ",
                "********************************************************************************",
            ],
        )

    def test_enter_editor_after_loading_jabber(self):
        output, screen = self._load_and_enter_editor("examples/jabber.v")
        self.assertIn(
            b"jabber.v",
            output,
            f"Expected filename in LOAD output, got: {repr(output)}",
        )
        expected = [
            "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
            "   She  puzzled  over this for some time, but at last a bright thought struck   ",
            "   her. \"Why, it's  a Looking-glass book, of course! And if I hold it up to a   ",
            '   glass, the words will all go the right way again."                           ',
            "                                                                                ",
            "   This was the poem that Alice read.                                           ",
            "                                                                                ",
            "         'Twas brillig, and the slithy toves                                    ",
            "          did gyre and gimble in the wabe;                                      ",
            "         All mimsy were the borogroves,                                         ",
            "          and the mome raths outgrabe.                                          ",
            "                                                                                ",
            '         "Beware the Jabberwock, my son!                                        ',
            "          The jaws that bite, the claws that catch!                             ",
            "         Beware the Jubjub bird, and shun                                       ",
            "          the frumious Bandersnatch!                                            ",
            "                                                                                ",
            "         He took his vorpal sword in hand:                                      ",
            "          long time the maxome foe he sought---                                 ",
            "         So rested he by the Tumtum tree,                                       ",
            "          and stood awhile in thought.                                          ",
            "                                                                                ",
            "         And as in uffish thought he stood,                                     ",
        ]
        self._assert_screen_lines(screen, expected)


if __name__ == "__main__":
    unittest.main()
