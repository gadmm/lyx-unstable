#! /usr/bin/env python
# -*- coding: utf-8 -*-

# file generate_symbols_images.py
# This file is part of LyX, the document processor.
# Licence details can be found in the file COPYING.

# author Georg Baum
# author Juergen Spitzmueller (adaptation for SVG)

# Full author contact details are available in file CREDITS

# This script generates a toolbar image for each missing math symbol.
# It needs the template document generate_symbols_images.lyx for generating
# the png image via preview.sty and dvipng, or the template document
# generate_symbols_svg.lyx for generating the SVG image via dvisvgm.
# Either document must contain the placeholder formula '$a$'.
# The created images are not always optimal, therefore the existing manually
# created images should never be replaced by automatically created ones.


from __future__ import print_function
import os, re, string, sys, subprocess, tempfile, shutil
import Image
import io

def usage(prog_name):
    return ("Usage: %s png|svg lyxexe outputpath\n" % prog_name)


def error(message):
    sys.stderr.write(message + '\n')
    sys.exit(1)


def getlist(lyxexe, lyxfile):
    """ Call LyX and get a list of symbols from mathed debug output.
        This way, we can re-use the symbols file parser of LyX, and do not
        need to reimplement it in python. """

    # The debug is only generated if lyxfile contains a formula
    cmd = "%s %s -dbg mathed -x lyx-quit" % (lyxexe, lyxfile)
    proc = subprocess.Popen(cmd, shell=True, stderr=subprocess.PIPE)
    (stdout, stderr) = proc.communicate()
    regexp = re.compile(r'.*: read symbol \'(\S+)\s+inset:\s+(\S+)\s+draw:\s+(\S*)\s+extra:\s+(\S+)')
    # These insets are more complex than simply symbols, so the images need to
    # be created manually
    skipinsets = ['big', 'font', 'lyxblacktext', 'matrix', 'mbox', 'oldfont', \
                  'ref', 'split', 'space', 'style']
    mathsymbols = []
    textsymbols = []
    for line in stderr.split('\n'):
        m = regexp.match(line)
        if m:
            inset = m.group(2)
            if not inset in skipinsets:
                if m.group(4) == 'textmode':
                    textsymbols.append(m.group(1))
                else:
                    mathsymbols.append(m.group(1))
    return (mathsymbols, textsymbols)


def getreplacements(filename):
    replacements = {}
    replacements['|'] = 'vert'
    replacements['/'] = 'slash'
    replacements['\\'] = 'backslash'
    replacements['*'] = 'ast'
    replacements['AA'] = 'textrm_AA'
    replacements['O'] = 'textrm_O'
    cppfile = io.open(filename, 'r', encoding='utf_8')
    regexp = re.compile(r'.*"([^"]+)",\s*"([^"]+)"')
    found = False
    for line in cppfile.readlines():
        if found:
            m = regexp.match(line)
            if m:
                replacements[m.group(1)] = m.group(2)
            else:
                return replacements
        elif line.find('ImgMap sorted_img_map') == 0:
            found = True


def gettoolbaritems(filename):
    items = []
    uifile = io.open(filename, 'r', encoding='utf_8')
    regexp = re.compile(r'.*Item "([^"\[]+)(\[\[[^\]]+\]\])?"\s*"math-insert\s+([^"]+)"')
    for line in uifile.readlines():
        m = regexp.match(line)
        if m:
            if '\\' + m.group(1) == m.group(3):
                items.append(m.group(1))
    return items


def getmakefileentries(filename):
    items = []
    makefile = io.open(filename, 'r', encoding='utf_8')
    regexp = re.compile(r'.*images/math/(.+)\.(png|svgz)')
    for line in makefile.readlines():
        m = regexp.match(line)
        if m:
            items.append(m.group(1))
    return items


def createimage(name, path, template, lyxexe, tempdir, math, replacements, toolbaritems, makefileentries, usepng):
    """ Create the image file for symbol name in path. """

    if name in replacements.keys():
        filename = replacements[name]
    elif name.startswith('lyx'):
        print('Skipping ' + name)
        return
    else:
        skipchars = ['|', '/', '\\', '*', '!', '?', ':', ';', '^', '<', '>']
        for i in skipchars:
            if name.find(i) >= 0:
                print('Skipping ' + name)
                return
        filename = name
    if usepng:
        imgname = os.path.join(path, filename + '.png')
    else:
        imgname = os.path.join(path, filename + '.svgz')
    if name in toolbaritems:
        if filename in makefileentries:
            suffix = ' (found in toolbar and makefile)'
        else:
            suffix = ' (found in only in toolbar)'
    else:
        if filename in makefileentries:
            suffix = ' (found only in makefile)'
        else:
            suffix = ' (not found)'
    if os.path.exists(imgname):
        print('Skipping ' + name + suffix)
        return
    print('Generating ' + name + suffix)
    lyxname = os.path.join(tempdir, filename)
    lyxfile = io.open(lyxname + '.lyx', 'w', encoding='utf_8')
    if math:
        lyxfile.write(template.replace('$a$', '$\\' + name + '$'))
    else:
        lyxfile.write(template.replace('$a$', '$\\text{\\' + name + '}$'))
    lyxfile.close()
    cmd = "%s %s.lyx -e dvi" % (lyxexe, lyxname)
    proc = subprocess.Popen(cmd, shell=True)
    proc.wait()
    if proc.returncode != 0:
        print('Error in DVI creation for ' + name)
        return
    # The magnifaction factor is calculated such that we get an image of
    # height 18 px for most symbols and document font size 11. Then we can
    # add a small border to get the standard math image height of 20 px.
    if usepng:
        cmd = "dvipng %s.dvi -bg Transparent -D 115 -o %s" % (lyxname, imgname)
    else:
        cmd = "dvisvgm -z --no-fonts --exact --output=%s%s%%f %s.dvi" % (path, os.path.sep, lyxname)
    proc = subprocess.Popen(cmd, shell=True)
    proc.wait()
    if proc.returncode != 0:
        if png:
            print('Error in PNG creation for ' + name)
        else:
            print('Error in SVG creation for ' + name)
        return
    if not usepng:
        return
    image = Image.open(imgname)
    (width, height) = image.size
    if width < 20 and height < 20:
        if width == 19 and height == 19:
            padded = Image.new('RGBA', (width+1, height+1), (0, 0, 0, 0))
            padded.paste(image, (0, 0))
        elif width == 19:
            padded = Image.new('RGBA', (width+1, height+2), (0, 0, 0, 0))
            padded.paste(image, (0, 1))
        elif height == 19:
            padded = Image.new('RGBA', (width+2, height+1), (0, 0, 0, 0))
            padded.paste(image, (1, 0))
        else:
            padded = Image.new('RGBA', (width+2, height+2), (0, 0, 0, 0))
            padded.paste(image, (1, 1))
        padded.convert(image.mode)
        padded.save(imgname, "PNG")


def main(argv):

    if len(argv) == 4:
        (base, ext) = os.path.splitext(argv[0])
        (mathsymbols, textsymbols) = getlist(argv[2], base)
        cppfile = os.path.join(os.path.dirname(base), '../../src/frontends/qt4/GuiApplication.cpp')
        replacements = getreplacements(cppfile)
        uifile = os.path.join(os.path.dirname(base), '../../lib/ui/stdtoolbars.inc')
        toolbaritems = gettoolbaritems(uifile)
        makefile = os.path.join(os.path.dirname(base), '../../lib/Makefile.am')
        makefileentries = getmakefileentries(makefile)
        if argv[1] == 'png':
            lyxtemplate = base + '.lyx'
            usepng = True
        elif argv[1] == 'svg':
            lyxtemplate = os.path.join(os.path.dirname(base), 'generate_symbols_svg.lyx')
            usepng = False
        else:
            error(usage(argv[0]))
        templatefile = io.open(base + '.lyx', 'r', encoding='utf_8')
        template = templatefile.read()
        templatefile.close()
        tempdir = tempfile.mkdtemp()
        for i in mathsymbols:
            createimage(i, argv[3], template, argv[2], tempdir, True, replacements, toolbaritems, makefileentries, usepng)
        for i in textsymbols:
            createimage(i, argv[3], template, argv[2], tempdir, False, replacements, toolbaritems, makefileentries, usepng)
        shutil.rmtree(tempdir)
    else:
        error(usage(argv[0]))

    return 0


if __name__ == "__main__":
    main(sys.argv)
