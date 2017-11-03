# What is LyX unstable?

LyX unstable is a branch of LyX master that reads, edits and writes the current
LyX stable file format. See the documentation at
`development/unstable/unstable.lyx` for more information on the why and how. LyX
*2.3unstable* edits LyX files with the 2.2 file format and follows the current
master branch (2.4dev). When LyX 2.3 releases, unstable will be reset to the LyX
2.3 format.

This branch contains bug fixes and new features on top of LyX master (2.4),
detailed at the end of this file.

Please report bugs at <https://github.com/gadmm/lyx-unstable/issues>.

For Ubuntu this is distributed on the PPA:
<https://code.launchpad.net/~gadmm/+archive/ubuntu/lyx-unstable>.

# What is LyX?

LyX is a document processor that encourages an approach to writing based on the
structure of your documents, not their appearance. It is released under a Free
Software / Open Source license.

LyX is for people that write and want their writing to look great, right out of
the box. No more endless tinkering with formatting details, “finger painting”
font attributes or futzing around with page boundaries. You just write. In the
background, Prof. Knuth's legendary TeX typesetting engine makes you look good.

On screen, LyX looks like any word processor; its printed output — or richly
cross-referenced PDF, just as readily produced — looks like nothing else. Gone
are the days of industrially bland .docs, all looking similarly not-quite-right,
yet coming out unpredictably different on different printer drivers. Gone are
the crashes “eating” your dissertation the evening before going to press.

LyX is stable and fully featured. It is a multi-platform, fully
internationalized application running natively on Unix/Linux and the Macintosh
and modern Windows platforms.

# Bug fixes and new features

For a complete list of changes try
<https://github.com/gadmm/lyx-unstable/compare/master...staging>.

## Important bug fixes

* Buggy event compressor causing from slowness to deadlock and dataloss on
  repeated key presses with iBus (Ubuntu in particular). (#9362, #9790, #10516)
* Crash with segfault when exiting or dissolving math insets.
* Scrolling issues with keyboard across large insets.

## New features

* Smooth scrolling.
* Option to invert colors (i.e. dark color theme). (#8325)
* Better display for High-dpi screens: lines and drawings are now proportional to
  the zoom level.
* Better display for Low-dpi screens: lines and drawings are now antialiased.
* Beamer-flex: Layout and examples for beamer that uses flex insets instead of
  environments for frames and similar insets. (Also available from
  <https://github.com/gadmm/beamer-flex>.)
* Layout and templates for easychair.cls. (Also available from
  <https://github.com/gadmm/easychair-lyx>.)
* Custom insets are now available from the toolbar.
* Improved default parameters and shortcuts.

## Minor bug fixes

* Fix LyX's amnesia of last cursor positions when there are too many files (#10310)
* Instant preview now works in child documents. (#9868)
* Fix hard-to-read label color in math.
* Fix a missing repaint of the cursor.

## Minor enhancements

* Module InStar: Add on-screen labels for InTitle and InPreamble environments.
* Moving tabs is now visible (#10733).
* Minor improvement to the citation GUI.
* Performance improvements for scrolling.
* Add \x*arrows in Math Arrows toolbar menu.
* Fix vertical and horizontal spacings for decorations (\hat, \overbrace...).
* Fix the toolbar button "math-insert \sideset".
* This file readable from the About menu.

