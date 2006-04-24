# makedemo.py: A script to help automate generation of CSPSim demo releases.
# Copyright 2005 The Combat Simulator Project.

import sys
import shutil
import os.path
import re
import time
from glob import glob

def error(msg):
	print >>sys.stderr, msg
	sys.exit(1)

USAGE = """\
CSPSim demo generation script.

Usage: %s version [options]
"""

FURTHER_INSTRUCTIONS = """\
Please edit README and LICENSE in %(DEMO)s as necessary.
In particular, pay close attention to the versions of third party libraries
are being packaged in the demo and any changes to the copyright notices or
licensing terms.  Finally, create a zip archive of the entire demo directory
named %(DEMO)s.zip.
"""

# redirect csp log output if not otherwise specified.
os.environ.setdefault('CSPLOG_FILE', 'makedemo.log')

# ensure that the devpack binaries are found first in case the
# PATH is not set properly.
CSPDEVPACK = os.environ.get('CSPDEVPACK')
if CSPDEVPACK:
	CSPDEVPACK_BIN = os.path.join(CSPDEVPACK, 'bin')
	PATH = os.environ.get('PATH', '')
	if PATH.split(os.pathsep)[0] != CSPDEVPACK_BIN:
		print >>sys.stderr, 'WARNING: placing %s at start of PATH' % CSPDEVPACK_BIN
		os.environ['PATH'] = '%s;%s' % (CSPDEVPACK_BIN, os.environ.get('PATH', ''))

try:
	import csp
	import csp.csplib
	import csp.cspsim
except ImportError, e:
	error('ERROR: unable to import csp packages. %s\nHave you built everything?' % e)

BASE = os.path.dirname(csp.__file__)
DATA = os.path.join(BASE, 'data')
BIN = os.path.join(BASE, 'bin')

CSPSIM_BIN = os.path.join(BASE, 'cspsim', '.bin')
CSPLIB_BIN = os.path.join(BASE, 'csplib', '.bin')

try:
	import modulefinder
except ImportError, e:
	error('ERROR: unable to import modulefinder %s' % e)

# modulefinder needs a bit of help to deal with the .bin __path__
# redirections in csp.csplib and csp.cspsim.
modulefinder.AddPackagePath('csp.cspsim', CSPSIM_BIN)
modulefinder.AddPackagePath('csp.csplib', CSPLIB_BIN)

# in addition to the package paths, sys.path needs to be tweaked
# so that modulefinder can import the swig extensions in .bin.
sys.path.append(CSPLIB_BIN)
sys.path.append(CSPSIM_BIN)

from distutils.core import setup

try:
	import py2exe
except ImportError, e:
	error('ERROR: unable to import py2exe. %s\nIs it installed?' % e)


def copy_tree(src, dst, symlinks=0, exclude=None):
	"""Just like shutil.copytree, but allows files to be excluded based on a regexp."""
	if isinstance(exclude, str): exclude = re.compile(exclude)
	names = os.listdir(src)
	os.mkdir(dst)
	errors = []
	for name in names:
		if exclude and exclude.search(name): continue
		srcname = os.path.join(src, name)
		dstname = os.path.join(dst, name)
		try:
			if symlinks and os.path.islink(srcname):
				linkto = os.readlink(srcname)
				os.symlink(linkto, dstname)
			elif os.path.isdir(srcname):
				copy_tree(srcname, dstname, symlinks, exclude)
			else:
				shutil.copy2(srcname, dstname)
		except (IOError, os.error), why:
			errors.append((srcname, dstname, why))
	if errors:
		raise Error, errors


# TODO force msvcrt80.dll and msvcrp80.dll to be included?
def make_demo(version):
	TEMPLATE = 'template'
	DEMO = 'csp-demo-%s' % version

	if not os.path.exists(TEMPLATE):
		error('%s not found!' % TEMPLATE)

	if os.path.exists(DEMO):
		error('%s already exists!  Aborting.' % DEMO)

	CSPDEVPACK = os.environ.get('CSPDEVPACK')
	if CSPDEVPACK is None:
		error('CSPDEVPACK environment variable is not set')
	DP_BIN = os.path.join(CSPDEVPACK, 'bin')

	print 'Copying data from %s to %s' % (TEMPLATE, DEMO)
	copy_tree(TEMPLATE, DEMO, exclude=r'^\.svn$')

	DIST_DIR = os.path.join(DEMO, 'bin')
	print 'Running py2exe to create %s' % DIST_DIR

	TARGET = os.path.join(BIN, 'sim.py')
	CONFIG = os.path.join(BIN, 'sim.ini')
	TEST_OBJECTS = os.path.join(BIN, 'test_objects')

	opts = {
		'py2exe': {
			'excludes': ['dl'],
			'dist_dir': DIST_DIR,
			'packages': ['encodings'],
			'dll_excludes': ['msvcp80.dll', 'msvcr80.dll'],
		}
	}
	windows = {
		'script': TARGET,
		'icon_resources':[(1, 'logo.ico')]
	}

	setup(options=opts, windows=[windows], console=[TARGET], data_files=[CONFIG, TEST_OBJECTS])

	# not all dependencies can be found by py2exe, so ensure that all
	# dlls in the devpack are copied.  same for csp modules.
	DEVPACK_DLLS = glob(os.path.join(DP_BIN, '*.dll'))
	print os.path.join(BASE, 'modules', '*', '.bin', '*.dll')
	MODULES = glob(os.path.join(BASE, 'modules', '*', '.bin', '*.dll'))
	for src in MODULES + DEVPACK_DLLS:
		dest = os.path.join(DIST_DIR, os.path.basename(src))
		if not os.path.exists(dest) or os.path.getmtime(dest) < os.path.getmtime(src):
			print 'Copying', src
			shutil.copy2(src, DIST_DIR)

	DATA_TARGET = os.path.join(DEMO, 'data')
	if not os.path.exists(DATA_TARGET):
		print 'Copying data from %s to %s' % (DATA, DATA_TARGET)
		copy_tree(DATA, DATA_TARGET, exclude=r'^\.svn$')

	print 'Writing README header'
	README = os.path.join(DEMO, 'README')
	readme = open(README).read()
	open(README, 'wt').write(readme % {'VERSION': version, 'DATE': time.strftime('%Y-%m-%d')})

	print 'Basic setup complete!\n'
	print FURTHER_INSTRUCTIONS % {'DEMO': DEMO}


def usage():
	print USAGE % sys.argv[0]


def main():
	args = sys.argv[1:]
	if len(args) < 1:
		usage()
		sys.exit(1)

	version = args[0]
	sys.argv[1:] = ['py2exe'] + args[1:]

	if not re.match(r'\d+\.\d+', version):
		error('Invalid version "%s"; should be of the form X.Y (e.g., "0.5")' % version)

	make_demo(version)


if __name__ == '__main__':
	main()
