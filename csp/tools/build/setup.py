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
            if not isinstance(targets, list):
                targets = [targets]
            result.extend(targets)
    return SCons.Script.Flatten(result)


def FinalizePackages(env):
    registry.BuildRegistry.Build(env)


def GlobalSetup(env):
    import SCons.Tool.swig
    if not SCons.Tool.swig.exists(env):
        SCons.Tool.swig.generate(env)

    builders.AddBuilders(env)
    util.AddPhonyTarget(env, 'config')
    SConsEnvironment.CopyEnvironment = util.CopyEnvironment
    SConsEnvironment.SetConfig = autoconf.SetConfig
    SConsEnvironment.Documentation = MakeDocumentation
    SConsEnvironment.ReadPackages = ReadPackages
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
