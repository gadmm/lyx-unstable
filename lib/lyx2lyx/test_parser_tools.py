# This file is part of lyx2lyx
# -*- coding: utf-8 -*-
# Copyright (C) 2006 José Matos <jamatos@lyx.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

" This modules tests the functions used to help parse lines."

from parser_tools import *

import unittest

ug = r"""
\begin_layout Standard
The
\begin_inset Quotes eld
\end_inset


\emph on
Introduction
\emph default

\begin_inset Quotes erd
\end_inset

 describes several things in addition to LyX's philosophy: most importantly,
 the format of all of the manuals.
 If you don't read it, you'll have a bear of a time navigating this manual.
 You might also be better served looking in one of the other manuals instead
 of this one.

\begin_inset Quotes eld
\end_inset


\emph on
Introduction
\emph default

\begin_inset Quotes erd
\end_inset

 describes that, too.
\end_layout

"""

lines = ug.splitlines()

class TestParserTools(unittest.TestCase):

    def test_check_token(self):
        line = "\\begin_layout Standard"

        self.assertEqual(check_token(line, '\\begin_layout'), True)
        self.assertEqual(check_token(line, 'Standard'), False)


    def test_is_nonempty_line(self):
        self.assertEqual(is_nonempty_line(lines[0]), False)
        self.assertEqual(is_nonempty_line(lines[1]), True)
        self.assertEqual(is_nonempty_line(" "*5), False)


    def test_find_token(self):
        self.assertEqual(find_token(lines, '\\emph', 0), 7)
        # no line starts with "emph" (without backspace):
        self.assertEqual(find_token(lines, 'emph', 0), -1)
        # token on line[start] is found:
        self.assertEqual(find_token(lines, '\\emph', 7), 7)
        self.assertEqual(find_token(lines, '\\emph', 8), 9)
        # token on line[end] is not found:
        self.assertEqual(find_token(lines, '\\emph', 0, 7), -1)
        # `ignorews` looks for whitespace-separated tokens:
        self.assertEqual(find_token(lines, '\\emp', 0, ignorews=True), -1)
        self.assertEqual(find_token(lines, '\\emph',0, ignorews=True), 7)
        self.assertEqual(find_token(lines, '\\emph', 7, ignorews=True), 7)
        self.assertEqual(find_token(lines, '\\emph', 0, 7, True), -1)
        # only first token is found:
        self.assertEqual(find_token(lines, 'Quotes', 0), -1)
        self.assertEqual(find_token(lines, 'Quotes', 0, ignorews=True), -1)


    def test_find_tokens(self):
        tokens = ['\\emph', '\\end_inset']
        self.assertEqual(find_tokens(lines, tokens, 0), 4)
        self.assertEqual(find_tokens(lines, tokens, 0, 4), -1)


    def test_find_complete_lines(self):
        sublines = ["\\begin_inset Quotes eld",
                    "\\end_inset"]
        # return index of first line of sublines:
        self.assertEqual(find_complete_lines(lines, sublines), 3)
        self.assertEqual(find_complete_lines(lines, ["\\end_inset"]), 4)
        # return -1 if sublines is not found:
        self.assertEqual(find_complete_lines(lines, ['x']), -1)
        # search includes line `start`:
        self.assertEqual(find_complete_lines(lines, sublines, 3), 3)
        self.assertEqual(find_complete_lines(lines, sublines, 4), 20)
        self.assertEqual(find_complete_lines(lines, sublines, 21), -1)
        # serch excludes line `end`
        self.assertEqual(find_complete_lines(lines, sublines, 4, 20), -1)
        # an empty list is always found
        self.assertEqual(find_complete_lines(lines, []), 0)


    def test_find_across_lines(self):
        # sub with at least 2 line-breaks (uses find_complete_lines):
        sub = "Quotes eld\n\\end_inset\n\n\n"
        self.assertEqual(find_across_lines(lines, sub), 3)
        # Return -1 if not found
        self.assertEqual(find_across_lines(lines, sub, 4), -1)
        self.assertEqual(find_across_lines(lines, sub, 0, 6), -1)
        sub = "Quotes eld\n\\end_inset\nx\n"
        self.assertEqual(find_across_lines(lines, sub), -1)
        sub = "Quotes X\n\\end_inset\n\n"
        self.assertEqual(find_across_lines(lines, sub), -1)
        sub = "Quotes eld\n\\end_insert\n\n"
        self.assertEqual(find_across_lines(lines, sub), -1)
        # sub with up to 1 line-break:
        sub = "Quotes eld\n\\end_inset"
        self.assertEqual(find_across_lines(lines, sub), 3)
        self.assertEqual(find_across_lines(lines, sub, 4), -1)
        self.assertEqual(find_across_lines(lines, sub, 0, 4), -1)
        self.assertEqual(find_across_lines(lines, sub, 4, 3), -1)
        sub = "Quotes X eld\n\\end_inset\n"
        self.assertEqual(find_across_lines(lines, sub), -1)
        sub = "Quotes eld\n\\end_insert\n"
        self.assertEqual(find_across_lines(lines, sub), -1)
        # sub without line-break
        sub = "end_"
        self.assertEqual(find_across_lines(lines, sub), 4)
        self.assertEqual(find_across_lines(lines, sub, 5), 12)
        self.assertEqual(find_across_lines(lines, sub, 0, 4), -1)
        self.assertEqual(find_across_lines(lines, sub, 2, 1), -1)
        self.assertEqual(find_across_lines(lines, "XXX"), -1)


    def test_get_value(self):
        self.assertEqual(get_value(lines, "\\begin_inset"), "Quotes eld")
        # TODO: do we want this:
        self.assertEqual(get_value(lines, "\\begin_inset Quotes"), "Quotes eld")
        # or only the part after "token":
        #  self.assertEqual(get_value(lines, "\\begin_inset Quotes"), "eld")
        # return default if not found
        self.assertEqual(get_value(lines, "\\begin_insert", default=42), 42)
        # TODO: do we want this:
        self.assertEqual(get_value(lines, "\\end_inset", default=None), None)
        # or emtpy string if token is found but has no value:
        #  self.assertEqual(get_value(lines, "\\end_inset", default=None), "")


    def test_del_complete_lines(self):
        l = lines[:]
        sublines = ["\\begin_inset Quotes eld",
                    "\\end_inset"]
        # normal operation: remove the first occurence of sublines:
        self.assertEqual(del_complete_lines(l, sublines), True)
        self.assertEqual(l[3], "")
        self.assertEqual(len(l), len(lines)-len(sublines))
        # special cases:
        l = lines[:]
        self.assertEqual(del_complete_lines(l, sublines, 21), False)
        self.assertEqual(l, lines)
        # deleting empty sublist returns success but does not change the list:
        self.assertEqual(del_complete_lines(l, [], 21), True)
        self.assertEqual(l, lines)

    def test_del_value(self):
        l = lines[:]
        self.assertEqual(del_value(l, "\\begin_inset"), "Quotes eld")
        self.assertEqual(del_value(l, "\\begin_inset Quotes"), "erd")
        # return default if not found
        self.assertEqual(del_value(l, "\\begin_insert", default=42), 42)
        self.assertEqual(del_value(l, "\\end_inset", default=None), "")


if __name__ == '__main__':
    unittest.main()
