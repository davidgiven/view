#!/usr/bin/env python3
"""Editor integration tests for VIEW via PTY."""

import unittest
import pyte

from utils import *
from process import PtyProcess


class EditorTests(unittest.TestCase):

    def setUp(self):
        self.proc = PtyProcess([VIEW_BIN])

    def tearDown(self):
        self.proc.close()

    def _load_and_enter_editor(self, filename):
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
        for i, exp in enumerate(expected):
            self.assertEqual(
                exp,
                screen.display[i],
                f"Row {i} mismatch: expected {repr(exp)}, got {repr(screen.display[i])}",
            )

    def _enter_editor_empty(self):
        self.proc.read_until(b"=>", timeout=0.5)
        self.proc.writeline("")
        raw = self._drain_editor(1)
        screen = pyte.Screen(80, 24)
        stream = pyte.Stream(screen)
        stream.feed(raw.decode("latin-1"))
        return screen

    def _test_enter_editor_and_type(self, string_to_type, expected_screen):
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
        data = b""
        for _ in range(n_markers):
            data += self.proc.read_until(b"\x05", timeout=2.0)
        return data.replace(b"\x05", b"")

    def test_enter_editor_and_type_q(self):
        self._test_enter_editor_and_type(
            b"q",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   q                                                                            ",
                "********************************************************************************",
            ],
        )

    def test_delete_with_backspace(self):
        self._test_enter_editor_and_type(
            b"hello" + KEY_BACKSPACE,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   hell                                                                         ",
                "********************************************************************************",
            ],
        )

    def test_enter_editor_press_enter(self):
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
        self._test_enter_editor_and_type(
            b"qwerty",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   qwerty                                                                       ",
                "********************************************************************************",
            ],
        )

    def test_enter_editor_and_type_two_lines(self):
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
            CTRL_M
            + KEY_UP
            + b"text"
            + KEY_DOWN
            + b"1"
            + KEY_UP
            + b"2"
            + KEY_UP
            + b"3",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   text 23                                                                      ",
                "       1                                                                        ",
                "********************************************************************************",
            ],
        )

    def test_enter_editor_and_a_long_line_nonjustified(self):
        self._test_enter_editor_and_type(
            CTRL_O
            + CTRL_J
            + b"In a small loch in north-west Scotland whose name I can't quite remember, "
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
            CTRL_O
            + CTRL_J
            + b"In a small loch in north-west Scotland whose name I can't quite remember, "
            b"there once lived a water horse"
            + KEY_UP
            + CTRL_V
            + b"WORD "
            + CTRL_B,
            [
                "F  .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   In a small loch in north-west WORD Scotland whose name I can't quite         ",
                "   remember, there once lived a water horse                                     ",
                "********************************************************************************",
            ],
        )

    def test_enter_editor_and_split_line(self):
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
        self._test_enter_editor_and_type(
            b"abc" + CTRL_M + b"def" + KEY_UP + CTRL_J,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   abcdef                                                                       ",
                "********************************************************************************",
            ],
        )

    def test_enter_editor_and_join_last_line_noop(self):
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
        keys = b""
        for i in range(1, 31):
            keys += str(i).encode("ascii")
            if i < 30:
                keys += CTRL_M
        keys += CTRL_Q + CTRL_R
        keys += CTRL_Q + CTRL_C
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
        self._test_enter_editor_and_type(
            b"abc def" + CTRL_Q + CTRL_S + b"X" + CTRL_Q + CTRL_D + b"Y",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   Xbc defY                                                                     ",
                "********************************************************************************",
            ],
        )

    def test_word_left(self):
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
        self._test_enter_editor_and_type(
            b"abc"
            + CTRL_M
            + b"def"
            + KEY_UP
            + CTRL_Q
            + CTRL_S
            + CTRL_F
            + CTRL_F
            + b"X",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   abc                                                                          ",
                "   Xef                                                                          ",
                "********************************************************************************",
            ],
        )

    def test_delete_char_at_beginning(self):
        self._test_enter_editor_and_type(
            b"abc" + CTRL_Q + CTRL_S + CTRL_G,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   bc                                                                           ",
                "********************************************************************************",
            ],
        )

    def test_delete_char_in_middle(self):
        self._test_enter_editor_and_type(
            b"abc" + CTRL_Q + CTRL_S + KEY_RIGHT + CTRL_G,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   ac                                                                           ",
                "********************************************************************************",
            ],
        )

    def test_delete_char_at_end(self):
        self._test_enter_editor_and_type(
            b"abc" + CTRL_Q + CTRL_D + KEY_LEFT + CTRL_G,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   ab                                                                           ",
                "********************************************************************************",
            ],
        )

    def test_insert_line(self):
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
        self._test_enter_editor_and_type(
            b"abc" + CTRL_M + b"def" + KEY_UP + CTRL_Y,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   def                                                                          ",
                "********************************************************************************",
            ],
        )

    def test_insert_line_at_beginning(self):
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
        self._test_enter_editor_and_type(
            b"abc"
            + CTRL_M
            + b"def"
            + CTRL_M
            + b"ghi"
            + CTRL_Q
            + CTRL_R
            + CTRL_Y,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   def                                                                          ",
                "   ghi                                                                          ",
                "********************************************************************************",
            ],
        )

    def test_delete_to_char(self):
        self._test_enter_editor_and_type(
            b"abc.def" + CTRL_Q + CTRL_S + CTRL_T + b".",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   def                                                                          ",
                "********************************************************************************",
            ],
        )

    def test_delete_to_char_not_found(self):
        self._test_enter_editor_and_type(
            b"abc" + CTRL_T + b"z",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   abc                                                                          ",
                "********************************************************************************",
            ],
        )

    def test_edit_command_immediate_exit(self):
        self._test_enter_editor_and_type(
            b"Hello" + CTRL_O + CTRL_C + CTRL_M,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   Hello                                                                        ",
                "********************************************************************************",
            ],
        )

    def test_edit_command_set_and_change(self):
        self._test_enter_editor_and_type(
            b"Hello"
            + CTRL_O
            + CTRL_C
            + b"AB"
            + CTRL_M
            + CTRL_O
            + CTRL_C
            + b"CD"
            + CTRL_M,
            [
                "MJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "CD Hello                                                                        ",
                "********************************************************************************",
            ],
        )

    def test_edit_command_set_then_immediate_exit(self):
        self._test_enter_editor_and_type(
            b"Hello"
            + CTRL_O
            + CTRL_C
            + b"AB"
            + CTRL_M
            + CTRL_O
            + CTRL_C
            + CTRL_M,
            [
                "MJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "AB Hello                                                                        ",
                "********************************************************************************",
            ],
        )

    def test_edit_command_set_then_delete(self):
        self._test_enter_editor_and_type(
            b"Hello" + CTRL_O + CTRL_C + b"AB" + CTRL_M + CTRL_O + CTRL_D,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   Hello                                                                        ",
                "********************************************************************************",
            ],
        )

    def test_delete_line_at_end(self):
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
        self._test_enter_editor_and_type(
            b"ABC" + CTRL_Q + CTRL_S + CTRL_P + CTRL_P + CTRL_P,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   abc                                                                          ",
                "********************************************************************************",
            ],
        )

    def test_swap_case_lowercase_unchanged(self):
        self._test_enter_editor_and_type(
            b"abc" + CTRL_Q + CTRL_S + CTRL_P + CTRL_P + CTRL_P,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   abc                                                                          ",
                "********************************************************************************",
            ],
        )

    def test_swap_case_non_alpha_unchanged(self):
        self._test_enter_editor_and_type(
            b"123!@#"
            + CTRL_Q
            + CTRL_S
            + CTRL_P
            + CTRL_P
            + CTRL_P
            + CTRL_P
            + CTRL_P
            + CTRL_P,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   123!@#                                                                       ",
                "********************************************************************************",
            ],
        )

    def test_swap_case_mixed(self):
        self._test_enter_editor_and_type(
            b"A1b2C"
            + CTRL_Q
            + CTRL_S
            + CTRL_P
            + CTRL_P
            + CTRL_P
            + CTRL_P
            + CTRL_P,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   a1b2c                                                                        ",
                "********************************************************************************",
            ],
        )

    def test_insert_mode_at_beginning(self):
        self._test_enter_editor_and_type(
            b"abc" + CTRL_Q + CTRL_S + CTRL_V + b"XYZ",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   XYZabc                                                                       ",
                "********************************************************************************",
            ],
        )

    def test_insert_mode_at_middle(self):
        self._test_enter_editor_and_type(
            b"abc" + CTRL_Q + CTRL_S + KEY_RIGHT + CTRL_V + b"XYZ",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   aXYZbc                                                                       ",
                "********************************************************************************",
            ],
        )

    def test_insert_mode_at_end(self):
        self._test_enter_editor_and_type(
            b"abc" + CTRL_Q + CTRL_D + CTRL_V + b"XYZ",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   abcXYZ                                                                       ",
                "********************************************************************************",
            ],
        )

    def test_insert_mode_toggle_off_at_beginning(self):
        self._test_enter_editor_and_type(
            b"abc" + CTRL_Q + CTRL_S + CTRL_V + b"XYZ" + CTRL_V + b"123",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   XYZ123                                                                       ",
                "********************************************************************************",
            ],
        )

    def test_insert_mode_toggle_off_at_middle(self):
        self._test_enter_editor_and_type(
            b"abcde"
            + CTRL_Q
            + CTRL_S
            + KEY_RIGHT
            + KEY_RIGHT
            + CTRL_V
            + b"XY"
            + CTRL_V
            + b"12",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   abXY12e                                                                      ",
                "********************************************************************************",
            ],
        )

    def test_insert_mode_toggle_off_at_end(self):
        self._test_enter_editor_and_type(
            b"abc" + CTRL_Q + CTRL_D + CTRL_V + b"XY" + CTRL_V + b"12",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   abcXY12                                                                      ",
                "********************************************************************************",
            ],
        )

    def test_highlight1(self):
        self._test_enter_editor_and_type(
            b"ab" + CTRL_O + CTRL_U + b"X",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   ab-X                                                                         ",
                "********************************************************************************",
            ],
        )

    def test_highlight2(self):
        self._test_enter_editor_and_type(
            b"ab" + CTRL_Q + CTRL_S + CTRL_O + CTRL_B + b"X",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   *Xb                                                                          ",
                "********************************************************************************",
            ],
        )

    def test_insert_ruler(self):
        self._test_enter_editor_and_type(
            CTRL_O + CTRL_S,
            [
                "MJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                ".. .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "                                                                                ",
                "********************************************************************************",
            ],
        )

    def test_insert_ruler_and_down(self):
        self._test_enter_editor_and_type(
            CTRL_O + CTRL_S + KEY_DOWN,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                ".. .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "                                                                                ",
                "********************************************************************************",
            ],
        )

    def test_tab_key(self):
        self._test_enter_editor_and_type(
            b"a" + CTRL_I + b"b" + CTRL_I + b"c" + CTRL_I + b"d",
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   a      b       c       d                                                     ",
                "********************************************************************************",
            ],
        )

    def test_k_command_key_extra_marker(self):
        screen = self._enter_editor_empty()
        self.proc.write(b"a" + CTRL_K + b"1")
        raw = self._drain_editor(3)
        pyte.Stream(screen).feed(raw.decode("latin-1"))
        expected = [
            "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
            "   a                                                                            ",
            "********************************************************************************",
        ]
        self._assert_screen_lines(screen, expected)

    def test_set_and_go_to_marker(self):
        screen = self._enter_editor_empty()
        self.proc.write(
            b"hello" + CTRL_K + b"1" + b" world" + CTRL_Q + b"1" + b"XYZ"
        )
        raw = self._drain_editor(18)
        pyte.Stream(screen).feed(raw.decode("latin-1"))
        self._assert_screen_lines(
            screen,
            [
                "FJ .......*.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   helloXYZrld                                                                  ",
                "********************************************************************************",
            ],
        )

    def test_indent(self):
        self._test_enter_editor_and_type(
            CTRL_O + CTRL_S + b"       >" + CTRL_M + b"hello",
            [
                "FJ        >.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "..        >.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "          hello                                                                 ",
                "********************************************************************************",
            ],
        )

    def test_indent_and_margin_release(self):
        self._test_enter_editor_and_type(
            CTRL_O
            + CTRL_S
            + b"       >"
            + CTRL_M
            + b"hello"
            + CTRL_O
            + CTRL_X
            + b">>",
            [
                "FJ        >.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "..        >.......*.......*.......*.......*.......*.......*.......*.......*.<   ",
                "   >>     hello                                                                 ",
                "********************************************************************************",
            ],
        )

    def test_load_horse_and_enter_editor_then_exit_with_escape(self):
        self.proc.read_until(b"=>", timeout=0.5)
        self.proc.writeline("LOAD examples/horse.v")
        self.proc.read_until(b"=>", timeout=1.0)
        self.proc.writeline("")
        self.proc.read_until(b"\x05", timeout=2.0)
        self.proc.write(b"\x1b")
        output = self.proc.read_until(b"=>", timeout=2.0)
        screen = pyte.Screen(80, 24)
        stream = pyte.Stream(screen)
        stream.feed(output.replace(b"\x05", b"").decode("latin-1"))
        expected_re = [
            r"VIEW B3\.0 for CP/M-65\s+",
            r"\s+",
            r"Bytes free \d+\s+",
            r"Editing examples/horse\.v\s+",
            r"=>\s+",
        ]
        for i, exp in enumerate(expected_re):
            self.assertRegex(
                screen.display[i],
                exp,
                f"Row {i} after ESCAPE exit: expected pattern {repr(exp)}, got {repr(screen.display[i])}",
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
            "          'Twas brillig, and the slithy toves                                   ",
            "                  did gyre and gimble in the wabe;                              ",
            "          All mimsy were the borogroves,                                        ",
            "                  and the mome raths outgrabe.                                  ",
            "                                                                                ",
            '          "Beware the Jabberwock, my son!                                       ',
            "                  The jaws that bite, the claws that catch!                     ",
            "          Beware the Jubjub bird, and shun                                      ",
            "                  the frumious Bandersnatch!                                    ",
            "                                                                                ",
            "          He took his vorpal sword in hand:                                     ",
            "                  long time the maxome foe he sought---                         ",
            "          So rested he by the Tumtum tree,                                      ",
            "                  and stood awhile in thought.                                  ",
            "                                                                                ",
            "          And as in uffish thought he stood,                                    ",
        ]
        self._assert_screen_lines(screen, expected)


if __name__ == "__main__":
    unittest.main()
