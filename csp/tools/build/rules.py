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


from csp.tools.build import autoconf
from csp.tools.build import scons
from csp.tools.build import registry
from csp.tools.build import util

import os
import re
import sys
import SCons
import SCons.Script


class SourceGroup:
    def __init__(self, env, name='', sources=[], deps=[], **kw):
        self._env = env.Clone()
        self._name = name
        self._sources = [x for x in env.arg2nodes(sources)]
        self._deps = deps
        self._objects = None
        self._options = kw
        self._bdeps = []
        registry.BuildRegistry.AddSourceGroup(name, self)

    def _makeObjects(self):
        if self._objects is None:
            settings = util.Settings()
            for dep in self._deps:
                lib = registry.BuildRegistry.GetDependency(dep)
                if lib:
                    lib._addSettings(settings, self._bdeps)
                else:
                    print('%s: could not find dependency %s' % (self._name, dep))
                    sys.exit(1)
            self._settings = settings
            settings.apply(self._env)
            self._objects = []
            for file in self._sources:
                if isinstance(file, SCons.Node.FS.File):
                    if file.get_suffix() == '.i':
                        swigEnv = self._env.Clone()
                        if swigEnv.get('SWIGCXXFLAGS') is not None:
                            swigEnv.Replace(CXXFLAGS=swigEnv['SWIGCXXFLAGS'])
                        self._objects.append(swigEnv.SharedObject(file, **self._options))
                    elif file.get_suffix() != '.h':
                        self._objects.append(self._env.SharedObject(file, **self._options))
                else:
                    self._objects.append(file)

    def _addSettings(self, settings, bdeps):
        self._makeObjects()
        settings.merge(self._settings)
        bdeps.extend(self._bdeps)

    def add(self, objects, settings, bdeps):
        self._makeObjects()
        objects.extend(self._objects)
        self._addSettings(settings, bdeps)

    _header_ext = ('.h', '.hh', '.hpp', '.hxx')
    _source_ext = ('.c', '.cc', '.cpp', '.cxx')
    _swig_ext = ('.i',)
    _message_ext = ('.net',)
    _known_ext = _header_ext + _source_ext + _swig_ext + _message_ext

    def getAll(self):
        return [x.srcnode() for x in self._sources]

    def getHeaders(self):
        return [x.srcnode() for x in self._sources if x.get_suffix() in SourceGroup._header_ext]

    def getSources(self):
        return [x.srcnode() for x in self._sources if x.get_suffix() in SourceGroup._source_ext]

    def getMessages(self):
        return [x.srcnode() for x in self._sources if x.get_suffix() in SourceGroup._message_ext]

    def getSwigInterfaces(self):
        return [x.srcnode() for x in self._sources if x.get_suffix() in SourceGroup._swig_ext]

    def getMiscellaneousFiles(self):
        header_or_source = SourceGroup._header_ext + SourceGroup._source_ext
        return [x.srcnode() for x in self._sources if x.get_suffix() not in header_or_source]

    def getOtherFiles(self):
        return [x.srcnode() for x in self._sources if x.get_suffix() not in SourceGroup._known_ext]

    all = property(getAll)
    headers = property(getHeaders)
    sources = property(getSources)
    messages = property(getMessages)
    misc = property(getMiscellaneousFiles)
    swig = property(getSwigInterfaces)
    other = property(getOtherFiles)


class Target:
    def __init__(self, env, name, sources=[], aliases=[], deps=[], always_build=0, softlink=0, doxygen=None, is_test=0, **kw):
        self._env = env.Clone()
        self._name = name
        self._sources = [s for s in sources if s.startswith('@')]
        pure = [s for s in sources if not s.startswith('@')]
        if pure or deps:
            SourceGroup(env, name, sources=pure, deps=deps)
            self._sources.append('@' + name)
        self._path = env.Dir('.').srcnode().path
        self._target = os.path.join(env.Dir('.').path, name)  # a bit ugly
        if isinstance(aliases, str):
            aliases = [aliases]
        else:
            aliases = aliases[:]
        if is_test:
            aliases.append('tests')
        self._aliases = aliases
        self._is_test = is_test or ('tests' in aliases)  # the latter is for backward compatibility
        self._always_build = always_build
        self._softlink = softlink
        self._doxygen = None
        if doxygen:
            self._dox = env.Dir('.dox').srcnode()
            self._doxygen = env.File(doxygen)
        self._options = kw
        self._groups = None
        self._output = None
        registry.BuildRegistry.AddTarget(name, self)

    def isTest(self):
        return self._is_test

    def build(self):
        if self._output:
            return self._output
        objects = []
        settings = util.Settings()
        self._findSources()
        bdeps = []
        for group in self._groups:
            group.add(objects, settings, bdeps)
        bdeps = SCons.Script.Flatten(bdeps)
        objects = SCons.Script.Flatten(objects)
        settings.merge(self._options)
        settings.apply(self._env)
        target = self._apply(objects)

        scons.Alias(self._name, target)
        if self._aliases:
            scons.Alias(self._aliases, target)
        if self._doxygen:
            self._env.Documentation(self._dox, self._doxygen, self._sources)
        if self._always_build:
            scons.AlwaysBuild(target)
        self._output = target
        if bdeps and not self._env.GetOption('clean'):
            scons.Depends(target, bdeps)
        return target  # [0]

    def _findSources(self):
        if self._groups is None:
            self._groups = []
            for source in self._sources:
                group = registry.BuildRegistry.GetSourceGroup(source)
                if not group:
                    print('%s: unknown source %s' % (self._name, source))
                    sys.exit(1)
                self._groups.append(group)

    def _addSettings(self, settings, bdeps):
        bdeps.append(self.build())
        self._findSources()
        for group in self._groups:
            group._addSettings(settings, bdeps)

    def _addRPath(self):
        xrpath = self._env.get('XRPATH', [])
        if xrpath:
            rpath = []
            target_path = os.path.abspath(os.path.dirname(self._target)).split(os.sep)
            for path in xrpath:
                path = path.split(os.sep)
                for idx, dir in enumerate(target_path):
                    if path[0] == dir:
                        path = path[1:]
                    else:
                        path = ['..'] * (len(target_path) - idx) + path
                        break
                rpath.append(os.path.join(r'\$\ORIGIN', *path))

            self._env.AppendUnique(RPATH=rpath)
            self._env.AppendUnique(LINKFLAGS=['$__RPATH'])

    def _apply(self):
        pass


class Program(Target):
    def _apply(self, objects):
        self._addRPath()
        program = self._env.Program(self._target, objects)
        return program


class Generate(Target):
    def __init__(self, env, command, targets, sources=[], **kw):
        name = '__' + '_'.join(targets)
        Target.__init__(self, env, name, sources, **kw)
        self._target = targets
        self._command = command

    def _apply(self, objects):
        command = self._command
        objects = []
        while 1:
            match = re.search(r'{(.*?)}', command)
            if match is None:
                break
            var = match.group(1)
            target = registry.BuildRegistry.GetTarget(var).build()[0]
            objects.append(target)
            command = command[:match.start(0)] + target.abspath + command[match.end(0):]
        self._env.Command(self._target, objects, SCons.Action.Action(command, '$GENERATECOMSTR'))
        return self._target


class SharedLibrary(Target):
    def _apply(self, objects):
        self._addRPath()
        shlib = self._env.SharedLibrary(self._target, objects)
        if util.IsWindows(self._env):
            self._bindManifest(shlib)
        return shlib

    MT_BIN = 0

    def _bindManifest(self, shlib):
        if SharedLibrary.MT_BIN == 0:
            SharedLibrary.MT_BIN = scons.WhereIs('mt')
            if not SharedLibrary.MT_BIN:
                print('WARNING: could not find mt.exe, will not bind manifests')
        if SharedLibrary.MT_BIN:
            dll = shlib[0]
            CMD = '"%s" /nologo /manifest ${TARGET}.manifest /outputresource:${TARGET};#2' % SharedLibrary.MT_BIN
            MSG = '- Binding manifest to %s' % dll.name
            self._env.AddPostAction(dll, util.SimpleCommand(CMD, MSG))

    def _addSettings(self, settings, bdeps):
        Target._addSettings(self, settings, bdeps)
        if util.IsWindows(self._env) or not self._softlink:
            settings.merge(LIBPATH=[os.path.dirname(self._target)], LIBS=[os.path.basename(self._target)])
        else:
            target_dir = os.path.dirname(self._target)
            settings.merge(XRPATH=[os.path.abspath(target_dir)], LIBPATH=[target_dir], LIBS=[os.path.basename(self._target)])


class PythonSharedLibrary(SharedLibrary):
    def __init__(self, env, **kw):
        libEnv = env.Clone()
        libEnv['SHLIBPREFIX'] = '_'
        if util.IsWindows(env):
            if int(libEnv.get('CONFIG_DEBUG', 0)) == 2:
                libEnv['SHLIBSUFFIX'] = '_d.pyd'
            else:
                libEnv['SHLIBSUFFIX'] = '.pyd'
        SharedLibrary.__init__(self, libEnv, **kw)


class Test(SharedLibrary):
    def __init__(self, env, **kw):
        kw['is_test'] = 1
        testEnv = env.Clone()
        testEnv['no_import_lib'] = 1
        SharedLibrary.__init__(self, testEnv, **kw)


class Command(Target):
    def __init__(self, env, function, **kw):
        name = getattr(function, '__name__', None)
        if not name:
            klass = getattr(function, '__class__', None)
            if klass:
                name = klass.__name__
            else:
                name = str(name)
        Target.__init__(self, env, name, [], **kw)
        self._command = function

    def _apply(self, objects):
        return self._env.Command(self._target, objects, self._command)
