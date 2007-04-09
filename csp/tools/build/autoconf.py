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

from csp.tools.build import util
from csp.tools.build import scons

import os
import pickle
import re

from distutils import sysconfig

GetPythonInclude = sysconfig.get_python_inc
GetPythonLibrary = sysconfig.get_python_lib


def CheckSConsVersion(minimum):
	version = scons.GetVersion()
	if util.CompareVersions(version, minimum) < 0:
		print ('SCons version %s is too old.  Please install'
		       'version %s or newer.' % (version, minimum))
		sys.exit(1)


def CheckPythonVersion(minimum):
	version = '.'.join(map(str, sys.version_info[:3]))
	if util.CompareVersions(version, minimum) < 0:
		print ('Python version %s is too old.  Please install'
		       'version %s or newer.' % (version, minimum))
		sys.exit(1)

def GetGCCVersion():
	version = os.popen('gcc -dumpversion').read().strip()
	try:
		return tuple(map(int, version.split('.')))
	except:
		return None

def CheckSwig(context, min_version, not_versions=[]):
	ok = 0
	_checking(context, 'swig')
	swig_in, swig_out, swig_err = os.popen3('%s -version' % context.env.get('SWIG', 'swig'), 't')
	if swig_err is not None:
		output = swig_err.readlines() + swig_out.readlines()
		output = " ".join(map(lambda x: x.strip(), output))
		match = re.search(r'SWIG Version (\d+\.\d+.\d+)', output)
		if match is not None:
			swig_version = match.groups()[0]
			if util.CompareVersions(swig_version, min_version) >= 0:
				ok = 1
				for nv in not_versions:
					if util.CompareVersions(swig_version, nv) == 0:
						ok = 0
	if ok:
		context.Result("yes (%s)" % swig_version)
		context.env['SWIG_VERSION'] = swig_version
	else:
		context.Result("no")
	return ok


def CheckLibVersion(context, name, source, min_version):
	_checking(context, name, min_version)
	old_LIBS = context.env.get('LIBS', '')
	context.env.Append(LIBS=[name])
	success, output = context.TryRun(source, '.c')
	version = output.strip()
	if success and util.CompareVersions(version, min_version) >= 0:
		context.Result("yes (%s)" % version)
		context.env['%s_VERSION' % name.upper()] = version
	else:
		context.Result("no")
		context.env.Replace(LIBS=old_LIBS)
	return success


def CheckOSGVersion(context, lib, min_version):
	OSG_VERSION_CHECK = '''
	/* Override any gcc2 internal prototype to avoid an error.  */
	#ifdef __cplusplus
	extern "C"
	#endif
	#include <stdio.h>
	#include <stdlib.h>
	const char *%(lib)sGetVersion(); /**/
	int main() {
		printf("%%s", %(lib)sGetVersion());
		exit(0);
		return 0;
	}
	''' % {'lib': lib}
	return CheckLibVersion(context, lib, OSG_VERSION_CHECK, min_version)


def CheckPkgConfig(context, lib, version=None, lib_name=None, command='pkg-config', version_flag='--modversion', config_flags='--cflags --libs'):
	if lib_name is None: lib_name = lib
	env = context.env
	_checking(context, lib, version)
	output = os.popen('%s %s %s' % (command, version_flag, lib))
	ok = 0
	if output is not None:
		lib_version = output.readline().strip()
		ok = (version is None) or (util.CompareVersions(lib_version, version) >= 0)
	if ok:
		context.Result("yes (%s)" % lib_version)
		env['%s_VERSION' % lib_name.upper()] = lib_version
		env.ParseConfig('%s %s %s' % (command, config_flags, lib))
	else:
		context.Result("no")
	return ok


def _checking(context, target, min_version=None):
	if min_version is None:
		context.Message('Checking for %s... ' % (target))
	else:
		context.Message('Checking for %s (>= %s)... ' % (target, min_version))


def CheckCommandVersion(context, lib, command, min_version=None, lib_name=None):
	if lib_name is None: lib_name = lib
	_checking(context, lib, min_version)
	output = os.popen(command)
	ok = output is not None
	if ok:
		version = output.readline().strip()
		ok = (util.CompareVersions(version, min_version) >= 0)
	if ok:
		context.Result("yes (%s)" % version)
		context.env['%s_VERSION' % lib_name.upper()] = version
	else:
		context.Result("no")
	return ok


def CustomConfigure(env):
	scons.SetReading()
	scons.DisableQuiet()
	conf = env.Configure(log_file="#/.config.log")
	conf.AddTests({'CheckSwig': CheckSwig})
	conf.AddTests({'CheckLibVersion': CheckLibVersion})
	conf.AddTests({'CheckOSGVersion': CheckOSGVersion})
	conf.AddTests({'CheckCommandVersion': CheckCommandVersion})
	conf.AddTests({'CheckPkgConfig': CheckPkgConfig})
	return conf


def SetConfig(env, config):
	if config:
		if ('config' in scons.GetCommandlineTargets()) or ReadConfig(env):
			if not env.GetOption('clean'):
				config(env)
				WriteConfig(env)


def ReadConfig(env):
	config_file = env.GetBuildPath('.config')
	try:
		config = open(config_file, 'rt')
	except IOError:
		return None
	# hack to rerun the config if the SConstruct file is modified; this can
	# probably be done more cleanly using regular scons dependencies.  also
	# shouldn't hardcode the name 'SConstruct' here.
	t0 = scons.FS.getmtime(scons.File('#/SConstruct').abspath)
	t1 = scons.FS.getmtime(scons.File('#/.config').abspath)
	if t0 > t1: return None
	return pickle.load(config)


def SaveConfig(env, config):
	config_file = env.GetBuildPath('.config')
	pickle.dump(config, open(config_file, 'wt'))

