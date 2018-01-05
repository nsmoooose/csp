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
import os
import sys

dir = os.path.abspath(__path__[0])


def _configureModules():
    if sys.platform.startswith('win') or os.name == 'nt':
        # if CSPDEVPACK is defined in the environment, add the devpack bin
        # directory to the execution path.  this ensures that devpack libraries
        # will be found before other (potentially incompatible) versions of the
        # same libraries.  note that windows is currently the only system with
        # a devpack.
        path = [
            os.path.join(dir, 'csplib', '.bin'),
            os.path.join(dir, 'cspsim', '.bin'),
            os.path.join(dir, 'cspwf', '.bin'),
        ]
        devpack = os.environ.get('CSPDEVPACK')
        if devpack:
            bin = os.path.join(devpack, 'bin')
            if not os.path.exists(bin):
                # (usr/bin is deprecated)
                bin = os.path.join(devpack, 'usr', 'bin')
            path.append(bin)
        if 'PATH' in os.environ:
            path.append(os.environ.get('PATH'))
        os.environ['PATH'] = os.pathsep.join(path)


_configureModules()
