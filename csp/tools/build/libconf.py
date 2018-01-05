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
import re
import sys

from csp.tools.build import registry
from csp.tools.build import util


class ExternalLibrary:
    def __init__(self, name='', config=[]):
        assert name
        self._name = name
        self._config = config
        self._settings = util.Settings()
        registry.BuildRegistry.AddLibrary(name, self)

    def _save(env, oldenv):
        settings = {}
        for key in ('CCFLAGS', 'CPPPATH', 'CPPFLAGS', 'LIBS', 'LIBPATH', 'LINKFLAGS'):
            new_values = env.get(key, [])
            old_values = oldenv.get(key, [])
            for old in old_values:
                if old in new_values:
                    new_values.remove(old)
            settings[key] = new_values
        return settings
    _save = staticmethod(_save)

    def config(self, conf, global_settings):
        valid = 1
        oldenv = conf.env.Clone()
        settings = util.Settings()
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
        if label is None:
            label = package
        self._package = package
        self._version = version
        self._label = label

    def configure(self, conf):
        if util.IsWindows(conf.env):
            return 1
        return conf.CheckPkgConfig(self._package, version=self._version, lib_name=self._label)


class CommandConfig:
    def __init__(self, package=None, version_command=None, flags_command=None, version=None, label=None):
        self._package = package
        self._version_command = version_command
        self._flags_command = flags_command
        self._version = version
        self._label = label

    def configure(self, conf):
        if util.IsWindows(conf.env):
            return 1
        valid = conf.CheckCommandVersion(self._package, self._version_command, self._version, lib_name=self._label)
        if valid:
            conf.env.ParseConfig(self._flags_command)
        return valid


class LibConfig:
    def __init__(self, lib=None, symbol='', label=None):
        if label is None:
            label = lib
        self._lib = lib
        self._symbol = symbol
        self._label = label

    def configure(self, conf):
        return conf.CheckLib(self._lib, self._symbol)


class UnixLibConfig(LibConfig):
    def __init__(self, lib=None, symbol='', label=None):
        LibConfig.__init__(self, lib, symbol, label)

    def configure(self, conf):
        if util.IsWindows(conf.env):
            return 1
        return conf.CheckLib(self._lib, self._symbol)


class WindowsLibConfig(LibConfig):
    # using atoi as the default test symbol since it always succeeds.
    # haven't yet figured out how to test real symbols in many libraries,
    # probably due to complications with declspec.
    def __init__(self, lib=None, symbol='atoi', label=None):
        LibConfig.__init__(self, lib, symbol, label)

    def configure(self, conf):
        if not util.IsWindows(conf.env):
            return 1
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
        if DevpackConfig.DEVPACK:
            return
        path = os.environ.get('CSPDEVPACK', '')
        if not path:
            print 'CSPDEVPACK environment variable not set.'
            sys.exit(1)
        if path.startswith('"') and path.endswith('"'):
            path = path[1:-1]
        try:
            v = map(int, re.search(r'([0-9.]+)"?$', path).group(1).split('.'))
        except Exception:
            print('ERROR: CSPDEVPACK environment variable (%s) does not look like a valid devpack path.' % path)
            sys.exit(1)
        if v < map(int, DevpackConfig.VERSION.split('.')):
            print('ERROR: The installed devpack (%s) is too old; need version %s' % (path, DevpackConfig.VERSION))
            sys.exit(1)
        DevpackConfig._checkPath(path)
        DevpackConfig._checkPath(path, 'bin')
        DevpackConfig._checkPath(path, 'lib')
        DevpackConfig._checkPath(path, 'include')
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
        if util.IsWindows(conf.env):
            DevpackConfig._Find()
            dp = DevpackConfig.DEVPACK
            for dll in self._dlls:
                self._checkPath(dp, 'bin', dll + '.dll')
                self._checkPath(dp, 'lib', dll + '.lib')
            for lib in self._libs:
                self._checkPath(dp, 'lib', lib + '.lib')
            for header in self._headers:
                if isinstance(header, tuple):
                    header = os.path.join(*header)
                self._checkPath(os.path.join(dp, 'include', header))
            conf.env.AppendUnique(CPPPATH=[os.path.join(dp, 'include')])
            conf.env.AppendUnique(LIBPATH=[os.path.join(dp, 'lib')])
            conf.env.AppendUnique(LIBS=self._dlls + self._libs)
        return 1
