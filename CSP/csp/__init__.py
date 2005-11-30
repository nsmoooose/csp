# Combat Simulator Project - CSPSim
# Copyright (C) 2002, 2004 The Combat Simulator Project
# http://csp.sourceforge.net
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


import sys
import os
import os.path

# ugly kludge to allow us to import dl.  the bootstrap loader removes the
# original contents of sys.path, and stashes the full path in sys.CSP_PATH.
# if it has been set we restore the path here and clear the sys attribute.
if len(sys.path) == 1 and hasattr(sys, 'CSP_PATH'):
	sys.path = sys.CSP_PATH
	del sys.CSP_PATH

dirname = __path__[0]

# bring compiled modules into the csp package
__path__.insert(0, os.path.join(dirname, 'cspsim', '.bin'))

def initDynamicLoading():
	"""Enable lazy loading of shared library modules if available"""
	if os.name == 'posix':
		import dl
		sys.setdlopenflags(dl.RTLD_GLOBAL|dl.RTLD_LAZY)

initDynamicLoading()

