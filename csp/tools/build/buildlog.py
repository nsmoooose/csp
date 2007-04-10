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
import time

if __name__ == '__main__':
	print 'This module provides support for the SCons build system; it is not'
	print 'intended to be run as a standalone program.'
	sys.exit(1)


from csp.tools.build import scons

import SCons.Action
import SCons.Builder


_BUILDLOG = None
_SPAWN = None


class _Tee:
	"""Generalized "T" for writing to multiple file objects."""
	def __init__(self, *out):
		self._out = out
	def write(self, data):
		for out in self._out:
			out.write(data)


def _MakeLogHook(pspawn, log):
	"""
	Return a spawn function suitable for env['SPAWN'].   The function logs the
	command string and then calls pspawn to execute the command, which writes
	stdout and stderr to the log.
	"""
	stdout = _Tee(log, sys.stdout)
	stderr = _Tee(log, sys.stderr)
	def _spawn_wrapper(sh, escape, cmd, cargs, env):
		Log(' '.join(cargs))
		return pspawn(sh, escape, cmd, cargs, env, stdout, stderr)
	return _spawn_wrapper


def _InstallBuildLogHook(env):
	"""
	Replaces env['SPAWN'] with a wrapper function to record command lines
	and output to _BUILDLOG.  Requires env['PSPAWN'] to be set.  Returns
	nonzero if the hook was installed.
	"""
	global _BUILDLOG, _SPAWN

	if _BUILDLOG is not None: return 0

	pspawn = env.get('PSPAWN')
	if pspawn is None:
		print >>sys.stderr, 'env[PSPAWN] undefined; cannot save build log.'
		return 0

	_BUILDLOG =  open(scons.File('#/.buildlog').abspath, 'wt')
	_SPAWN, env['SPAWN'] = env['SPAWN'], _MakeLogHook(pspawn, _BUILDLOG)
	return 1


def _UninstallHook(env):
	"""
	Removes the hook installed by _InstallBuildLogHook, if any, restoring
	the original value of env['SPAWN'] and closing the build log.
	"""
	global _BUILDLOG, _SPAWN
	if _SPAWN is not None:
		env['SPAWN'] = _SPAWN
		_SPAWN = None
	if _BUILDLOG:
		_BUILDLOG.close()
		_BUILDLOG = None


def InitializeLogging(env):
	"""Open the build log file and configure scons to write to it."""
	_InstallBuildLogHook(env)


def SetShortMessages(env):
	"""
	Add short message strings that are displayed when running various
	SCons builders, replacing the raw command strings.  This function
	is intended to be used with 'scons -Q' to produce more readable
	build output.  Note that the function only simplifies messages
	written to stdout, not the entries in the build log.
	"""
	env['CCCOMSTR'] = '- Compiling $SOURCE'
	env['SHCCCOMSTR'] = '- Compiling $SOURCE (shared)'
	env['CXXCOMSTR'] = '- Compiling $SOURCE'
	env['SHCXXCOMSTR'] = '- Compiling $SOURCE (shared)'
	env['LINKCOMSTR'] = '- Linking $TARGET'
	env['SHLINKCOMSTR'] = '- Linking $TARGET (shared)'
	env['SWIGCOMSTR'] = '- Generating $TARGET (swig)'
	env['GENERATECOMSTR'] = '- Generating $TARGET'
	env['TRCCOMSTR'] = '- Generating $TARGET (trc)'
	env['DOXYGENCOMSTR'] = '- Generating documentation for $SOURCE.dir'
	env['COPYCOMSTR'] = '- Copying $SOURCE to $TARGET'
	env['SYMLINKCOMSTR'] = '- Creating symlink $TARGET'


def Log(message):
	"""
	Write a message (and timestamp) to the build log.  InitializeLogging must
	be called first.
	"""
	if _BUILDLOG:
		_BUILDLOG.write(time.strftime('[%Y%m%d %H:%M:%S] '))
		_BUILDLOG.write(message)
		_BUILDLOG.write('\n')
