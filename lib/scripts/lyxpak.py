# -*- coding: utf-8 -*-

# file lyxpak.py
# This file is part of LyX, the document processor.
# Licence details can be found in the file COPYING.

# author Enrico Forestieri
# author Richard Heck

# Full author contact details are available in file CREDITS

# This script creates a tar or zip archive with a lyx file and all included
# files (graphics and so on). By default, the created archive is the standard
# type on a given platform, such that a zip archive is created on Windows and
# a gzip compressed tar archive on *nix. This can be controlled by command
# line options, however.

from __future__ import print_function
import gzip, os, re, sys
from getopt import getopt
from io import BytesIO
import subprocess

# Provide support for both python 2 and 3
if sys.version_info[0] != 2:
    def unicode(arg, enc):
        return arg

# The path to the current python executable. sys.executable may fail, so in
# this case we revert to simply calling "python" from the path.
PYTHON_BIN = sys.executable if sys.executable else "python"

running_on_windows = (os.name == 'nt')

if running_on_windows:
    from shutil import copyfile
    from tempfile import NamedTemporaryFile

# Pre-compiled regular expressions.
re_lyxfile = re.compile(b"\.lyx$")
re_input = re.compile(b'^(.*)\\\\(input|include){(\\s*)(.+)(\\s*)}.*$')
re_ertinput = re.compile(b'^(input|include)({)(\\s*)(.+)(\\s*)}.*$')
re_package = re.compile(b'^(.*)\\\\(usepackage){(\\s*)(.+)(\\s*)}.*$')
re_class = re.compile(b'^(\\\\)(textclass)(\\s+)(.+)\\s*$')
re_norecur = re.compile(b'^(.*)\\\\(verbatiminput|lstinputlisting|includegraphics\\[*.*\\]*){(\\s*)(.+)(\\s*)}.*$')
re_ertnorecur = re.compile(b'^(verbatiminput|lstinputlisting|includegraphics\\[*.*\\]*)({)(\\s*)(.+)(\\s*)}.*$')
re_filename = re.compile(b'^(\\s*)(filename)(\\s+)(.+)\\s*$')
re_options = re.compile(b'^(\\s*)options(\\s+)(.+)\\s*$')
re_bibfiles = re.compile(b'^(\\s*)bibfiles(\\s+)(.+)\\s*$')


def usage(prog_name):
    msg = '''
Usage: %s [-t] [-z] [-l path] [-o output_dir] file.lyx
Options:
-l: Path to lyx2lyx script
-o: Directory for output
-t: Create gzipped tar file
-z: Create zip file
By default, we create file.zip on Windows and file.tar.gz on *nix,
with the file output to where file.lyx is, and we look for lyx2lyx
in the known locations, querying LyX itself if necessary.
'''
    return msg % prog_name


def error(message):
    sys.stderr.write(message + '\n')
    sys.exit(1)


def tostr(message):
    return message.decode(sys.getfilesystemencoding())


def gzopen(file):
    input = open(file.decode('utf-8'), 'rb')
    magicnum = input.read(2)
    input.close()
    if magicnum == b"\x1f\x8b":
        return gzip.open(file.decode('utf-8'))
    return open(file.decode('utf-8'), 'rb')


def find_exe(candidates, extlist, path):
    for prog in candidates:
        for directory in path:
            for ext in extlist:
                full_path = os.path.join(directory, prog + ext)
                if os.access(full_path, os.X_OK):
                    return prog, full_path
    return None, None


def abspath(name):
    " Resolve symlinks and returns the absolute normalized name."
    newname = os.path.normpath(os.path.abspath(name))
    if not running_on_windows:
        newname = os.path.realpath(newname)
    return newname


def gather_files(curfile, incfiles, lyx2lyx):
    " Recursively gather files."
    curdir = os.path.dirname(abspath(curfile))
    is_lyxfile = re_lyxfile.search(curfile)

    if is_lyxfile:
        if running_on_windows:
            # subprocess cannot cope with unicode arguments and we cannot be
            # sure that curfile can be correctly converted to the current
            # code page. So, we resort to running lyx2lyx on a copy.
            tmp = NamedTemporaryFile(delete=False)
            tmp.close()
            copyfile(curfile.decode('utf-8'), tmp.name)
            try:
                l2l_stdout = subprocess.check_output([PYTHON_BIN, lyx2lyx, tmp.name])
            except subprocess.CalledProcessError:
                error('%s failed to convert "%s"' % (lyx2lyx, tostr(curfile)))
            os.unlink(tmp.name)
        else:
            try:
                l2l_stdout = subprocess.check_output([PYTHON_BIN, lyx2lyx, curfile])
            except subprocess.CalledProcessError:
                error('%s failed to convert "%s"' % (lyx2lyx, tostr(curfile)))
        if l2l_stdout.startswith(b"\x1f\x8b"):
            l2l_stdout = gzip.GzipFile("", "rb", 0, BytesIO(l2l_stdout)).read()
        elif running_on_windows:
            # For some unknown reason, there can be a spurious '\r' in the line
            # separators, causing spurious empty lines when calling splitlines.
            l2l_stdout = l2l_stdout.replace('\r\r\n', '\r\n')
        lines = l2l_stdout.splitlines()
    else:
        input = gzopen(curfile)
        lines = input.readlines()
        input.close()

    maybe_in_ert = False
    i = 0
    while i < len(lines):
        # Gather used files.
        recursive = True
        extlist = [b'']
        match = re_filename.match(lines[i])
        if not match:
            if maybe_in_ert:
                match = re_ertinput.match(lines[i])
            else:
                match = re_input.match(lines[i])
            if not match:
                match = re_package.match(lines[i])
                extlist = [b'.sty']
                if not match:
                    match = re_class.match(lines[i])
                    extlist = [b'.cls']
                    if not match:
                        if maybe_in_ert:
                            match = re_ertnorecur.match(lines[i])
                        else:
                            match = re_norecur.match(lines[i])
                        extlist = [b'', b'.eps', b'.pdf', b'.png', b'.jpg']
                        recursive = False
        maybe_in_ert = is_lyxfile and lines[i] == b"\\backslash"
        if match:
            file = match.group(4).strip(b'"')
            if not os.path.isabs(file):
                file = os.path.join(curdir, file)
            file_exists = False
            if not os.path.isdir(unicode(file, 'utf-8')):
                for ext in extlist:
                    if os.path.exists(unicode(file + ext, 'utf-8')):
                        file = file + ext
                        file_exists = True
                        break
            if file_exists and not abspath(file) in incfiles:
                incfiles.append(abspath(file))
                if recursive:
                    gather_files(file, incfiles, lyx2lyx)
            i += 1
            continue

        if not is_lyxfile:
            i += 1
            continue

        # Gather bibtex *.bst files.
        match = re_options.match(lines[i])
        if match:
            file = match.group(3).strip(b'"')
            if file.startswith(b"bibtotoc,"):
                file = file[9:]
            if not os.path.isabs(file):
                file = os.path.join(curdir, file + b'.bst')
            if os.path.exists(unicode(file, 'utf-8')):
                incfiles.append(abspath(file))
            i += 1
            continue

        # Gather bibtex *.bib files.
        match = re_bibfiles.match(lines[i])
        if match:
            bibfiles = match.group(3).strip(b'"').split(b',')
            j = 0
            while j < len(bibfiles):
                if os.path.isabs(bibfiles[j]):
                    file = bibfiles[j] + b'.bib'
                else:
                    file = os.path.join(curdir, bibfiles[j] + b'.bib')
                if os.path.exists(unicode(file, 'utf-8')):
                    incfiles.append(abspath(file))
                j += 1
            i += 1
            continue

        i += 1

    return 0


def find_lyx2lyx(progloc, path):
    " Find a usable version of the lyx2lyx script. "
    # first we will see if the script is roughly where we are
    # i.e., we will assume we are in $SOMEDIR/scripts and look
    # for $SOMEDIR/lyx2lyx/lyx2lyx.
    ourpath = os.path.dirname(abspath(progloc))
    (upone, discard) = os.path.split(ourpath)
    tryit = os.path.join(upone, "lyx2lyx", "lyx2lyx")
    if os.access(tryit, os.X_OK):
        return tryit

    # now we will try to query LyX itself to find the path.
    extlist = ['']
    if "PATHEXT" in os.environ:
        extlist = extlist + os.environ["PATHEXT"].split(os.pathsep)
    lyx_exe, full_path = find_exe(["lyxc", "lyx"], extlist, path)
    if lyx_exe == None:
        error('Cannot find the LyX executable in the path.')
    try:
        cmd_stdout = subprocess.check_output([lyx_exe, '-version'], stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError:
        error('Cannot query LyX about the lyx2lyx script.')
    re_msvc = re.compile(r'^(\s*)(Host type:)(\s+)(win32)$')
    re_sysdir = re.compile(r'^(\s*)(LyX files dir:)(\s+)(\S+)$')
    lines = cmd_stdout.splitlines()
    for line in lines:
        match = re_msvc.match(line)
        if match:
            # The LyX executable was built with MSVC, so the
            # "LyX files dir:" line is unusable
            basedir = os.path.dirname(os.path.dirname(full_path))
            tryit = os.path.join(basedir, 'Resources', 'lyx2lyx', 'lyx2lyx')
            break
        match = re_sysdir.match(line)
        if match:
            tryit = os.path.join(match.group(4), 'lyx2lyx', 'lyx2lyx')
            break

    if not os.access(tryit, os.X_OK):
        error('Unable to find the lyx2lyx script.')
    return tryit


def main(args):

    ourprog = args[0]

    try:
      (options, argv) = getopt(args[1:], "htzl:o:")
    except:
      error(usage(ourprog))

    # we expect the filename to be left
    if len(argv) != 1:
        error(usage(ourprog))

    makezip = running_on_windows
    outdir = ""
    lyx2lyx = None

    for (opt, param) in options:
      if opt == "-h":
        print(usage(ourprog))
        sys.exit(0)
      elif opt == "-t":
        makezip = False
      elif opt == "-z":
        makezip = True
      elif opt == "-l":
        lyx2lyx = param
      elif opt == "-o":
        outdir = param
        if not os.path.isdir(unicode(outdir, 'utf-8')):
          error('Error: "%s" is not a directory.' % outdir)

    lyxfile = argv[0]
    if not running_on_windows:
        lyxfile = unicode(lyxfile, sys.getfilesystemencoding()).encode('utf-8')
    if not os.path.exists(unicode(lyxfile, 'utf-8')):
        error('File "%s" not found.' % tostr(lyxfile))

    # Check that it actually is a LyX document
    input = gzopen(lyxfile)
    line = input.readline()
    input.close()
    if not (line and line.startswith(b'#LyX')):
        error('File "%s" is not a LyX document.' % tostr(lyxfile))

    if makezip:
        import zipfile
    else:
        import tarfile

    ar_ext = b".tar.gz"
    if makezip:
        ar_ext = b".zip"

    ar_name = re_lyxfile.sub(ar_ext, abspath(lyxfile)).decode('utf-8')
    if outdir:
        ar_name = os.path.join(abspath(outdir), os.path.basename(ar_name))

    path = os.environ["PATH"].split(os.pathsep)

    if lyx2lyx == None:
        lyx2lyx = find_lyx2lyx(ourprog, path)

    # Initialize the list with the specified LyX file and recursively
    # gather all required files (also from child documents).
    incfiles = [abspath(lyxfile)]
    gather_files(lyxfile, incfiles, lyx2lyx)

    # Find the topmost dir common to all files
    path_sep = os.path.sep.encode('utf-8')
    if len(incfiles) > 1:
        topdir = os.path.commonprefix(incfiles)
        # As os.path.commonprefix() works on a character by character basis,
        # rather than on path elements, we need to remove any trailing bytes.
        topdir = topdir.rpartition(path_sep)[0] + path_sep
    else:
        topdir = os.path.dirname(incfiles[0]) + path_sep

    # Remove the prefix common to all paths in the list
    i = 0
    while i < len(incfiles):
        incfiles[i] = incfiles[i].replace(topdir, b'', 1)
        i += 1

    # Remove duplicates and sort the list
    incfiles = list(set(incfiles))
    incfiles.sort()

    if topdir != '':
        os.chdir(unicode(topdir, 'utf-8'))

    # Create the archive
    try:
        if makezip:
            zip = zipfile.ZipFile(ar_name, "w", zipfile.ZIP_DEFLATED)
            for file in incfiles:
                zip.write(file.decode('utf-8'))
            zip.close()
        else:
            tar = tarfile.open(ar_name, "w:gz")
            for file in incfiles:
                tar.add(file.decode('utf-8'))
            tar.close()
    except:
        error('Failed to create LyX archive "%s"' % ar_name)

    print('LyX archive "%s" created successfully.' % ar_name)
    return 0


if __name__ == "__main__":
    if running_on_windows:
        # This works around <http://bugs.python.org/issue2128> for Python 2.
        # All arguments are retrieved in unicode format and converted to utf-8.
        # In this way, when launched from the command line, lyxpak.py can deal
        # with any non-ascii names. Unfortunately, this is not the case when
        # launched by LyX, because LyX converts the arguments of the converters
        # to the filesystem encoding. On Windows this corresponds to the current
        # code page and not to the UTF-16 encoding used by NTFS, such that they
        # are transliterated if not exactly encodable. As an example, α may
        # become a, β may become ß, and so on. However, this is a problem only
        # if the full path of the LyX document contains an unencodable character
        # as all other paths are extracted from the document in utf-8 format.
        from ctypes import WINFUNCTYPE, windll, POINTER, byref, c_int
        from ctypes.wintypes import LPWSTR, LPCWSTR
        GetCommandLineW = WINFUNCTYPE(LPWSTR)(("GetCommandLineW", windll.kernel32))
        CommandLineToArgvW = WINFUNCTYPE(POINTER(LPWSTR), LPCWSTR, POINTER(c_int))(("CommandLineToArgvW", windll.shell32))
        argc = c_int(0)
        argv_unicode = CommandLineToArgvW(GetCommandLineW(), byref(argc))
        # unicode_argv[0] is the Python interpreter, so skip that.
        argv = [argv_unicode[i].encode('utf-8') for i in xrange(1, argc.value)]
        # Also skip option arguments to the Python interpreter.
        while len(argv) > 0:
            if not argv[0].startswith("-"):
                break
            argv = argv[1:]
        sys.argv = argv

    main(sys.argv)
