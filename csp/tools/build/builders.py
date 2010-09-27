# Copyright 2004-2006 Mark Rose <mkrose@users.sourceforge.net>
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


import sys

if __name__ == '__main__':
	print 'This module provides support for the SCons build system; it is not'
	print 'intended to be run as a standalone program.'
	sys.exit(1)


from csp.tools.build import scons

import os
import re
import shutil

import SCons.Builder
import SCons.Scanner
import SCons.Tool


def AddCopyFile(env):
	def copy(target, source, env):
		source, target = source[0].abspath, target[0].abspath
		shutil.copy(source, target)
	CopyFile = SCons.Builder.Builder(action=SCons.Action.Action(copy, '$COPYFILECOMSTR'))
	env.Append(BUILDERS = {'CopyFile': CopyFile})


def AddLinkFile(env):
	def symlink(target, source, env):
		source, target = source[0].abspath, target[0].abspath
		if os.name=='posix':
			os.link(source, target)
		else:
			shutil.copy(source, target)
	LinkFile = SCons.Builder.Builder(action=SCons.Action.Action(symlink, '$SYMLINKCOMSTR'))
	env.Append(BUILDERS = {'LinkFile': LinkFile})



