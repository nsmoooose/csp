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

import os

from csp.tools.build import autoconf
from csp.tools.build import scons
from csp.tools.build import util


class _BuildRegistry:
	def __init__(self):
		self._targets = {}
		self._sources = {}
		self._libraries = {}
		self._tests = []

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
		if ('config' in scons.GetCommandlineTargets()) or not self._ReadConfigs(env):
			if not env.GetOption('clean'):
				valid = 1
				conf = autoconf.CustomConfigure(env.Copy())
				settings = util.Settings()
				for name, lib in self._libraries.items():
					valid = lib.config(conf, settings) and valid
				new = conf.Finish()
				if not valid:
					print 'Configure failed'
					sys.exit(1)
				self._SaveConfigs(env)

	def _ReadConfigs(self, env):
		saved = autoconf.ReadConfig(env)
		if not isinstance(saved, dict): return 0
		for name in self._libraries.keys():
			if not saved.has_key(name): return 0
		for name, settings in saved.items():
			if not self._libraries.has_key(name): return 0
			self._libraries[name]._settings = util.Settings(settings)
		return 1

	def _SaveConfigs(self, env):
		config = {}
		for name, lib in self._libraries.items():
			config[name] = dict(lib._settings)
		autoconf.SaveConfig(env, config)

	def _RunTests(self):
		if not self._tests: return
		os.environ.setdefault('CSPLOG_FILE', os.path.join(scons.File('#/.testlog').abspath))
		try:
			import csp.csplib
		except ImportError:
			print 'ERROR: unnable to import csp.csplib'
			return
		for test in self._tests:
			csp.csplib.TestRegistry.loadTestModule(test[0].abspath)
		csp.csplib.TestRegistry.runAll()

	def Build(self, env):
		self.Configure(env)
		for target in self._targets.values():
			object = target.build()
			if target.isTest():
				self._tests.append(object)
		def runtests(*args, **kw):
			self._RunTests()
		env.Command('runtests', 'tests', runtests)

BuildRegistry = _BuildRegistry()

