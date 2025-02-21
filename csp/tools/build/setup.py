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
import atexit
import logging
import os
import time

import SCons
import SCons.Script
import SCons.Script.SConscript
from SCons.Script.SConscript import SConsEnvironment

from csp.tools.build import autoconf
from csp.tools.build import builders
from csp.tools.build import registry
from csp.tools.build import util


def FinalizePackages(env):
    registry.BuildRegistry.Build(env)


def GlobalSetup(env):
    builders.AddBuilders(env)
    env.AlwaysBuild(env.Alias('config', [], env.Action([])))
    SConsEnvironment.CopyEnvironment = util.CopyEnvironment
    SConsEnvironment.SetConfig = autoconf.SetConfig
    SConsEnvironment.Documentation = MakeDocumentation
    SConsEnvironment.FinalizePackages = FinalizePackages


def MakeDocumentation(env, target, config, sources):
    target = SCons.Script.Dir(target)
    html = target.Dir('html')
    if 'dox' in SCons.Script.SConscript.CommandLineTargets:
        index = html.File('index.html')
        dox = env.Doxygen(index, SCons.Script.File(config))
        for item in sources:
            if isinstance(item, str):
                if item.startswith('@'):
                    item = registry.BuildRegistry.GetSourceGroup(item)
                    env.Depends(dox, item.all)
                else:
                    env.Depends(dox, util.Glob(item))
            else:
                print('implement me')
        env.Alias('dox', dox)
    env.Clean(['dox', 'all'], html)
