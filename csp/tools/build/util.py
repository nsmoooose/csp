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
import glob
import os
import sys

from SCons.Action import Action, CommandAction
from SCons.Builder import Builder
from SCons.Node.FS import default_fs


def Glob(path):
    if isinstance(path, string):
        path = [path]
    result = []
    for p in path:
        result.extend(glob.glob(str(default_fs.File(p))))
    return result


def CompareVersions(a, b):
    if (a.find('.') < 0) or (b.find('.') < 0):
        return -1

    # We compare number by number. And we do the string to int conversion
    # as late as possible. This due to the fact that some version numbers
    # contains dates and other characters in the end. For example (from a
    # scons release):
    #    1.0.0.d20080826
    # Hopefully we will find out if the version is good to use before we
    # get to the 'd' character.
    aa = a.split('.')
    bb = b.split('.')
    for index in range(min(len(aa), len(bb))):
        x = int(aa[index])
        y = int(bb[index])
        result = (x > y) - (x < y)
        if result != 0:
            return result
    return 0


def IsWindows(env):
    return env['PLATFORM'].startswith('mingw')


def SimpleCommand(cmd, msg):
    def s(target, src, env):
        print((msg % {'src': str(src), 'target': str(target[0])}))
    return CommandAction(cmd, strfunction=s)


def SilentAction(callback):
    def noprint(*args, **kw): pass
    return Action(callback, noprint)


def Apply(builder, sources, **overrides):
    def builder_wrap(source):
        return builder(source, **overrides)
    return list(map(builder_wrap, sources))


class Settings(dict):
    def merge(self, *args, **kw):
        settings = kw
        if args:
            assert len(args) == 1
            settings.update(args[0])
        for key, value in list(settings.items()):
            base = self.setdefault(key, [])
            if isinstance(value, list):
                for item in value:
                    if item not in base:
                        base.append(item)
            else:
                self[key] = value

    def apply(self, env):
        for key, value in self.items():
            if isinstance(value, list):
                env.AppendUnique(**{key: value})
            elif isinstance(value, tuple):
                env[key] = list(value)
            else:
                env[key] = value
