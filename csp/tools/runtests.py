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
import csp

os.environ.setdefault('CSPLOG_FILE', os.path.join(csp.dir, '.testlog'))

import csp.csplib


def LoadOneModule(path):
	if not csp.csplib.TestRegistry.loadTestModule(path):
		print 'Unable to load test module "%s".  Check that it has been built.' % path
		sys.exit(0)

def LoadModules(modules):
	for module in modules:
		LoadOneModule(module)

if __name__ == '__main__':
	if len(sys.argv) > 1:
		LoadModules(sys.argv[1:])
		csp.csplib.TestRegistry.runAll()
	else:
		print 'A simple utility to run CSP test modules manually.'
		print
		print 'Note that scons is generally a better tool for running tests (e.g.,'
		print 'use "scons runtests" to build and run all tests).'
		print
		print 'Usage: %s test_module [test_module ...]' % sys.argv[0]

