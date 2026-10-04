"""Shared constants and utilities for VIEW editor tests."""

import os

VIEW_BIN = os.path.join(
    os.path.dirname(__file__), "..", "bin", "view_for_testing"
)

# Control codes used in editor commands
CTRL_A = b"\x01"
CTRL_B = b"\x02"
CTRL_C = b"\x03"
CTRL_D = b"\x04"
CTRL_E = b"\x05"
CTRL_F = b"\x06"
CTRL_G = b"\x07"
CTRL_H = b"\x08"
CTRL_I = b"\x09"
CTRL_J = b"\x0a"
CTRL_K = b"\x0b"
CTRL_L = b"\x0c"
CTRL_M = b"\x0d"
CTRL_N = b"\x0e"
CTRL_O = b"\x0f"
CTRL_P = b"\x10"
CTRL_Q = b"\x11"
CTRL_R = b"\x12"
CTRL_S = b"\x13"
CTRL_T = b"\x14"
CTRL_U = b"\x15"
CTRL_V = b"\x16"
CTRL_W = b"\x17"
CTRL_X = b"\x18"
CTRL_Y = b"\x19"
CTRL_Z = b"\x1a"

# Screen key codes (from screen.h)
KEY_UP = b"\x8b"
KEY_DOWN = b"\x8a"
KEY_LEFT = b"\x88"
KEY_RIGHT = b"\x89"
KEY_BACKSPACE = b"\x7f"