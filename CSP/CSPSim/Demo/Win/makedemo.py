# makedemo.py: A script to help automate generation of CSPSim demo releases.
# Copyright 2005 The Combat Simulator Project.

import sys
import shutil
import os.path
import re
import time

USAGE = """\
CSPSim demo generation script.

Usage: %s version
"""

FURTHER_INSTRUCTIONS = """\
Please edit README and LICENSE in %(DEMO)s as necessary.
In particular, pay close attention to the versions of third party libraries
are being packaged in the demo and any changes to the copyright notices or
licensing terms.  Finally, create a zip archive of the entire demo directory
named %(DEMO)s.zip.
"""


import CSP
import CSP.SimData
import CSP.CSPSim

BASE = os.path.dirname(CSP.__file__)
CSPSIM_DATA = os.path.join(BASE, 'CSPSim', 'Data')
CSPSIM_BIN = os.path.join(BASE, 'CSPSim', 'Bin')
CSPSIM_BIN_SOURCE = os.path.join(BASE, 'CSPSim', '.bin', 'Source')
SIMDATA = os.path.join(BASE, 'SimData')

try:
	import modulefinder
	modulefinder.AddPackagePath("CSP.CSPSim", CSPSIM_BIN)
	modulefinder.AddPackagePath("CSP.CSPSim", CSPSIM_BIN_SOURCE)
	modulefinder.AddPackagePath("CSP.SimData", SIMDATA)
except ImportError:
	print "WARNING: unable to import modulefinder"


from distutils.core import setup
#import py2exe


def error(msg):
	print >>sys.stderr, msg
	sys.exit(1)


def make_demo(version):
	TEMPLATE = 'CSPSim-Demo'
	DEMO = 'CSPSim-Demo-%s' % version

	if not os.path.exists(TEMPLATE):
		error('%s not found!' % TEMPLATE)

	if os.path.exists(DEMO):
		error('%s already exists!  Aborting.' % DEMO)

	print 'Copying data from %s to %s' % (TEMPLATE, DEMO)
	shutil.copytree(TEMPLATE, DEMO)

	DIST_DIR = os.path.join(DEMO, 'Bin')
	print 'Running py2exe to create %s' % DIST_DIR

	opts = {
		"py2exe": {
			"excludes": ['SimData', 'dl'],
			"dist_dir": DIST_DIR
		}
	}

	TARGET = os.path.join(CSPSIM_BIN, 'CSPSim.py')
	CONFIG = os.path.join(CSPSIM_DATA, 'CSPSim.ini')

	setup(options=opts, console=[TARGET], data_files=[CONFIG])

	DATA_TARGET = os.path.join(DEMO, 'Data')
	if not os.path.exists(DATA_TARGET):
		print 'Copying data from %s to %s' % (CSPSIM_DATA, DATA_TARGET)
		shutil.copytree(CSPSIM_DATA, DATA_TARGET)

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
	if len(args) != 1:
		usage()
		sys.exit(1)

	version = args[0]

	if not re.match(r'\d+\.\d+', version):
		error('Invalid version "%s"; should be of the form X.Y (e.g., "0.5")' % version)

	make_demo(version)


if __name__ == '__main__':
	main()
