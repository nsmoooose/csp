# utilities for building devpack libraries

import SCons.Environment
import SCons.Defaults

import os

CPPFLAGS = '/D_MBCS /DWIN32 /D_WINDOWS /D_DLL /D_USRDLL /DNDEBUG /D_CRT_SECURE_NO_DEPRECATE'.split()
CFLAGS = '/nologo /GR /O2 /MD /EHsc'.split()
LFLAGS = '/nologo /INCREMENTAL:NO /RELEASE'.split()
Alias = SCons.Defaults.DefaultEnvironment().Alias

def makeEnvironment(**kw):
	env = SCons.Environment.Environment(ENV=os.environ, CPPFLAGS=CPPFLAGS, CCFLAGS=CFLAGS, CXXFLAGS=CFLAGS, LINKFLAGS=LFLAGS)
	env.AppendUnique(**kw)
	env.Append(BUILDERS = {'dll': _dll, 'lib': _lib, 'exe': _exe})
	return env

def _dll(env, target, sources, alias=[], depends=[], **kw):
	if depends:
		env = env.Clone()
		for dep in depends:
			dep.apply(env)
	dll, lib, exp = env.SharedLibrary(target, sources, **kw)
	MT = 'mt /nologo /manifest ${TARGET}.manifest /outputresource:${TARGET};#2'
	env.AddPostAction(dll, MT)
	alias.append('all')
	Alias(alias, [dll, lib])
	return (dll, lib)

def _lib(env, target, sources, alias=[], **kw):
	lib = env.StaticLibrary(target, sources, **kw)
	alias.append('all')
	Alias(alias, lib)

def _exe(env, target, sources, alias=[], depends=[], **kw):
	if depends:
		env = env.Clone()
		for dep in depends:
			dep.apply(env)
	exe = env.Program(target, sources, **kw)
	MT = 'mt /nologo /manifest ${TARGET}.manifest /outputresource:${TARGET}'
	env.AddPostAction(exe, MT)
	alias.append('all')
	Alias(alias, exe)

class Library:
	def __init__(self, name, dir, include='include', lib='.'):
		self.name = name
		self.dir = dir
		self.include = os.path.join(dir, include)
		self.lib = os.path.join(dir, lib)
	def apply(self, env):
		env.AppendUnique(CPPPATH=[self.include], LIBPATH=[self.lib], LIBS=[self.name + '.lib'])


ogg = Library('libogg', '#/libogg-1.1.3')
vorbis = Library('libvorbis', '#/libvorbis-1.1.2')
vorbisfile = Library('libvorbisfile', '#/libvorbis-1.1.2')
openalpp = Library('openalpp', '#/openalpp')
OpenThreads = Library('OpenThreadsWin32', '#/OSG_OP_OT-1.0/OpenThreads')
Producer = Library('Producer', '#/OSG_OP_OT-1.0/Producer')
sigc = Library('sigc-2.0', '#/libsigc++-2.0.16')
png = Library('libpng', '#/libpng-1.2.8-config', include='.')
zlib = Library('zdll', '#/zlib-1.2.3', include='.')
ccgnu2 = Library('ccgnu2', '#/commoncpp2-1.3.22')
ccext2 = Library('ccext2', '#/commoncpp2-1.3.22')
jpeg = Library('libjpeg', '#/libjpeg/jpeg-6b', include='.')
freetype  = Library('freetype219MT', '#/freetype-2.1.9')
osg = Library('osg', '#/OSG_OP_OT-1.0/OpenSceneGraph')
osgDB = Library('osgDB', '#/OSG_OP_OT-1.0/OpenSceneGraph')
osgUtil = Library('osgUtil', '#/OSG_OP_OT-1.0/OpenSceneGraph')
sdl_image = Library('SDL_image', '#/SDL_image-1.2.4', include='.')
OpenAL = Library('OpenAL32', 'c:/Program Files/OpenAL 1.1 SDK', lib='libs/Win32')
alut = Library('alut', 'c:/Program Files/OpenAL 1.1 SDK', lib='libs/Win32')

# the sdl source distribution puts the headers in include/. rather than
# the normal installation path SDL/.  just copy them by hand to
# custom/SDL so that the include path below will work.
sdl = Library('SDL', '#/SDL-1.2.9', include='custom')
