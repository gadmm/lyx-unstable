# About this version

This branch contains bug fixes and new features on top of LyX master (2.4),
detailed at the end of this file.

Please report bugs at <https://github.com/gadmm/lyx-unstable/issues>.

For Ubuntu this is distributed as part of LyX unstable on the PPA:
<https://code.launchpad.net/~gadmm/+archive/ubuntu/lyx-unstable>.

# What is LyX?

LyX is a document processor and equation editor that encourages an approach to
writing based on the structure of your documents, rather than their appearance.
It is released under a Free Software / Open Source license.

LyX is for people who write and want their writing to look great, right out of
the box. No more endless tinkering with formatting details, “finger painting”
font attributes or futzing around with page boundaries. You just write. LyX's
semantic approach helps your article, thesis or book mature from the early
scribbles, to a draft full of notes and change-tracked revisions, to a final
camera-ready version.

In the background, Prof. Knuth's legendary TeX typesetting engine makes you look
good. LyX's output—or richly cross-referenced PDF, just as readily
produced—looks like nothing else. Gone are the days of industrially bland .docs,
all looking similarly not-quite-right, yet coming out unpredictably different on
different printer drivers. Gone are the crashes “eating” your dissertation the
evening before going to press.

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
* Fix flicker due to changing metrics while selecting with the mouse. (#8951)
* Do not randomly delete aux files while compiling. (#9963)

## New features

* Smooth scrolling.
* Performance improvements for scrolling, especially large insets.
* Option to invert colors (i.e. dark color theme). (#8325)
* Improve the drawing of many math insets.
* Better display for High-dpi screens: lines and drawings are now proportional
  to the zoom level.
* Better display for Low-dpi screens: lines and drawings are now antialiased.
* Beamer-flex: Layout for beamer that uses flex insets instead of environments
  for frames and similar environments, complete with documentation, templates
  and examples. This solves common issues reported by Beamer users. (The layout
  is available from <https://github.com/gadmm/beamer-flex> for backwards-
  compatibility.)
* A new Custom insets menu is available from the toolbar, useful for Beamer-flex.
  The button remembers which inset was entered last.
* New command to split an inset (inset-split), useful for Beamer-flex.
* Layout and templates for easychair.cls. (Also available from
  <https://github.com/gadmm/easychair-lyx>.)
* Improved default parameters and shortcuts, more beginner-friendly.
* Open hyperlinks in application from the contextual menu in a secure way.
  (#9906)

## Minor bug fixes

* Fix LyX's amnesia of last cursor positions when there are too many files.
  (#10310)
* Instant preview now works in child documents. (#9868)
* Fix hard-to-read label color in math.
* Fix a missing repaint of the cursor.
* Fix the toolbar button "math-insert \sideset".
* Fix spurious space in output of \mkern.
* Fix math class of \{ and \}.
* The "apply" button now correctly saves preferences. (#10016)
* Precautionary measure against a potential data loss during emergency save.
* Fix automatically loaded packages with acmart.
* Properly enforce scrolling boundaries.
* Properly reset references and counters when updating citations.
* Properly reset selection when inputting a composed character.

## Minor enhancements

* Module InStar: Add on-screen labels for InTitle and InPreamble environments.
* Moving tabs is now visible. (#10733)
* Minor improvement to the citation GUI.
* Add \x*arrows in Math Arrows toolbar menu.
* This file readable from the About menu.
