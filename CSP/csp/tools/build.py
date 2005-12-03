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
import time
import atexit

from distutils import sysconfig

import SCons
import SCons.Builder
from SCons.Builder import Builder
from SCons.Script import SConscript
from SCons.Script.SConscript import SConsEnvironment
from SCons.Action import Action
from SCons.Scanner import Scanner
from SCons.Node.FS import default_fs

File = default_fs.File
Dir = default_fs.Dir

SConsVersion = tuple(map(int, SCons.__version__.split('.')))


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
	return (a.find('.') > 0) and (b.find('.') > 0) and cmp(map(int, a.split('.')), map(int, b.split('.')))


def IsWindows(env):
	return env['PLATFORM'].startswith('win')


def SimpleCommand(cmd, msg):
	def s(target, src, env):
		print msg % {'src':str(src), 'target':str(target[0])}
	return SCons.Action.CommandAction(cmd, strfunction=s)


############################################################################
# BUILD LOG AND DISPLAY MESSAGES
#
# The code here modifies the way scons displays build operations.  As of
# version 0.96.91, SCons provides a relatively simple mechanism for
# customizing build messages via environment variables.  Unfortunately
# this feature has some bugs and significant limitations.  In particular,
# it isn't possible to customize the output for certian types of commands
# (such as $ARCOM, which is a known issue that will probably be fixed in
# the next release), and there is no easy way to simplify the display output
# while still logging the full build commands.
#
# Avoiding these limitations requires low-level tweaks to the scons Action
# classes.  This is extremely brittle and prone to breakage with each new
# SCons release, although hopefully soon SCons will provide good enough
# support for build logging and message customization that these hacks will
# no longer be needed.


def WriteToBuildLog(message):
	"""
	Write a message to the build log.  The log should contain a verbose
	record of the actual commands that were executed.
	"""
	if message and BUILD_LOG:
		print >>BUILD_LOG, message
		BUILD_LOG.flush()


def TargetMessage(phrase, suffix=''):
	"""Simplified build message function that displays the first target."""
	def printer(target, source, env, phrase=phrase, suffix=suffix):
		if type(target) == type([]):
			target = map(str, target)
			if len(target) == 1: target = target[0]
		print '- %s %s%s' % (phrase, target, suffix)
	return printer


def SourceMessage(phrase, suffix='', limit=None):
	"""Simplified build message function that displays the first source."""
	def printer(target, source, env, phrase=phrase, suffix=suffix):
		if type(source) == type([]):
			source = map(str, source)
			if limit is not None and len(source) > limit:
				source = source[:limit]
			if len(source) == 1: source = source[0]
		print '- %s %s%s' % (phrase, source, suffix)
	return printer


def SourceToTargetMessage(phrase, suffix=''):
	"""Simplified build message function that displays the first source and target."""
	def printer(target, source, env, phrase=phrase, suffix=suffix):
		source = map(str, source)
		target = map(str, target)
		print '- %s %s to %s %s' % (phrase, source[0], target[0], suffix)
	return printer


def ActionCallWrapper(self, target, source, env, *args, **kw):
	"""
	A replacement for SCons.Action._ActionAction.__call___.  This function wraps
	the original __call__ implementation to provide low-level hooks for logging
	and displaying build messsages.
	
	Note that this is an extremely brittle hack than should be removed If and
	when SCons provides better native support for these customizations.

	Rationale: As of SCons 0.96.91, the introduction of memoization and lazy
	action evaluation makes it impossible to directly modify many of the
	important Action instances (since they are generated on the fly during the
	build).  The only customization hook is to override strfunction, but the
	replacement function does not have access to the Action instance and so
	cannot generate the actual build command string.
	"""
	if not SCons.Util.is_List(target):
		target = [target]
	if not SCons.Util.is_List(source):
		source = [source]

	# generate the full build message by directly calling the original strfunction
	# implementation.
	log = self.__class__.strfunction(self, target, source, env)
	WriteToBuildLog(log)

	# if the strfunction has been replaced with our ShortMessageWrapper helper we
	# pass the action and verbose log message as additional arguments.  these are
	# currently needed in order to suppress duplicate display messages for some
	# multi-action build steps (such as $ARCOM).
	if isinstance(self.strfunction, ShortMessageWrapper):
		self.strfunction(target, source, env, action=self, log=log)
	else:
		if self.strfunction is not None:
			self.strfunction(target, source, env)

	# disable build message display in the original __call__ implementation.
	# ideally we would let the original method handle build logging via
	# env['PRINT_CMD_LINE_FUNC'] and this wrapper would just provide the
	# short message display, but with the new memoization infrastructure in
	# SCons the only way to get a custom build function into the action is
	# to override strfunction.  (we could swap strfunction here, but that would
	# cause problems for parallel builds.)
	if len(args) > 2:
		args = list(args)
		args[2] = 0
	else:
		kw['show'] = 0

	# now call the original __call__ implementation.
	return self._internal_call(target, source, env, *args, **kw)


class ShortMessageWrapper:
	def __init__(self, message):
		self.message = message

	def bind(self, env, action):
		if SConsVersion >= (0, 96, 91):
			self._bindNew(env, action)
		else:
			self._bindOld(env, action)

	def _bindNew(self, env, action):
		if isinstance(action, SCons.Action.LazyAction):
			action.gen_kw['strfunction'] = self
			#self._bind(env, action)
			#self.action = action
			return
		if isinstance(action, SCons.Action.CommandGeneratorAction) and hasattr(action, 'generator'):
			if isinstance(action.generator, SCons.Builder.DictCmdGenerator):
				for ext, subaction in action.generator.items():
					if subaction:
						self._bind(env, subaction)
					else:
						print 'warning: unable to attach short message wrapper to action (%s) for extension "%s"' % (self.message, ext)
				return
		if isinstance(action, SCons.Action.ListAction):
			for act in action.list:
				self._bind(env, act)
			return
		self._bind(env, action)

	def _bindOld(self, env, action):
		if isinstance(action, SCons.Action.ListAction):
			for act in action.list:
				self._bind(env, act)
			return
		self._bind(env, action)

	def _bind(self, env, action):
		strfunction = getattr(action, 'strfunction', None)
		if not isinstance(strfunction, ShortMessageWrapper):
			action.strfunction = self

	def __call__(self, target, source, env, action=None, log=''):
		# nasty hack.  staticlib and sharedlib are multistep actions, but we only
		# want to display one message.  here we suppress output for the less interesting
		# step(s).
		if not log.startswith('ranlib') and not log.startswith('SharedFlagChecker'):
			return self.message(target, source, env)


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
		ShortMessageWrapper(message).bind(env, action)


def InstallMessageHook():
	try:
		action_class = SCons.Action._ActionAction
	except:
		action_class = SCons.Action.ActionBase  # version 0.96.1 and earlier
	action_class._internal_call = action_class.__call__
	action_class.__call__ = ActionCallWrapper


def SetShortMessages(env):
	InstallMessageHook()
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
		output = swig_err.readlines() + swig_out.readlines()
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
		ok = (CompareVersions(version, min_version) >= 0)
	if ok:
		context.Result("yes (%s)" % version)
		context.env['%s_VERSION' % lib_name.upper()] = version
	else:
		context.Result("no")
	return ok


def CustomConfigure(env):
	conf = env.Configure(log_file="#/.config.log")
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


def RemoveFlags(env, **kw):
	"""
	Remove flags from environment variables.  The specified environment
	variables must be lists for this function to work.
	"""
	for key, val in kw.items():
		if isinstance(val, str):
			val = [val]
		if env.has_key(key):
			flags = env[key]
			if isinstance(flags, list):
				for flag in val:
					try:
						flags.remove(flag)
					except ValueError:
						pass


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
	SetConfig(env, config)
	def nop(target, source, env): pass
	env.Append(BUILDERS = {'config': Builder(action=Action(nop, ''))})


def SetDefaultMessage(env, message):
	AddPhonyTarget(env, 'default', message)
	env.Default('default')


def GlobalSetup(env, distributed=1, short_messages=None, default_message=None, config=None, with_swig=1, timer=1):
	try:
		options = SCons.Script.Main.options
		ssoptions = SCons.Script.Main.ssoptions
	except:
		options = SCons.Script.options  # version 0.96.1
		ssoptions = SCons.Script.ssoptions

	if default_message is not None:
		SetDefaultMessage(env, default_message)
	if with_swig:
		AddSwigSupport(env)
	if short_messages is None:
		short_messages = options.no_progress
	AddBuilders(env)
	if short_messages:
		SetShortMessages(env)
	if distributed and ssoptions.get('num_jobs') > 1:
		SetDistributed(env)
	AddPhonyTarget(env, 'config')
	SConsEnvironment.SetConfig = SetConfig
	SConsEnvironment.Documentation = MakeDocumentation
	SConsEnvironment.RemoveFlags = RemoveFlags
	if config:
		env.SetConfig(config)
	if timer:
		start_time = time.time()
		def showtime(start_time=start_time):
			print 'build time: %d sec' % (time.time() - start_time)
		atexit.register(showtime)


class GlobalSettings:
	def __init__(self, config=None):
		self._platform = sys.platform
		self._config = config
		self._dict = {}
	def IsWindows(self):
		return self._platform.startswith('win')
	def IsLinux(self):
		return self._platform.startswith('linux')
	def UnsupportedPlatform(self):
		print 'Platform "%s" not supported' % self._platform
		sys.exit(1)
	def __setattr__(self, key, value):
		if key.startswith('_'):
			self.__dict__[key] = value
		else:
			self._dict[key] = value
	def env(self):
		env = SCons.Environment.Environment(**self._dict)
		GlobalSetup(env)
		if self._config is not None:
			env.SetConfig(self._config)
		return env


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
		cmd = env.subst('$SWIG $SWIGFLAGS $SWIGINCLUDES -MM %s' % str(node))
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
	CopyFile = Builder(action=Action(copy, strfunction=report))
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
		source.target_from_source('', env['CXXFILESUFFIX']),
		dir.File(name + '.h')
	]
	return (target, source)


def AddNet(env):
	_, cxx_file = SCons.Tool.createCFileBuilders(env)
	cxx_file.add_emitter('.net', EmitNet)
	cxx_file.add_action('.net', '$TRC')
	trc = env.File('#/tools/trc/trc.py')
	env['TRC'] = '%s --source=${TARGETS[0]} --header=${TARGETS[1]} $SOURCES' % trc


def AddBuilders(env):
	AddDoxygen(env)
	AddCopyFile(env)
	AddLinkFile(env)
	AddNet(env)


def MarkVersionSource(env, target, prefix='Version.'):
	if type(target) == type([]): target = target[0]
	if hasattr(target, 'children'): target = target.children()
	version = [child for child in target if str(child).startswith(prefix)]
	others = [child for child in target if not str(child).startswith(prefix)]
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

