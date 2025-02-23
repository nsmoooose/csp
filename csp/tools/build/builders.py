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


def AddDoxygen(env):
    env['DOXYGEN'] = 'doxygen'
    env['DOXYGENCOM'] = 'cd $SOURCE.dir && $DOXYGEN $SOURCES.file 1>.dox.log 2>.dox.err'
    env.Append(BUILDERS={'Doxygen': SCons.Builder.Builder(action=SCons.Action.Action('$DOXYGENCOM', '$DOXYGENCOMSTR'))})


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


def AddBuilders(env):
    AddDoxygen(env)
    AddLinkFile(env)
    AddNet(env)
