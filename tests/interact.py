#!/usr/bin/env python3
"""Integration tests for the VIEW text editor via PTY."""

import re

from utils import *


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
