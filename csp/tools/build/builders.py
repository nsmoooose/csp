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

if __name__ == '__main__':
	print 'This module provides support for the SCons build system; it is not'
	print 'intended to be run as a standalone program.'
	sys.exit(1)


from csp.tools.build import scons

import os
import re
import shutil

import SCons.Builder
import SCons.Scanner
import SCons.Tool


def EmitSwig(target, source, env):
	target = []
	assert(len(source)==1)
	ext = env['CXXFILESUFFIX']
	for s in source:
		wrapper = s.target_from_source('', '_wrap'+ext)
		target.append(wrapper)
		module = None
		# first try to find %module directive in the swig interface.  this is
		# a fairly simplistic search that can easily be tripped up (e.g., by
		# multiline comments.
		re_module = re.compile(r'^\s*%module\s+([a-zA-Z0-9_]+)')
		for line in open(str(s)):
			m = re_module.match(line)
			if m is not None:
				module = s.dir.File(m.group(1) + '.py')
				break
		# if a %module directive wasn't found, assume the output module has
		# the same name as the interface file.  to be thorough we should also
		# consider swig's -module flag, but this isn't implemented yet.
		if module is None:
			module = s.target_from_source('', '.py')
		target.append(module)
	return (target, source)


def AddSwigBuild(env):
	env['SWIGCOM'] = '$SWIG $SWIGFLAGS $_SWIGINCFLAGS -o $TARGET $SOURCE'
	env['SWIGINCPREFIX'] = '-I'
	env['SWIGINCSUFFIX'] = '$INCSUFFIX'
	env['SWIGCXXFLAGS'] = '$CXXFLAGS'
	env['_SWIGINCFLAGS'] = '$( ${_concat(SWIGINCPREFIX, SWIGINCLUDES, SWIGINCSUFFIX, __env__, RDirs)} $)'
	builder = SCons.Builder.Builder(action = '$SWIG $SWIGFLAGS $_SWIGINCFLAGS -o ${TARGETS[0]} $SOURCE', emitter = EmitSwig)
	env.Append(BUILDERS = {'Swig': builder})
	def wrapper_builder(env, target = None, source = SCons.Builder._null, **kw):
		kw.setdefault('CXXFLAGS', env['SWIGCXXFLAGS'])
		return env.SharedObject(target, source, **kw)
	env.Append(BUILDERS = {'SwigWrapper': wrapper_builder })


def AddSwigDep(env):
	def SwigScanner(node, env, path, arg=None):
		cmd = env.subst('$SWIG $SWIGFLAGS $SWIGINCLUDES -MM %s' % str(node))
		stdin, stdout, stderr = os.popen3(cmd, 't')
		deps = ''.join(map(lambda x: x.strip(), stdout.readlines()))
		deps = map(lambda x: "#/"+x.strip(), deps.split('\\'))[1:]
		return deps
	scanner = SCons.Scanner.Scanner(name = 'SwigScanner', function = SwigScanner, skeys = ['.i'], recursive = 0)
	env.Append(SCANNERS = scanner)


def AddSwigSupport(env, required=1):
	SWIG = scons.WhereIs('swig')
	if not SWIG:
		print 'WARNING: swig not found in path'
		if required:
			print >>sys.stderr, 'Cannot continue without swig.'
			sys.exit(1)
	AddSwigDep(env)
	AddSwigBuild(env)


def AddDoxygen(env):
	def report(target, source, env):
		source = str(source[0])
		return 'doxygen %s' % source
	action = 'cd $SOURCE.dir && doxygen $SOURCES.file 1>.dox.log 2>.dox.err'
	env.Append(BUILDERS = {'Doxygen': SCons.Builder.Builder(action=SCons.Action.Action(action, report))})


def AddCopyFile(env):
	def copy(target, source, env):
		shutil.copy(str(source[0]), str(target[0]))
	def report(target, source, env):
		source = str(source[0])
		target = str(target[0])
		return 'copying %s -> %s' % (source, target)
	CopyFile = SCons.Builder.Builder(action=SCons.Action.Action(copy, strfunction=report))
	env.Append(BUILDERS = {'CopyFile': CopyFile})


def AddLinkFile(env):
	def copy(target, source, env):
		source = str(source[0])
		target = str(target[0])
		if os.name=='posix':
			os.link(source, target)
		else:
			shutil.copy(source, target)
	def report(target, source, env):
		source = str(source[0])
		target = str(target[0])
		return 'linking %s -> %s' % (source, target)
	CopyFile = SCons.Builder.Builder(action=SCons.Action.Action(copy, report))
	env.Append(BUILDERS = {'LinkFile': CopyFile})


def EmitNet(target, source, env):
	assert(len(source)==1)
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
	cxx_file.add_action('.net', '$TRC')
	trc = env.File('#/tools/trc/trc.py')
	env['TRC'] = '%s --source=${TARGETS[0]} --header=${TARGETS[1]} $SOURCES' % trc


def AddVisualStudioProject(env):
	def VisualStudioProject(env, target, source):
		if util.IsWindows(env) and hasattr(env, 'MSVSProject'):
			if isinstance(target, list): target = target[0]
			project_base = os.path.splitext(os.path.basename(target.name))[0]
			if project_base.startswith('_'): project_base = project_base[1:]
			dbg = env.MSVSProject(target=project_base+'-dbg.vcproj', srcs=source.sources, incs=source.headers, misc=source.misc, buildtarget=target, variant='Debug')
			rel = env.MSVSProject(target=project_base+'-rel.vcproj', srcs=source.sources, incs=source.headers, misc=source.misc, buildtarget=target, variant='Release')
			return [dbg, rel]
		else:
			return []
	env.Append(BUILDERS = {'VisualStudioProject': VisualStudioProject})


def AddBuilders(env):
	#env['MSVS_VERSION'] = '7.1'
	#SCons.Tool.msvs.generate(env)
	AddDoxygen(env)
	AddCopyFile(env)
	AddLinkFile(env)
	AddNet(env)
	AddVisualStudioProject(env)
