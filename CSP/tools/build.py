#!/usr/bin/python
#
# Copyright 2004 Mark Rose <mkrose@users.sourceforge.net>
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

import os
import re
import shutil
import pickle
import glob

from distutils import sysconfig

import SCons
import SCons.Builder
from SCons.Builder import Builder
from SCons.Script import SConscript
from SCons.Action import Action
from SCons.Scanner import Scanner
from SCons.Node.FS import default_fs

File = default_fs.File
Dir = default_fs.Dir


PYTHON_INCLUDE = sysconfig.get_python_inc()
PYTHON_LIBRARY = sysconfig.get_python_lib()

BUILD_LOG = open(File('#/.buildlog').abspath, 'wt')


############################################################################
# GENERAL HELPERS

def Glob(path):
	if type(path) == type(''): path = [path]
	result = []
	for p in path:
		result.extend(glob.glob(str(File(p))))
	return result


def AddPhonyTarget(env, target, message=''):
	def nop(target, source, env): pass
	def msg(target, source, env, message=message): return message
	env.Alias(target, Builder(action=Action(nop, msg))(env, target, ''))


def CompareVersions(a, b):
	return (a.find('.') > 0) and (b.find('.') > 0) and cmp(a.split('.'), b.split('.'))


def IsWindows(env):
	return env['PLATFORM'].startswith('win')


############################################################################
# SHORT MESSAGES

class ShortMessageWrapper:
	def __init__(self, message):
		self.message = message
		self.old_message = None

	def bind(self, action):
		if not isinstance(action.strfunction, ShortMessageWrapper):
			self.old_message = action.strfunction
			action.strfunction = self

	def __call__(self, target, source, env):
		if self.old_message and BUILD_LOG:
			old = self.old_message(target, source, env)
			print >>BUILD_LOG, old
			BUILD_LOG.flush()
		return self.message(target, source, env)


def TargetMessage(phrase, suffix=''):
	def printer(target, source, env, phrase=phrase, suffix=suffix):
		if type(target) == type([]):
			target = map(str, target)
			if len(target) == 1: target = target[0]
		print '- %s %s%s' % (phrase, target, suffix)
	return printer


def SourceMessage(phrase, suffix='', limit=None):
	def printer(target, source, env, phrase=phrase, suffix=suffix):
		if type(source) == type([]):
			source = map(str, source)
			if limit is not None and len(source) > limit:
				source = source[:limit]
			if len(source) == 1: source = source[0]
		print '- %s %s%s' % (phrase, source, suffix)
	return printer


def SourceToTargetMessage(phrase, suffix=''):
	def printer(target, source, env, phrase=phrase, suffix=suffix):
		source = map(str, source)
		target = map(str, target)
		print '- %s %s to %s %s' % (phrase, source[0], target[0], suffix)
	return printer


def SetShortMessage(env, builder, message):
	idx = builder.find(':')
	if idx > 0:
		extension = builder[idx+1:]
		builder = builder[:idx]
	else:
		extension = None
	builders = env['BUILDERS']
	if builders.has_key(builder):
		if extension is None:
			action = builders[builder].action
		else:
			action = builders[builder].cmdgen.get(extension, None)
			if action is None: return
			if type(action) == type(''):
				action = Action(action)
				builders[builder].add_action(extension, action)
		if isinstance(action, SCons.Action.ListAction):
			for a in action.get_actions():
				if a.strfunction is not None:
					ShortMessageWrapper(message).bind(a)
					return
		ShortMessageWrapper(message).bind(action)

def SetShortMessages(env):
	SetShortMessage(env, 'Program', TargetMessage('Building'))
	SetShortMessage(env, 'StaticObject', SourceMessage('Compiling'))
	SetShortMessage(env, 'SharedObject', SourceMessage('Compiling', suffix=' (shared)'))
	SetShortMessage(env, 'Object', SourceMessage('Compiling'))
	SetShortMessage(env, 'StaticLibrary', TargetMessage('Linking'))
	SetShortMessage(env, 'SharedLibrary', TargetMessage('Linking'))
	SetShortMessage(env, 'Doxygen', SourceMessage('Generating documentation', limit=1))
	SetShortMessage(env, 'Swig', SourceMessage('Generating SWIG interface', limit=1))
	SetShortMessage(env, 'CXXFile:.i', SourceMessage('Generating SWIG interface', limit=1))
	SetShortMessage(env, 'CXXFile:.net', SourceMessage('Generating tagged record', limit=1))
	SetShortMessage(env, 'LinkFile', SourceToTargetMessage('Creating link'))
	SetShortMessage(env, 'CopyFile', SourceToTargetMessage('Copying'))



############################################################################
# HIERARCHICAL BUILD HELPERS

def BuildPackages(env, packages, **kw):
	import build
	result = []
	for package in packages:
		kw['build_dir'] = '%s/.bin' % package
		kw.setdefault('duplicate', 0)
		kw.setdefault('exports', 'env build')
		targets = env.SConscript('%s/SConscript' % package, **kw)
		if targets:
			if type(targets) != type([]): targets = [targets]
			result.extend(targets)
	result = env.Flatten(result)
	return result

def BuildModules(env, modules, **kw):
	import build
	result = []
	for module in modules:
		kw.setdefault('exports', 'env build')
		targets = env.SConscript('%s/SConscript' % module, **kw)
		if targets:
			if type(targets) != type([]): targets = [targets]
			result.extend(targets)
	result = env.Flatten(result)
	return result



############################################################################
# AUTOCONF HELPEERS


def CheckSConsVersion(minimum):
	version = SCons.__version__
	if CompareVersions(version, minimum) < 0:
		print ('SCons version %s is too old.  Please install'
		       'version %s or newer.' % (version, minimum))
		sys.exit(1)


def CheckPythonVersion(minimum):
	version = '.'.join(map(str, sys.version_info[:3]))
	if CompareVersions(version, minimum) < 0:
		print ('Python version %s is too old.  Please install'
		       'version %s or newer.' % (version, minimum))
		sys.exit(1)


def CheckSwig(context, min_version, not_versions=[]):
	ok = 0
	_checking(context, 'swig')
	swig_in, swig_out, swig_err = os.popen3('%s -version' % context.env.get('SWIG', 'swig'), 't')
	if swig_err is not None:
		output = swig_err.readlines()
		output = " ".join(map(lambda x: x.strip(), output))
		match = re.search(r'SWIG Version (\d+\.\d+.\d+)', output)
		if match is not None:
			swig_version = match.groups()[0]
			if CompareVersions(swig_version, min_version) >= 0:
				ok = 1
				for nv in not_versions:
					if CompareVersions(swig_version, nv) == 0:
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
	if success and CompareVersions(version, min_version) >= 0:
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


def CheckPkgConfig(context, lib, version=None, lib_name=None):
	if lib_name is None: lib_name = lib
	env = context.env
	_checking(context, lib, version)
	output = os.popen('pkg-config --modversion %s' % lib)
	ok = 0
	if output is not None:
		lib_version = output.readline().strip()
		ok = (version is None) or (CompareVersions(lib_version, version) >= 0)
	if ok:
		context.Result("yes (%s)" % lib_version)
		env['%s_VERSION' % lib_name.upper()] = lib_version
		env.ParseConfig('pkg-config --cflags --libs %s' % lib)
	else:
		context.Result("no")


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
		ok = (CompareVersions(version, min_version) >= 0)
	if ok:
		context.Result("yes (%s)" % version)
		context.env['%s_VERSION' % lib_name.upper()] = version
	else:
		context.Result("no")
	return ok


def CustomConfigure(env):
	conf = env.Configure()
	conf.AddTests({'CheckSwig': CheckSwig})
	conf.AddTests({'CheckLibVersion': CheckLibVersion})
	conf.AddTests({'CheckOSGVersion': CheckOSGVersion})
	conf.AddTests({'CheckCommandVersion': CheckCommandVersion})
	conf.AddTests({'CheckPkgConfig': CheckPkgConfig})
	return conf


def WriteConfig(env):
	config_file = env.GetBuildPath('.config')
	config = open(config_file, 'wt')
	pickle.dump((env.get('LIBS', ''), env.get('CPPPATH', [])), config)
	return None


def ReadConfig(env):
	config_file = env.GetBuildPath('.config')
	try:
		config = open(config_file, 'rt')
	except IOError:
		return 1
	libs, cpppath = pickle.load(config)
	env['LIBS'] = libs
	env['CPPPATH'] = cpppath
	return 0


def SetConfig(env, config):
	if config:
		if ('config' in SConscript.CommandLineTargets) or ReadConfig(env):
			if not env.GetOption('clean'):
				config(env)
				WriteConfig(env)



############################################################################
# GLOBAL BUILD SETUP

def SetDistributed(env):
	CXX = env['CXX']
	if CXX is None: return
	distcc = SCons.Util.WhereIs('distcc')
	if not distcc:
		print 'Concurrent build requested, but distcc not found.'
		return
	CXX = '%s %s' % (distcc, CXX)
	# distcc needs access to configuration in the home directory
	env['ENV']['HOME'] = os.environ.get('HOME', '')
	env.Replace(CXX=CXX)
	print 'Using distcc for distributed build'


def UpdateEnvironment(env, config=None):
	if config:
		if ('config' in SConscript.CommandLineTargets) or ReadConfig(env):
			if not env.GetOption('clean'):
				config(env)
				WriteConfig(env)
	def nop(target, source, env): pass
	env.Append(BUILDERS = {'config': Builder(action=Action(nop, ''))})


def SetDefaultMessage(env, message):
	AddPhonyTarget(env, 'default', message)
	env.Default('default')


def GlobalSetup(env, distributed=1, short_messages=None, default_message=None, with_swig=1):
	import SCons.Script
	if default_message is not None:
		SetDefaultMessage(env, default_message)
	if with_swig:
		AddSwigSupport(env)
	if short_messages is None:
		short_messages = SCons.Script.options.no_progress
	AddBuilders(env)
	if short_messages:
		SetShortMessages(env)
	if distributed and SCons.Script.ssoptions.get('num_jobs') > 1:
		SetDistributed(env)
	AddPhonyTarget(env, 'config')
	from SCons.Script.SConscript import SConsEnvironment
	SConsEnvironment.SetConfig = SetConfig
	SConsEnvironment.Documentation = MakeDocumentation



############################################################################
# ENHANCED SWIG SUPPORT

def EmitSwig(target, source, env):
	target = []
	assert(len(source)==1)
	ext = env['CXXFILESUFFIX']
	for s in source:
		wrapper = s.target_from_source('', '_wrap'+ext)
		target.append(wrapper)
		# XXX Module name really should be based on the %module directive
		# in the interface file.
		module = s.target_from_source('', '.py')
		target.append(module)
	return (target, source)


def AddSwigBuild(env):
	env['SWIGCOM'] = '$SWIG $SWIGFLAGS $SWIGINCLUDES -o $TARGET $SOURCE'
	builder = Builder(action = '$SWIG $SWIGFLAGS $SWIGINCLUDES -o ${TARGETS[0]} $SOURCE', emitter = EmitSwig)
	env.Append(BUILDERS = {'Swig': builder})
	def wrapper_builder(env, target = None, source = SCons.Builder._null, **kw):
		kw.setdefault('CXXFLAGS', env.get('SWIGCXXFLAGS', env.get('CXXFLAGS', '')))
		return env.SharedObject(target, source, **kw)
	env.Append(BUILDERS = {'SwigWrapper': wrapper_builder })


def AddSwigDep(env):
	def SwigScanner(node, env, path, arg=None):
		cmd = env.subst('$SWIG -MM $_CPPINCFLAGS %s' % str(node))
		stdin, stdout, stderr = os.popen3(cmd, 't')
		deps = ''.join(map(lambda x: x.strip(), stdout.readlines()))
		deps = map(lambda x: "#/"+x.strip(), deps.split('\\'))[1:]
		return deps
	scanner = Scanner(name = 'SwigScanner', function = SwigScanner, skeys = ['.i'], recursive = 0)
	env.Append(SCANNERS = scanner)


SWIG = 'swig'

def AddSwigSupport(env):
	global SWIG
	SWIG = SCons.Util.WhereIs('swig')
	env['SWIG'] = SWIG
	AddSwigDep(env)
	AddSwigBuild(env)



############################################################################
# ADDITIONAL BUILDERS

def AddDoxygen(env):
	def report(target, source, env):
		source = str(source[0])
		return 'doxygen %s' % source
	action = 'cd $SOURCE.dir && doxygen $SOURCES.file 1>.dox.log 2>.dox.err'
	env.Append(BUILDERS = {'Doxygen': Builder(action=Action(action, report))})


def AddCopyFile(env):
	def copy(target, source, env):
		shutil.copy(str(source[0]), str(target[0]))
	def report(target, source, env):
		source = str(source[0])
		target = str(target[0])
		return 'copying %s -> %s' % (source, target)
	CopyFile = Builder(action=Action(copy, report))
	env.Append(BUILDERS = {'CopyFile': CopyFile})


def AddLinkFile(env):
	def copy(target, source, env):
		source = str(source[0])
		target = str(target[0])
		if os.name=='posix':
			os.link(source, target)
		else:
			shutil.copy(source, target)
	def report(target, source, env):
		source = str(source[0])
		target = str(target[0])
		return 'linking %s -> %s' % (source, target)
	CopyFile = Builder(action=Action(copy, report))
	env.Append(BUILDERS = {'LinkFile': CopyFile})


def EmitNet(target, source, env):
	assert(len(source)==1)
	source = source[0]
	name = os.path.splitext(source.name)[0]
	dir = source.srcnode().dir
	target = [
		dir.File(name + env['CXXFILESUFFIX']),
		dir.File(name + '.h'),
	]
	return (target, source)


def AddNet(env):
	cxx_file = env['BUILDERS']['CXXFile']
	cxx_file.add_emitter('.net', EmitNet)
	cxx_file.add_action('.net', '$TRC')
	trc = env.File('#/SimData/Tools/TaggedRecordCompiler/trc.py')
	env['TRC'] = '%s --source=${TARGETS[0]} --header=${TARGETS[1]} $SOURCES' % trc


def AddBuilders(env):
	AddDoxygen(env)
	AddCopyFile(env)
	AddLinkFile(env)
	AddNet(env)


def MarkVersionSource(env, target, prefix='Version.'):
	if type(target) == type([]): target = target[0]
	version = [child for child in target.children() if str(child).startswith(prefix)]
	others = [child for child in target.children() if not str(child).startswith(prefix)]
	if version: env.Depends(version, others)


def MakeDocumentation(env, target, source, *args):
	target = Dir(target)
	html = target.Dir('html')
	if 'dox' in SConscript.CommandLineTargets:
		source = File(source)
		index = html.File('index.html')
		dox = env.Doxygen(index, source)
		[env.Depends(dox, Glob(g)) for g in args]
		env.Alias('dox', dox)
	env.Clean(['dox', 'all'], html)

def Apply(builder, sources, **overrides):
	def builder_wrap(source):
		return builder(source, **overrides)
	return map(builder_wrap, sources)

