#!/usr/bin/env python

# Copyright 2005 Mark Rose <mkrose@users.sourceforge.net>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

"""
Combat Simulator Project - Test Runner
"""

import os
import sys

MODULES = [
	'csplib/.bin/csplib_tests',
]

try:
	import csp
	os.environ.setdefault('CSPLOG_FILE', os.path.join(csp.dir, '.testlog'))
	import csp.csplib
except ImportError:
	print 'Unable to import csp.csplib.  Check that it is built and that the'
	print 'bootstrap module has been installed.  See README for details.'
	sys.exit(0)


def LoadOneModule(path):
	# todo pass in modules from scons -- this code is not very portable (e.g., .sl on hpux)
	if sys.platform.startswith('win'):
		ext = '.dll'
	else:
		ext = '.so'
	path = os.path.join(csp.dir, path + ext)
	if not csp.csplib.TestRegistry.loadTestModule(path):
		print 'Unable to load test module "%s".  Check that it has been built.' % path
		sys.exit(0)

def LoadModules(modules):
	for module in modules:
		LoadOneModule(module)

def runall():
	LoadModules(MODULES)
	csp.csplib.TestRegistry.runAll()

if __name__ == '__main__':
	LoadModules(MODULES)
	if len(sys.argv) > 1:
		for path in sys.argv[1:]:
			csp.csplib.TestRegistry.runOnePath(path)
	else:
		csp.csplib.TestRegistry.runAll()

