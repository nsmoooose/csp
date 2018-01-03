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


from csp.tools.build import autoconf
from csp.tools.build import builders
from csp.tools.build import registry
from csp.tools.build import scons
from csp.tools.build import util
from csp.tools.build import vcproj

import atexit
import logging
import os
import time

import SCons
from SCons.Script.SConscript import SConsEnvironment


class Environment:
	'''
	Environment defines the main entry point for initializing the build system
	and creating the top-level SCons environment.  Define a subclass that
	implements any of the following methods:
	
		customize_all(self, env):
			called on all platforms

		customize_default(self, env):
			called when no platform-specific customize methods match

		customize_{platform}(self, env):
			'platform' is any lowercase prefix of sys.platform (e.g.,
			'customize_win' or 'customize_linux'.

	The docstring of the subclass defines the default help message displayed
	when 'scons -h' is run.  To initialize the build system, call Initialize
	directly on the subclass (do not instantiate the subclass):

		env = MyEnvironment.Initialize()
	'''

	def Initialize(klass, opts=None):
		settings = klass()
		help = settings.__doc__

		env = SCons.Environment.Environment()
		GlobalSetup(env)

		if opts is None:
			opts = SCons.Variables.Variables()

		if hasattr(settings, 'options'):
			settings.options(opts)

		env.Help(help + opts.GenerateHelpText(env))
		opts.Update(env)

		if settings is not None:
			CustomizeForPlatform(env, settings)

		AddVCProjectTarget(env)
		return env
	Initialize = classmethod(Initialize)



def ReadPackages(env, packages, **kw):
	'''
	Helper for loading sconscript build definitions in subdirectories.
	This method is added to the scons environment.

	Similar to the build-in SConstruct() function, but by default sets
	the build dir to '.bin' in each subdirectory, disables source duplication,
	and exports both 'env' and 'build'.  Returns a single list of all
	variables returned by all child build scripts.
	
	Keyword arguments are passed to SConscript() and override the default
	settings.
	'''
	from csp.tools import build
	kw.setdefault('duplicate', 0)
	kw.setdefault('exports', 'env build')
	has_variant_dir = 'variant_dir' in kw
	result = []
	for package in packages:
		if not has_variant_dir:
			kw['variant_dir'] = '%s/.bin' % package
		targets = env.SConscript('%s/SConscript' % package, **kw)
		if targets:
			if type(targets) != type([]): targets = [targets]
			result.extend(targets)
	return scons.Flatten(result)


def CustomizeForPlatform(env, settings):
	'''
	Helper for setting platform-specific build options.  This method is added to
	the scons environment.
	
	To use, define a class in the main SConstruct file with methods for each
	supported platform.  The methods must be named 'customize_{platform}',
	where platform is a lowercase prefix of sys.platform (e.g., 'linux',
	'win', 'darwin').  The customize methods take self and the SCons environment
	as arguments.

	For example:

		class PlatformSettings:
			def customize_default(self, env):
				print 'warning: unsupported platform'
			def customize_win(self, env):
				env['CXXFLAGS'] = Split('/GR /MD /O2 /EHsc /W3 /nologo')
			def customize_linux(self, env):
				env['CXXFLAGS'] = Split('-O2 -g -W -Wall -pedantic -Wno-long-long')
				env['ARFLAGS'] = Split('cr')

		env.CustomizeForPlatform(PlatformSettings)
	
	In addition, 'config_{platform}' methods can be defined to handle platform-
	specific build configuration (e.g., checking for libraries and headers).
	Note however that build.ExternalLibrary provides a better mechanism for
	library configuration.
	
	TODO: are the platform config methods still needed and are they compatible
	with ExternalLibrary?
	'''
	platform = sys.platform.lower()

	configs = {}
	customizations = {}

	# build maps of all config_ and customize_ methods, indexed by platform.
	for item in dir(settings):
		if item.startswith('config_'):
			configs[item[len('config_'):]] = getattr(settings, item)
		elif item.startswith('customize_'):
			customizations[item[len('customize_'):]] = getattr(settings, item)

	if hasattr(settings, 'customize_all'):
		settings.customize_all(env)

	run_config = getattr(settings, 'config_default', None)
	run_customization = getattr(settings, 'customize_default', None)

	for index in range(len(platform), 0, -1):
		prefix = platform[:index]
		match_config = prefix in configs
		match_customization = prefix in customizations
		if match_config: run_config = configs[prefix]
		if match_customization: run_customization = customizations[prefix]
		if match_config or match_customization:
			break

	if run_config is not None:
		autoconf.SetConfig(env, run_config)

	if run_customization is not None:
		run_customization(env)


def FinalizePackages(env):
	registry.BuildRegistry.Build(env)


def GlobalSetup(env, distributed=1, config=None, timer=1):
	options = scons.GetOptions()
	# TODO remove ssoptions altogether; options.num_jobs should work in 0.97 and newer
	# versions of scons.
	try:
		ssoptions = scons.GetSettableOptions()
		num_jobs = ssoptions.get('num_jobs')
	except AttributeError:
		num_jobs = options.num_jobs

	import SCons.Tool.swig
	if not SCons.Tool.swig.exists(env):
		SCons.Tool.swig.generate(env)

	builders.AddBuilders(env)
	if distributed and num_jobs > 1:
		scons.SetDistributed(env)
	util.AddPhonyTarget(env, 'config')
	SConsEnvironment.CopyEnvironment = util.CopyEnvironment
	SConsEnvironment.SetConfig = autoconf.SetConfig
	SConsEnvironment.Documentation = MakeDocumentation
	SConsEnvironment.RemoveFlags = util.RemoveFlags
	SConsEnvironment.ReadPackages = ReadPackages
	SConsEnvironment.FinalizePackages = FinalizePackages
	if config:
		env.SetConfig(config)
	if timer:
		start_time = time.time()
		def showtime(start_time=start_time):
			elapsed = time.time() - start_time
			if elapsed > 10:
				print 'build time: %d sec' % elapsed
		atexit.register(showtime)


def MakeDocumentation(env, target, config, sources):
	target = scons.Dir(target)
	html = target.Dir('html')
	if 'dox' in scons.GetCommandlineTargets():
		index = html.File('index.html')
		dox = env.Doxygen(index, scons.File(config))
		for item in sources:
			if isinstance(item, str):
				if item.startswith('@'):
					item = registry.BuildRegistry.GetSourceGroup(item)
					env.Depends(dox, item.all)
				else:
					env.Depends(dox, util.Glob(item))
			else:
				print 'implement me'
		env.Alias('dox', dox)
	env.Clean(['dox', 'all'], html)


def AddVCProjectTarget(env):
	def generate(*args, **kw): vcproj.Generate()
	env.Command('vcproj', [], generate)

