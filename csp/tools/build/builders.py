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
import shutil
import sys

import SCons.Builder
import SCons.Scanner
import SCons.Tool

from csp.tools.build import scons


def AddDoxygen(env):
    env['DOXYGEN'] = 'doxygen'
    env['DOXYGENCOM'] = 'cd $SOURCE.dir && $DOXYGEN $SOURCES.file 1>.dox.log 2>.dox.err'
    env.Append(BUILDERS={'Doxygen': SCons.Builder.Builder(action=SCons.Action.Action('$DOXYGENCOM', '$DOXYGENCOMSTR'))})


def AddCopyFile(env):
    def copy(target, source, env):
        source, target = source[0].abspath, target[0].abspath
        shutil.copy(source, target)
    CopyFile = SCons.Builder.Builder(action=SCons.Action.Action(copy, '$COPYFILECOMSTR'))
    env.Append(BUILDERS={'CopyFile': CopyFile})


def AddLinkFile(env):
    def symlink(target, source, env):
        source, target = source[0].abspath, target[0].abspath
        if os.name == 'posix':
            os.link(source, target)
        else:
            shutil.copy(source, target)
    LinkFile = SCons.Builder.Builder(action=SCons.Action.Action(symlink, '$SYMLINKCOMSTR'))
    env.Append(BUILDERS={'LinkFile': LinkFile})


def EmitNet(target, source, env):
    assert(len(source) == 1)
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
    cxx_file.add_action('.net', SCons.Action.Action('$TRC', '$TRCCOMSTR'))
    trc = env.File('#/tools/trc/trc.py')
    env['TRC'] = '%s --source=${TARGETS[0]} --header=${TARGETS[1]} $SOURCES' % trc


def AddVisualStudioProject(env):
    def VisualStudioProject(env, target, source):
        if util.IsWindows(env) and hasattr(env, 'MSVSProject'):
            if isinstance(target, list):
                target = target[0]
            project_base = os.path.splitext(os.path.basename(target.name))[0]
            if project_base.startswith('_'):
                project_base = project_base[1:]
            dbg = env.MSVSProject(target=project_base+'-dbg.vcproj', srcs=source.sources,
                                  incs=source.headers, misc=source.misc, buildtarget=target, variant='Debug')
            rel = env.MSVSProject(target=project_base+'-rel.vcproj', srcs=source.sources,
                                  incs=source.headers, misc=source.misc, buildtarget=target, variant='Release')
            return [dbg, rel]
        else:
            return []
    env.Append(BUILDERS={'VisualStudioProject': VisualStudioProject})


def AddBuilders(env):
    # env['MSVS_VERSION'] = '7.1'
    # SCons.Tool.msvs.generate(env)
    AddDoxygen(env)
    AddCopyFile(env)
    AddLinkFile(env)
    AddNet(env)
    AddVisualStudioProject(env)
