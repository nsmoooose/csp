#!/usr/bin/python
# Copyright 2005 The Combat Simulator Project
# http://csp.sourceforge.net
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

import os
import os.path
import sys
import traceback

USAGE = """
FCS Generator - A tool for generating flight control system networks.

Parses network definitions specified using Python to produce an XML
description for use with the FlightControlSystem class.

Usage: %s networkpackage

Here network package is a Python package containing one or more files
that use the NodeConstructor module to define a flight control system.
On success the program writes XML definitions to stdout.
"""

def usage():
	print >>sys.stderr, USAGE % os.path.basename(sys.argv[0])
	sys.exit(1)

argv = sys.argv
if len(argv) != 2: usage()
source = argv[1]
if source.endswith(os.sep): source = source[:-1]
sys.path.insert(0, os.path.dirname(source))
module = os.path.basename(source)

try:
	__import__(module)
except ImportError, e:
	print >>sys.stderr, 'Error importing package %s: %s' % (source, e)
	sys.exit(1)
except Exception, e:
	tb = sys.exc_traceback
	while tb.tb_next: tb = tb.tb_next
	frame = tb.tb_frame
	print >>sys.stderr, "Error on line %d of module %s:" % (frame.f_lineno, frame.f_globals.get('__name__'))
	print >>sys.stderr, "==> %s: %s" % (str(e.__class__).split('.', 1)[1], e)
	sys.exit(1)

import NodeConstructor
NodeConstructor.GenerateNetwork(os.path.abspath(source))

sys.exit(0)
