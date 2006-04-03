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

dir = os.path.abspath(__path__[0])

def _configureModules():
	if os.name == 'posix':
		try:
			import dl
			# enable lazy loading of shared library modules if available.
			sys.setdlopenflags(dl.RTLD_GLOBAL|dl.RTLD_LAZY)
		except ImportError:
			sys.stderr.write('import dl failed; lazy module loading not enabled.\n')
	else:
		# if CSPDEVPACK is defined in the environment, add the devpack bin
		# directory to the execution path.  this ensures that devpack libraries
		# will be found before other (potentially incompatible) versions of the
		# same libraries.  note that windows is currently the only system with
		# a devpack.
		devpack = os.environ.get('CSPDEVPACK')
		if devpack:
			bin = os.path.join(devpack, 'usr', 'bin')
			path = [bin]
			if 'PATH' in os.environ:
				path.append(os.environ.get('PATH'))
			os.environ['PATH'] = os.pathsep.join(path)

_configureModules()
