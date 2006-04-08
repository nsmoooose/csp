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
import SCons.Defaults
import SCons.Builder
import SCons.Util
from SCons.Builder import Builder
from SCons.Script import SConscript
from SCons.Script.SConscript import SConsEnvironment
from SCons.Action import Action
from SCons.Scanner import Scanner
from SCons.Node.FS import default_fs

File = default_fs.File
Dir = default_fs.Dir
Alias = SCons.Defaults.DefaultEnvironment().Alias
AlwaysBuild = SCons.Defaults.DefaultEnvironment().AlwaysBuild
Depends = SCons.Defaults.DefaultEnvironment().Depends
Flatten = SCons.Util.flatten

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
	if (a.find('.') < 0) or (b.find('.') < 0): return -1
	return cmp(map(int, a.split('.')), map(int, b.split('.')))


def IsWindows(env):
	return env['PLATFORM'].startswith('win')


def SimpleCommand(cmd, msg):
	def s(target, src, env):
		print msg % {'src':str(src), 'target':str(target[0])}
	return SCons.Action.CommandAction(cmd, strfunction=s)

def Extension(fn):
	return os.path.splitext(fn)[1]

def GetCurrentScript():
	try:
		call_stack = SCons.Script.call_stack
	except AttributeError:
		call_stack = SCons.Script.SConscript.stack  # pre 0.96.91
	return call_stack[-1].sconscript

class SourceList(object):
	_header_ext = ('.h', '.hh', '.hpp', '.hxx')
	_source_ext = ('.c', '.cc', '.cpp', '.cxx')
	_swig_ext = ('.i',)
	_message_ext = ('.net',)
	_known_ext = _header_ext + _source_ext + _swig_ext + _message_ext

	def __init__(self, *items):
		self._items = items
		self._tests = []

	def addTests(self, *tests):
		self._tests += tests

	def getAll(self):
		return self._items
	def getHeaders(self):
		return [x for x in self._items if Extension(x) in SourceList._header_ext]
	def getSources(self):
		return [x for x in self._items if Extension(x) in SourceList._source_ext]
	def getMessages(self):
		return [x for x in self._items if Extension(x) in SourceList._message_ext]
	def getSwigInterfaces(self):
		return [x for x in self._items if Extension(x) in SourceList._swig_ext]
	def getMiscellaneousFiles(self):
		header_or_source = SourceList._header_ext + SourceList._source_ext
		return [x for x in self._items if Extension(x) not in header_or_source]
	def getOtherFiles(self):
		return [x for x in self._items if Extension(x) not in SourceList._known_ext]
	def getTests(self):
		return self._tests

	all = property(getAll)
	headers = property(getHeaders)
	sources = property(getSources)
	messages = property(getMessages)
	misc = property(getMiscellaneousFiles)
	swig = property(getSwigInterfaces)
	other = property(getOtherFiles)
	tests = property(getTests)

	def objects(self, env):
		out = []
		for item in self._items:
			if item.endswith('.i'):
				out.append(env.SwigWrapper(item))
			elif not item.endswith('.h'):
				out.append(item)
		return out


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
	result = Flatten(result)
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
	result = Flatten(result)
	return result


"""
class Module:
	def __init__(env, name, sources=[], headers=[], tests=[], messages=[], submodules=[]):
		self._name = name
		self._sources = map(env.File, sources)
		self._headers = map(env.File, headers)
		self._tests = map(env.File, tests)
		self._messages = map(env.File, messages)
		self._modules = modules
		env['_BUILD_MODULES_'][name] = self

	def __getattr__(self, name):
		if name.startswith('_'): return self.__dict__[name]
		if name == 'name': return self._name
		values = self.__dict__.get('_' + name, [])
		for module in self._modules:
			values += getattr(module, name)


def BuildModules_(env, modules, **kw):
	import build
	env.setdefault('_BUILD_MODULES_', {})
	result = []
	for module in modules:
		kw.setdefault('exports', 'env build')
		module = env.SConscript('%s/SConscript' % module, **kw)
		if module:
			if type(targets) != type([]): targets = [targets]
			result.extend(targets)
	result = Flatten(result)
	return result
"""


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


def CheckPkgConfig(context, lib, version=None, lib_name=None, command='pkg-config', version_flag='--modversion', config_flags='--cflags --libs'):
	if lib_name is None: lib_name = lib
	env = context.env
	_checking(context, lib, version)
	output = os.popen('%s %s %s' % (command, version_flag, lib))
	ok = 0
	if output is not None:
		lib_version = output.readline().strip()
		ok = (version is None) or (CompareVersions(lib_version, version) >= 0)
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
		ok = (CompareVersions(version, min_version) >= 0)
	if ok:
		context.Result("yes (%s)" % version)
		context.env['%s_VERSION' % lib_name.upper()] = version
	else:
		context.Result("no")
	return ok


def CustomConfigure(env):
	SCons.Script.SConscript.sconscript_reading = 1
	conf = env.Configure(log_file="#/.config.log")
	conf.AddTests({'CheckSwig': CheckSwig})
	conf.AddTests({'CheckLibVersion': CheckLibVersion})
	conf.AddTests({'CheckOSGVersion': CheckOSGVersion})
	conf.AddTests({'CheckCommandVersion': CheckCommandVersion})
	conf.AddTests({'CheckPkgConfig': CheckPkgConfig})
	# override -Q
	if hasattr(SCons.SConf, 'SetProgressDisplay'):
		Scons.SConf.SetProgressDisplay(SCons.Util.display)
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

def _CustomizeForPlatform(env, platform_settings):
	assert isinstance(platform_settings, PlatformSettings)
	SetConfig(env, platform_settings.config)
	platform_settings.customize(env)

def CopyEnvironment(env, vars):
	for var in vars:
		if os.environ.has_key(var):
			env['ENV'][var] = os.environ[var]

def CustomizeForPlatform(env, settings):
	if hasattr(settings, '__bases__'):
		settings = settings()
	#script = GetCurrentScript()
	platform = sys.platform.lower()

	configs = []
	customizations = []
	for item in dir(settings):
		if item.startswith('config_'):
			configs.append((item[len('config_'):], getattr(settings, item)))
		elif item.startswith('customize_'):
			customizations.append((item[len('customize_'):], getattr(settings, item)))

	run_config = getattr(settings, 'config_default', None)
	run_customization = getattr(settings, 'customize_default', None)
	if not platform.startswith('win'):
		if run_config is None:
			run_config = getattr(settings, 'config_linux', None)
		if run_customization is None:
			run_customization = getattr(settings, 'customize_linux', None)
	for target, method in configs:
		if platform.startswith(target):
			run_config = method
			break
	for target, method in customizations:
		if platform.startswith(target):
			run_customization = method
			break
	if run_config is not None:
		SetConfig(env, run_config)
	if run_customization is not None:
		run_customization(env)

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
	CXX = '$( %s $) %s' % (distcc, CXX)
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
	SConsEnvironment.CustomizeForPlatform = CustomizeForPlatform
	SConsEnvironment.CopyEnvironment = CopyEnvironment
	SConsEnvironment.SetConfig = SetConfig
	SConsEnvironment.Documentation = MakeDocumentation
	SConsEnvironment.RemoveFlags = RemoveFlags
	if config:
		env.SetConfig(config)
	if timer:
		start_time = time.time()
		def showtime(start_time=start_time):
			elapsed = time.time() - start_time
			if elapsed > 10:
				print 'build time: %d sec' % elapsed
		atexit.register(showtime)


class GlobalSettings:
	def __init__(self, config=None, help=''):
		self._platform = sys.platform
		self._config = config
		self._default_message = 'No targets specified; try scons -h for help.'
		self._help = help
		self._opts = SCons.Options.Options()
		self._dict = {}
	def IsWindows(self):
		return self._platform.startswith('win')
	def IsLinux(self):
		return self._platform.startswith('linux')
	def UnsupportedPlatform(self):
		print 'Platform "%s" not supported' % self._platform
		sys.exit(1)
	def AddOption(self, *arg, **kw):
		self._opts.Add(*arg, **kw)
	def __setattr__(self, key, value):
		if key.startswith('_'):
			self.__dict__[key] = value
		else:
			self._dict[key] = value
	def env(self):
		env = SCons.Environment.Environment(**self._dict)
		GlobalSetup(env, default_message=self._default_message, config=self._config)
		env.Help(self._help + self._opts.GenerateHelpText(env))
		self._opts.Update(env)
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
	env['SWIGCOM'] = '$SWIG $SWIGFLAGS $_SWIGINCFLAGS -o $TARGET $SOURCE'
	env['SWIGINCPREFIX'] = '-I'
	env['SWIGINCSUFFIX'] = '$INCSUFFIX'
	env['SWIGCXXFLAGS'] = '$CXXFLAGS'
	env['_SWIGINCFLAGS'] = '$( ${_concat(SWIGINCPREFIX, SWIGINCLUDES, SWIGINCSUFFIX, __env__, RDirs)} $)'
	builder = Builder(action = '$SWIG $SWIGFLAGS $_SWIGINCFLAGS -o ${TARGETS[0]} $SOURCE', emitter = EmitSwig)
	env.Append(BUILDERS = {'Swig': builder})
	def wrapper_builder(env, target = None, source = SCons.Builder._null, **kw):
		kw.setdefault('CXXFLAGS', env['SWIGCXXFLAGS'])
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

def AddSwigSupport(env, required=1):
	global SWIG
	SWIG = SCons.Util.WhereIs('swig')
	if not SWIG:
		print 'WARNING: swig not found in path'
		if required:
			print >>sys.stderr, 'Cannot continue without swig.'
			sys.exit(1)
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


def AddVisualStudioProject(env):
	def VisualStudioProject(env, target, source):
		if IsWindows(env) and hasattr(env, 'MSVSProject'):
			if isinstance(target, list): target = target[0]
			project_base = os.path.splitext(os.path.basename(target.name))[0]
			if project_base.startswith('_'): project_base = project_base[1:]
			dbg = env.MSVSProject(target=project_base+'-dbg.vcproj', srcs=source.sources, incs=source.headers, misc=source.misc, buildtarget=target, variant='Debug')
			rel = env.MSVSProject(target=project_base+'-rel.vcproj', srcs=source.sources, incs=source.headers, misc=source.misc, buildtarget=target, variant='Release')
			return [dbg, rel]
		else:
			return []
	env.Append(BUILDERS = {'VisualStudioProject': VisualStudioProject})


def AddBuilders(env):
	#env['MSVS_VERSION'] = '7.1'
	#SCons.Tool.msvs.generate(env)
	AddDoxygen(env)
	AddCopyFile(env)
	AddLinkFile(env)
	AddNet(env)
	AddVisualStudioProject(env)


def MarkVersionSource(env, target, prefix='Version.'):
	if type(target) == type([]): target = target[0]
	if hasattr(target, 'children'): target = target.children()
	version = [child for child in target if str(child).startswith(prefix)]
	others = [child for child in target if not str(child).startswith(prefix)]
	if version: env.Depends(version, others)


def MakeDocumentation(env, target, config, sources):
	target = Dir(target)
	html = target.Dir('html')
	if 'dox' in SConscript.CommandLineTargets:
		index = html.File('index.html')
		dox = env.Doxygen(index, File(config))
		for item in sources:
			if isinstance(item, str):
				if item.startswith('@'):
					item = BuildRegistry.GetSourceGroup(item)
					env.Depends(dox, item.all)
				else:
					env.Depends(dox, Glob(item))
			else:
				print 'implement me'
		env.Alias('dox', dox)
	env.Clean(['dox', 'all'], html)

def Apply(builder, sources, **overrides):
	def builder_wrap(source):
		return builder(source, **overrides)
	return map(builder_wrap, sources)


# =============================================================================================

class Settings(dict):
	def merge(self, *args, **kw):
		settings = kw
		if args:
			assert len(args) == 1
			settings.update(args[0])
		for key, value in settings.items():
			base = self.setdefault(key, [])
			for item in value:
				if not item in base: base.append(item)

	def apply(self, env):
		env.AppendUnique(**self)

	def Diff(old_env, new_env):
		settings = {}
		for key in ('CCFLAGS', 'CPPPATH', 'CPPFLAGS', 'LIBS', 'LIBPATH', 'LINKFLAGS'):
			new_values = new_env.get(key, [])
			old_values = old_env.get(key, [])
			for old in old_values:
				if old in new_values: new_values.remove(old)
			settings[key] = new_values
		return Settings(settings)
	Diff = staticmethod(Diff)


class _BuildRegistry:
	def __init__(self):
		self._targets = {}
		self._sources = {}
		self._libraries = {}
	def AddLibrary(self, name, lib):
		self._libraries[name] = lib
	def AddSourceGroup(self, name, group):
		self._sources['@' + name] = group
	def AddTarget(self, name, target):
		self._targets[name] = target
	def GetLibrary(self, name, req=0):
		#if not name.startswith('@'):
		#	raise 'invalid library %s' % name
		if req and not self._libraries.has_key(name):
			raise 'unknown library %s' % name
		return self._libraries.get(name)
	def GetTarget(self, name, req=0):
		#if not name.startswith('@'):
		#	raise 'invalid target %s' % name
		if req and not self._targets.has_key(name):
			raise 'unknown target %s' % name
		return self._targets.get(name)
	def GetSourceGroup(self, name, req=0):
		if not name.startswith('@'):
			raise 'invalid sourcegroup %s' % name
		if req and not self._sources.has_key(name):
			raise 'unknown source group %s' % name
		return self._sources.get(name)
	def GetDependency(self, name):
		return self._libraries.get(name) or self._sources.get(name) or self._targets.get(name)

	def Configure(self, env):
		if ('config' in SConscript.CommandLineTargets) or not self._ReadConfigs(env):
			if not env.GetOption('clean'):
				valid = 1
				conf = CustomConfigure(env.Copy())
				settings = Settings()
				for name, lib in self._libraries.items():
					valid = lib.config(conf, settings) and valid
				new = conf.Finish()
				if not valid:
					print 'Configure failed'
					sys.exit(1)
				self._SaveConfigs(env)

	def _ReadConfigs(self, env):
		config_file = env.GetBuildPath('.config')
		try:
			config = open(config_file, 'rt')
		except IOError:
			return 0
		saved = pickle.load(config)
		if not isinstance(saved, dict): return 0
		for name in self._libraries.keys():
			if not saved.has_key(name): return 0
		for name, settings in saved.items():
			if not self._libraries.has_key(name): return 0
			self._libraries[name]._settings = Settings(settings)
		return 1

	def _SaveConfigs(self, env):
		config_file = env.GetBuildPath('.config')
		config = open(config_file, 'wt')
		out = {}
		for name, lib in self._libraries.items():
			out[name] = dict(lib._settings)
		pickle.dump(out, config)

	def Build(self):
		for target in self._targets.values():
			target.build()

BuildRegistry = _BuildRegistry()


class ExternalLibrary:
	def __init__(self, name='', config=[]):
		assert name
		self._name = name
		self._config = config
		self._settings = Settings()
		BuildRegistry.AddLibrary(name, self)

	def _save(env, oldenv):
		settings = {}
		for key in ('CCFLAGS', 'CPPPATH', 'CPPFLAGS', 'LIBS', 'LIBPATH', 'LINKFLAGS'):
			new_values = env.get(key, [])
			old_values = oldenv.get(key, [])
			for old in old_values:
				if old in new_values: new_values.remove(old)
			settings[key] = new_values
		return settings
	_save = staticmethod(_save)

	def config(self, conf, global_settings):
		valid = 1
		oldenv = conf.env.Copy()
		settings = Settings()
		for c in self._config:
			valid = c.configure(conf) and valid
		if valid:
			settings.merge(ExternalLibrary._save(conf.env, oldenv))
			global_settings.merge(settings)
			self._settings = settings
		conf.env = oldenv
		return valid

	def _addSettings(self, settings, bdeps):
		settings.merge(self._settings)

class PkgConfig:
	def __init__(self, package=None, version=None, label=None):
		assert package
		if label is None: label = package
		self._package = package
		self._version = version
		self._label = label

	def configure(self, conf):
		if IsWindows(conf.env): return 1
		return conf.CheckPkgConfig(self._package, version=self._version, lib_name=self._label)

class CommandConfig:
	def __init__(self, package=None, version_command=None, flags_command=None, version=None, label=None):
		self._package = package
		self._version_command = version_command
		self._flags_command = flags_command
		self._version = version
		self._label = label

	def configure(self, conf):
		if IsWindows(conf.env): return 1
		valid = conf.CheckCommandVersion(self._package, self._version_command, self._version, lib_name=self._label)
		if valid:
			conf.env.ParseConfig(self._flags_command)
		return valid

class LibConfig:
	def __init__(self, lib=None, symbol='', label=None):
		if label is None: label = lib
		self._lib = lib
		self._symbol = symbol
		self._label = label
	def configure(self, conf):
		return conf.CheckLib(self._lib, self._symbol)

class UnixLibConfig(LibConfig):
	def __init__(self, lib=None, symbol='', label=None):
		LibConfig.__init__(self, lib, symbol, label)
	def configure(self, conf):
		if IsWindows(conf.env): return 1
		return conf.CheckLib(self._lib, self._symbol)

class WindowsLibConfig(LibConfig):
	# using atoi as the default test symbol since it always succeeds.
	# haven't yet figured out how to test real symbols in many libraries,
	# probably due to complications with declspec.
	def __init__(self, lib=None, symbol='atoi', label=None):
		LibConfig.__init__(self, lib, symbol, label)
	def configure(self, conf):
		if not IsWindows(conf.env): return 1
		return conf.CheckLib(self._lib, self._symbol)


class DevpackConfig:
	DEVPACK = None
	VERSION = None

	def _checkPath(*args):
		path = os.path.join(*args)
		if not os.path.exists(path):
			print 'CSPDEVPACK path (%s) not found.' % path
			sys.exit(1)
	_checkPath = staticmethod(_checkPath)

	def SetMinimumVersion(version):
		DevpackConfig.VERSION = version
	SetMinimumVersion = staticmethod(SetMinimumVersion)

	def _Find():
		if DevpackConfig.DEVPACK: return
		path = os.environ.get('CSPDEVPACK', '')
		if not path:
			print 'CSPDEVPACK environment variable not set.'
			sys.exit(1)
		try:
			v = map(int, re.search(r'[0-9.]+$', path).group().split('.'))
		except Exception:
			print 'CSPDEVPACK environment variable (%s) is does not look like a valid devpack path.' % path
			sys.exit(1)
		if v < map(int, DevpackConfig.VERSION.split('.')):
			print 'The installed devpack (%s) is too old; need version %s' % (path, version)
			sys.exit(1)
		DevpackConfig._checkPath(path)
		DevpackConfig._checkPath(path, 'usr', 'bin')
		DevpackConfig._checkPath(path, 'usr', 'lib')
		DevpackConfig._checkPath(path, 'usr', 'include')
		DevpackConfig.DEVPACK = path
	_Find = staticmethod(_Find)

	def __init__(self, dlls=[], libs=[], headers=[]):
		if isinstance(dlls, str):
			dlls = [dlls]
		if isinstance(libs, str):
			libs = [libs]
		self._dlls = dlls
		self._libs = libs
		self._headers = headers

	def configure(self, conf):
		if IsWindows(conf.env):
			DevpackConfig._Find()
			dp = DevpackConfig.DEVPACK
			for dll in self._dlls:
				self._checkPath(dp, 'usr', 'bin', dll + '.dll')
				self._checkPath(dp, 'usr', 'lib', dll + '.lib')
			for lib in self._libs:
				self._checkPath(dp, 'usr', 'lib', lib + '.lib')
			for header in self._headers:
				if isinstance(header, tuple):
					header = os.path.join(*header)
				self._checkPath(os.path.join(dp, 'usr', 'include', header))
			conf.env.AppendUnique(CPPPATH=[os.path.join(dp, 'usr', 'include')])
			conf.env.AppendUnique(LIBPATH=[os.path.join(dp, 'usr', 'lib')])
			conf.env.AppendUnique(LIBS=self._dlls + self._libs)
		return 1


class SourceGroup:
	def __init__(self, env, name='', sources=[], deps=[], **kw):
		self._env = env.Copy()
		self._name = name
		self._sources = [x for x in env.arg2nodes(sources)]
		self._deps = deps
		self._objects = None
		self._options = kw
		self._bdeps = []
		BuildRegistry.AddSourceGroup(name, self)

	def _makeObjects(self):
		if self._objects is None:
			settings = Settings()
			for dep in self._deps:
				lib = BuildRegistry.GetDependency(dep)
				if lib:
					lib._addSettings(settings, self._bdeps)
				else:
					print '%s: could not find dependency %s' % (self._name, dep)
					sys.exit(1)
			self._settings = settings
			settings.apply(self._env)
			self._objects = []
			for file in self._sources:
				if isinstance(file, SCons.Node.FS.File):
					if file.get_suffix() == '.i':
						wrapper, module = self._env.Swig(file, **self._options)
						self._objects += self._env.SwigWrapper(wrapper, **self._options)
					elif file.get_suffix() != '.h':
						self._objects.append(self._env.SharedObject(file, **self._options))
				else:
					self._objects.append(file)

	def _addSettings(self, settings, bdeps):
		self._makeObjects()
		settings.merge(self._settings)
		bdeps.extend(self._bdeps)

	def add(self, objects, settings, bdeps):
		self._makeObjects()
		objects.extend(self._objects)
		self._addSettings(settings, bdeps)

	_header_ext = ('.h', '.hh', '.hpp', '.hxx')
	_source_ext = ('.c', '.cc', '.cpp', '.cxx')
	_swig_ext = ('.i',)
	_message_ext = ('.net',)
	_known_ext = _header_ext + _source_ext + _swig_ext + _message_ext

	def getAll(self):
		return [x.srcnode() for x in self._sources]
	def getHeaders(self):
		return [x.srcnode() for x in self._sources if x.get_suffix() in SourceGroup._header_ext]
	def getSources(self):
		return [x.srcnode() for x in self._sources if x.get_suffix() in SourceGroup._source_ext]
	def getMessages(self):
		return [x.srcnode() for x in self._sources if x.get_suffix() in SourceGroup._message_ext]
	def getSwigInterfaces(self):
		return [x.srcnode() for x in self._sources if x.get_suffix() in SourceGroup._swig_ext]
	def getMiscellaneousFiles(self):
		header_or_source = SourceGroup._header_ext + SourceGroup._source_ext
		return [x.srcnode() for x in self._sources if x.get_suffix() not in header_or_source]
	def getOtherFiles(self):
		return [x.srcnode() for x in self._sources if x.get_suffix() not in SourceGroup._known_ext]

	all = property(getAll)
	headers = property(getHeaders)
	sources = property(getSources)
	messages = property(getMessages)
	misc = property(getMiscellaneousFiles)
	swig = property(getSwigInterfaces)
	other = property(getOtherFiles)


class SharedLibrary:
	def __init__(self, env, name, sources=[], aliases=[], deps=[], always_build=0, softlink=0, doxygen=None, **kw):
		self._env = env.Copy()
		self._name = name
		self._sources = [s for s in sources if s.startswith('@')]
		pure = [s for s in sources if not s.startswith('@')]
		if pure or deps:
			SourceGroup(env, name, sources=pure, deps=deps)
			self._sources.append('@' + name)
		self._target = os.path.join(env.Dir('.').path, name)  # a bit ugly
		self._aliases = aliases
		self._always_build = always_build
		self._softlink = softlink
		self._doxygen = None
		if doxygen:
			self._dox = env.Dir('.dox').srcnode()
			self._doxygen = env.File(doxygen)
		self._options = kw
		self._groups = None
		self._output = None
		BuildRegistry.AddTarget(name, self)

	def _findSources(self):
		if self._groups is None:
			self._groups = []
			for source in self._sources:
				group = BuildRegistry.GetSourceGroup(source)
				if not group:
					print 'shlib %s: unknown source %s' % (self._name, source)
					sys.exit(1)
				self._groups.append(group)

	def build(self):
		if self._output:
			return self._output
		objects = []
		settings = Settings()
		self._findSources()
		bdeps = []
		for group in self._groups:
			group.add(objects, settings, bdeps)
		bdeps = Flatten(bdeps)
		objects = Flatten(objects)
		settings.merge(self._options)
		shlib = self._env.SharedLibrary(self._target, objects, **settings)
		Alias = SCons.Defaults.DefaultEnvironment().Alias
		Alias(self._name, shlib)
		if self._aliases:
			Alias(self._aliases, shlib)
		if IsWindows(self._env):
			self._makeVisualStudioProject(shlib)
		if self._doxygen:
			self._env.Documentation(self._dox, self._doxygen, self._sources)
		if self._always_build:
			AlwaysBuild(shlib)
		self._output = shlib
		if bdeps and not self._env.GetOption('clean'):
			Depends(shlib, bdeps)
		return shlib[0]

	def _makeVisualStudioProject(self, shlib):
		if not hasattr(self._env, 'MSVSProject'):
			Alias('vs', [])
			return
		target = shlib[0]
		sources = []
		headers = []
		misc = []
		self._findSources()
		for group in self._groups:
			sources += group.sources
			headers += group.headers
			misc += group.misc
		dbg = self._env.MSVSProject(target=self._name+'-dbg.vcproj', srcs=sources, incs=headers, misc=misc, buildtarget=target, variant='Debug')
		rel = self._env.MSVSProject(target=self._name+'-rel.vcproj', srcs=sources, incs=headers, misc=misc, buildtarget=target, variant='Release')
		Alias('vs', [dbg, rel])

	def _addSettings(self, settings, bdeps):
		bdeps.append(self.build())
		self._findSources()
		for group in self._groups:
			group._addSettings(settings, bdeps)
		if IsWindows(self._env) or not self._softlink:
			settings.merge(LIBPATH=[os.path.dirname(self._target)], LIBS=[os.path.basename(self._target)])

