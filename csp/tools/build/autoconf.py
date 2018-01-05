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

from csp.tools.build import util
from csp.tools.build import scons
from subprocess import Popen, PIPE

import pickle
import re

from distutils import sysconfig

GetPythonInclude = sysconfig.get_python_inc
GetPythonLibrary = sysconfig.get_python_lib


def CheckSConsVersion(minimum):
    version = scons.GetVersion()
    if util.CompareVersions(version, minimum) < 0:
        print('SCons version %s is too old.  Please install'
              'version %s or newer.' % (version, minimum))
        sys.exit(1)


def CheckPythonVersion(minimum):
    version = '.'.join(map(str, sys.version_info[:3]))
    if util.CompareVersions(version, minimum) < 0:
        print('Python version %s is too old.  Please install'
              'version %s or newer.' % (version, minimum))
        sys.exit(1)


def CheckWXVersion(minimum):
    try:
        import wx
        version = wx.version()
        if util.CompareVersions(version, minimum) < 0:
            print('wxPython version (%s) is too old. Please install %s or newer.' % (version, minimum))
            sys.exit(1)
    except ImportError:
        print('WARNING: Missing wx python module. Please install wxPython')


def CheckLXMLVersion():
    try:
        import lxml
    except ImportError:
        print('Missing lxml python module used by the layout2 tool. Please install.')
        sys.exit(1)

    # Don't know how to test which version that is installed.
    # Just check that it is installed for now.


def GetGCCVersion():
    p = Popen('gcc -dumpfullversion', shell=True, stdout=PIPE)
    out, err = p.communicate()
    version = out.strip()
    try:
        return tuple(map(int, version.split('.')))
    except Exception:
        return None


def CheckSwig(context, min_version, not_versions=[]):
    ok = 0
    _checking(context, 'swig', reset_cached=1)
    p = Popen(['%s' % context.env.get('SWIG', 'swig'), '-version'], stdin=PIPE, stdout=PIPE, stderr=PIPE, close_fds=True)
    swig_out, swig_err = p.communicate()
    if swig_err is not None:
        output = swig_err.splitlines() + swig_out.splitlines()
        output = " ".join([x.strip() for x in output])
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


# Lazy check that pkg-config is installed.
HAS_PKG_CONFIG = None


def _CheckPkgConfigWorks(context):
    global HAS_PKG_CONFIG
    if HAS_PKG_CONFIG is None:
        _checking(context, 'pkg-config', reset_cached=1)
        p = Popen('pkg-config --version', shell=True, stdin=PIPE, stdout=PIPE, stderr=PIPE)
        output, error = p.communicate()
        version = output.decode().strip()
        error = error.decode().strip()
        if not error and re.match(r'\d', version) is not None:
            HAS_PKG_CONFIG = 1
            context.Result("yes (%s)" % version)
        else:
            HAS_PKG_CONFIG = 0
            context.Result("no")
            context.Log(error)
    return HAS_PKG_CONFIG


def CheckPkgConfig(context, lib, version=None, lib_name=None, command='pkg-config', version_flag='--modversion', config_flags='--cflags --libs'):
    has_pkg_config = _CheckPkgConfigWorks(context)
    if lib_name is None:
        lib_name = lib
    _checking(context, lib, version, reset_cached=1)
    if not has_pkg_config:
        context.Result("unknown (need pkg-config)")
        return 0
    p = Popen('%s %s %s' % (command, version_flag, lib), shell=True, stdout=PIPE, stderr=PIPE)
    output, error = p.communicate()
    ok = 0
    if output is not None:
        lib_version = output.decode().strip()
        ok = (version is None) or (util.CompareVersions(lib_version, version) >= 0)
    if ok:
        context.Result("yes (%s)" % lib_version)
        context.env['%s_VERSION' % lib_name.upper()] = lib_version
        context.env.ParseConfig('%s %s %s' % (command, config_flags, lib))
    else:
        context.Result("no")
    return ok


def CheckCommandVersion(context, lib, command, min_version=None, lib_name=None):
    if lib_name is None:
        lib_name = lib
    _checking(context, lib, min_version, reset_cached=1)
    p = Popen(command, shell=True, stdin=PIPE, stdout=PIPE, stderr=PIPE, close_fds=True)
    output, error = p.communicate()
    version = output.decode().strip()
    error = error.decode().strip()
    ok = not error and (util.CompareVersions(version, min_version) >= 0)
    if ok:
        context.Result("yes (%s)" % version)
        context.env['%s_VERSION' % lib_name.upper()] = version
    else:
        context.Result("no")
        context.Log(error)
    return ok


def _checking(context, target, min_version=None, reset_cached=0):
    if min_version is None:
        context.Message('Checking for %s... ' % (target))
    else:
        context.Message('Checking for %s (>= %s)... ' % (target, min_version))
    if reset_cached:
        # hack into scons internals to prevent the "(cached)" message from
        # displaying when the subsequent check doesn't use the SConf
        # infrastructure (e.g., a straight popen test).
        context.sconf.cached = 0


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
        config = open(config_file, 'rb')
    except IOError:
        return None
    # hack to rerun the config if the SConstruct file is modified; this can
    # probably be done more cleanly using regular scons dependencies.  also
    # shouldn't hardcode the name 'SConstruct' here.
    t0 = scons.FS.getmtime(scons.File('#/SConstruct').abspath)
    t1 = scons.FS.getmtime(scons.File('#/.config').abspath)
    if t0 > t1:
        return None
    return pickle.load(config)


def SaveConfig(env, config):
    config_file = env.GetBuildPath('.config')
    pickle.dump(config, open(config_file, 'wb'))
