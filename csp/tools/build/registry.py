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
import sys

from csp.tools.build import autoconf
from csp.tools.build import scons
from csp.tools.build import util


class _BuildRegistry:
    def __init__(self):
        self._targets = {}
        self._sources = {}
        self._libraries = {}
        self._tests = []
        self._dir_tests = {}

    def AddLibrary(self, name, lib):
        self._libraries[name] = lib

    def AddSourceGroup(self, name, group):
        self._sources['@' + name] = group

    def AddTarget(self, name, target):
        self._targets[name] = target

    def GetLibrary(self, name, req=0):
        # if not name.startswith('@'):
        #    raise 'invalid library %s' % name
        if req and name not in self._libraries:
            raise 'unknown library %s' % name
        return self._libraries.get(name)

    def GetTarget(self, name, req=0):
        # if not name.startswith('@'):
        #    raise 'invalid target %s' % name
        if req and name not in self._targets:
            raise 'unknown target %s' % name
        return self._targets.get(name)

    def GetSourceGroup(self, name, req=0):
        if not name.startswith('@'):
            raise 'invalid sourcegroup %s' % name
        if req and name not in self._sources:
            raise 'unknown source group %s' % name
        return self._sources.get(name)

    def GetDependency(self, name):
        return self._libraries.get(name) or self._sources.get(name) or self._targets.get(name)

    def Configure(self, env):
        if ('config' in scons.GetCommandlineTargets()) or not self._ReadConfigs(env):
            if not env.GetOption('clean'):
                valid = 1
                conf = autoconf.CustomConfigure(env.Clone())
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
        if not isinstance(saved, dict):
            return 0
        for name in self._libraries.keys():
            if name not in saved:
                return 0
        for name, settings in saved.items():
            if name not in self._libraries:
                return 0
            self._libraries[name]._settings = util.Settings(settings)
        return 1

    def _SaveConfigs(self, env):
        config = {}
        for name, lib in self._libraries.items():
            config[name] = dict(lib._settings)
        autoconf.SaveConfig(env, config)

    def _SetupTests(self):
        os.environ.setdefault('CSPLOG_FILE', os.path.join(scons.File('#/.testlog').abspath))
        try:
            import csp.csplib
        except ImportError:
            print 'ERROR: unnable to import csp.csplib'
            return
        return csp.csplib.TestRegistry

    def _RunTests(self, *args, **kw):
        tests = kw['source']
        if not tests:
            return
        tester = self._SetupTests()
        for test in tests:
            tester.loadTestModule(test.abspath)
        tester.runAll()

    def _RunOneTest(self, *args, **kw):
        tester = self._SetupTests()
        modules = kw['source']
        for module in modules:
            tester.loadTestModule(module.abspath)
        tester.runAll()

    def Build(self, env):
        self.Configure(env)
        for target in self._targets.values():
            object = target.build()
            if target.isTest():
                self._tests.append(object)
                if target._path not in self._dir_tests:
                    self._dir_tests[target._path] = []
                self._dir_tests[target._path].append(object)
                run_alias = os.path.join(target._path, target._name) + '.run'
                env.Alias(run_alias, env.Command(run_alias, object, util.SilentAction(self._RunOneTest)))
        env.Alias('runtests', env.Command('runtests', self._tests, util.SilentAction(self._RunTests)))
        for path, targets in self._dir_tests.items():
            alias = path + '.runtests'
            env.Alias(alias, env.Command(alias, targets, util.SilentAction(self._RunTests)))
        # sample aliases for running tests:
        #    $ scons runtests
        #    $ scons cspsim.runtests
        #    $ scons csplib.runtests
        #    $ scons cspsim/test_PhysicsModel.run
        #    $ scons csplib/data/test_Object.run


BuildRegistry = _BuildRegistry()
