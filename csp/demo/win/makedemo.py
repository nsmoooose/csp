# makedemo.py: A script to help automate generation of CSPSim demo releases.
# Copyright 2005 The Combat Simulator Project.

import sys
import shutil
import os.path
import re
import time

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

import csp
import csp.csplib
import csp.cspsim

BASE = os.path.dirname(csp.__file__)
DATA = os.path.join(BASE, 'data')
BIN = os.path.join(BASE, 'bin')

# not sure which if any of these and the AddPackagePath statements below are
# needed for the new directory structure.  some experimentation will probably
# be necessary.

#CSPLIB = os.path.join(BASE, 'csplib')
#CSPSIM_BIN = os.path.join(BASE, 'cspsim', '.bin')
#sys.path.append(CSPSIM_BIN)

try:
	import modulefinder
	#modulefinder.AddPackagePath('csp.cspsim', BIN)
	#modulefinder.AddPackagePath('csp.cspsim', CSPSIM_BIN)
	#modulefinder.AddPackagePath('csp', CSPLIB)
except ImportError:
	print 'WARNING: unable to import modulefinder'


from distutils.core import setup
import py2exe


def error(msg):
	print >>sys.stderr, msg
	sys.exit(1)


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


def make_demo(version):
	TEMPLATE = 'template'
	DEMO = 'cspsim-demo-%s' % version

	if not os.path.exists(TEMPLATE):
		error('%s not found!' % TEMPLATE)

	if os.path.exists(DEMO):
		error('%s already exists!  Aborting.' % DEMO)

	print 'Copying data from %s to %s' % (TEMPLATE, DEMO)
	copy_tree(TEMPLATE, DEMO, exclude=r'^\.svn$')

	DIST_DIR = os.path.join(DEMO, 'bin')
	print 'Running py2exe to create %s' % DIST_DIR

	opts = {
		'py2exe': {
			'excludes': ['dl'],
			'dist_dir': DIST_DIR
		}
	}

	TARGET = os.path.join(BIN, 'sim.py')
	CONFIG = os.path.join(BIN, 'sim.ini')

	setup(options=opts, console=[TARGET], data_files=[CONFIG])

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
