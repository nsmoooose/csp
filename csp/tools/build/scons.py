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
# this is ok and the warning is annoying. This warning is shown when building
# doxygen documentation where the target is index.html for both csplib and cspsim.
import SCons.Warnings
SCons.Warnings.suppressWarningClass(SCons.Warnings.DuplicateEnvironmentWarning)

FS = SCons.Node.FS.default_fs
File = SCons.Node.FS.default_fs.File
Dir = SCons.Node.FS.default_fs.Dir
AlwaysBuild = SCons.Defaults.DefaultEnvironment().AlwaysBuild
Depends = SCons.Defaults.DefaultEnvironment().Depends


def SetReading():
    SCons.Script.SConscript.sconscript_reading = 1


def DisableQuiet():
    # override -Q
    if hasattr(SCons.SConf, 'SetProgressDisplay'):
        SCons.SConf.SetProgressDisplay(SCons.Util.display)


def GetOptions():
    return SCons.Script.Main.OptionsParser.values


def GetCommandlineTargets():
    return SCons.Script.SConscript.CommandLineTargets
