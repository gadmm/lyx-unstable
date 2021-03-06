# What is LyX unstable?

LyX unstable is a branch of LyX master that reads, edits and writes the current
LyX stable file format. See the documentation at
`development/unstable/unstable.lyx` for more information on the why and how. LyX
*2.4unstable* edits LyX files with the 2.3 file format and follows the current
master branch (2.4dev). When LyX 2.4 releases, unstable will be reset to the LyX
2.4 format.

* Current LyX format: 544. LyX unstable also reads files with a more recent
  format thanks to the up-to-date `lyx2lyx` conversion script.
* Current local layout format: 66. LyX unstable will accept to read and write
  local layouts with a more recent format, but Format 66 has to be used if the
  file has to remain readable by LyX 2.3 stable.

This branch contains bug fixes and new features on top of LyX master (2.4dev),
detailed at the end of this file. We try to catch and fix showstopping bugs from
master before merging into unstable, but we cannot guarantee it.

Please report bugs at <https://github.com/gadmm/lyx-unstable/issues>.

For Ubuntu this is distributed on the PPA:
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

LyX unstable contains bug fixes and new features from LyX 2.3 and 2.4dev
* <https://wiki.lyx.org/LyX/NewInLyX23>
* <https://wiki.lyx.org/LyX/NewInLyX24>

with the exception that features requiring a newer file format have been
disabled.

In addition it contains the changes below. For a complete list of these changes
try <https://github.com/gadmm/lyx-unstable/compare/master...staging>.

For a (sort of) chronological view try
<https://github.com/gadmm/lyx-unstable/blame/unstable/README.md>.

## Important bug fixes

* Scrolling issues with keyboard across large insets.
* Fix flicker due to changing metrics while selecting with the mouse. (#8951)
* Do not randomly delete aux files while compiling. (#9963)
* Fix loss of last opened files information.
* Fix random scolling inside nested insets when the window is resized (e.g. math
  toolbars are shown).

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
* Add visibility and revocability to the new “needauth” feature, according to
  recommended practices in secure usability.

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
* Improve the behaviour on external modifications: notifications are
  less invasive, it is now possible to reload all children at once, and there
  should no longer be any false positive.
* Restore normal caret ascent and descent in math.
* Fix math selection being reset when leaving math inset.
* Fix a performance issues and some spurious warning when exporting with math.
* Work around buffer error during painting.

## Minor enhancements

* Module InStar: Add on-screen labels for InTitle and InPreamble environments.
* Moving tabs is now visible. (#10733)
* Minor improvement to the citation GUI.
* Add \x*arrows in Math Arrows toolbar menu.
* This file readable from the About menu.
