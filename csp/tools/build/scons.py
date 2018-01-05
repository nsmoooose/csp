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

import SCons
import SCons.Defaults
import SCons.Node
import SCons.Util
import SCons.Script

# several global targets (e.g. 'all') are shared between copied subenvironments;
# this is ok and the warning is annoying
import SCons.Warnings
SCons.Warnings.suppressWarningClass(SCons.Warnings.DuplicateEnvironmentWarning)

FS = SCons.Node.FS.default_fs
File = SCons.Node.FS.default_fs.File
Dir = SCons.Node.FS.default_fs.Dir
Alias = SCons.Defaults.DefaultEnvironment().Alias
AlwaysBuild = SCons.Defaults.DefaultEnvironment().AlwaysBuild
Depends = SCons.Defaults.DefaultEnvironment().Depends
Flatten = SCons.Util.flatten
WhereIs = SCons.Util.WhereIs
IsList = SCons.Util.is_List


def GetVersion():
    return SCons.__version__


def GetVersionTuple():
    return tuple(map(int, SCons.__version__.split('.')))


def SetReading():
    SCons.Script.SConscript.sconscript_reading = 1


def DisableQuiet():
    # override -Q
    if hasattr(SCons.SConf, 'SetProgressDisplay'):
        SCons.SConf.SetProgressDisplay(SCons.Util.display)


def GetOptions():
    return SCons.Script.Main.OptionsParser.values


def GetSettableOptions():
    return SCons.Script.Main.ssoptions


def GetCommandlineTargets():
    return SCons.Script.SConscript.CommandLineTargets


def GetCurrentScript():
    try:
        call_stack = SCons.Script.call_stack
    except AttributeError:
        call_stack = SCons.Script.SConscript.stack  # pre 0.96.91
    return call_stack[-1].sconscript


def TargetToString(x):
    if isinstance(x, list):
        return list(map(TargetToString, x))
    if isinstance(x, SCons.Node.FS.File):
        return 'FILE:%s' % x.abspath
    return str(x)


def SetDistributed(env):
    '''
    Use distcc if available to distribute the build across multiple hosts.
    distcc must be installed and configured correctly.
    '''
    CXX = env['CXX']
    if CXX is None:
        return
    distcc = WhereIs('distcc')
    if not distcc:
        print('Concurrent build requested, but distcc not found.')
        return
    # add distcc in '$( $)' escapes so that using it doesn't invalidate
    # objects that were built without it, and vice-versa.
    CXX = '$( %s $) %s' % (distcc, CXX)
    # distcc typically needs access to configuration files in the home directory.
    env['ENV']['HOME'] = os.environ.get('HOME', '')
    env.Replace(CXX=CXX)
    print('Using distcc for distributed build')
