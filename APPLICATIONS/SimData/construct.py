# -*-python-*-

# SimData: Data Infrastructure for Simulations
# Copyright (C) 2002, 2003 Mark Rose <tm2@stm.lbl.gov>
# 
# This file is part of SimData.
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

##
# Utilities for building and installing with SCons 


import SCons.Action
import SCons.Builder
import SCons.Scanner 
import SCons.Util

Action = SCons.Action.Action
Builder = SCons.Builder.Builder
Scanner = SCons.Scanner.Base

import os, os.path, re, shutil

# configure tests (defined below)
configure_tests = {}



############################################################################
# python module and package installation
############################################################################

def getPythonInc():
	from distutils import sysconfig
	return sysconfig.get_python_inc()

def getPythonLib():
	from distutils import sysconfig
	return sysconfig.get_python_lib()

def installFile(target, source, env):
	"""Install a source file into a target using the function specified
	as the INSTALL construction variable."""
	try:
		install = env['INSTALL']
	except KeyError:
		raise SCons.Errors.UserError('Missing INSTALL construction variable.')
	return install(target[0].path, source[0].path, env)

def byteCompile(target, source, env):
	"""Byte compile a python source file."""
	target = str(target[0])
	if target.endswith(".py") and os.path.exists(target):
		import py_compile
		try:
			py_compile.compile(target)  
			os.chmod(target+"c", 0644)
			return 0
		except:
			pass
	return 0

def installFileString(target, source, env):
	source = str(source[0])
	target = str(target[0])
	return 'copying %s -> %s' % (source, target)

def byteCompileString(target, source, env):
	target = str(target[0])
	if target.endswith(".py"):
		return 'byte-compiling %s to %sc' % (target, os.path.basename(target))

installFileAction = Action(installFile, installFileString)
byteCompileAction = Action(byteCompile, byteCompileString)

PythonInstallBuilder = Builder(action=[installFileAction, byteCompileAction])

def installPythonSources(env, dir, source):
	"""Install specified files in the given directory."""
	import SCons.Node
	sources = SCons.Node.arg2nodes(source, env.fs.File)
	dnodes = SCons.Node.arg2nodes(dir, env.fs.Dir)
	tgt = []
	for dnode in dnodes:
		for src in sources:
			target = SCons.Node.FS.default_fs.File(src.name, dnode)
			tgt.append(PythonInstallBuilder(env, target, src))
	if len(tgt) == 1:
		tgt = tgt[0]
	return tgt



############################################################################
# general utilities
############################################################################

def compareVersions(a, b):
	a = map(int, a.split('.'))
	b = map(int, b.split('.'))
	for i in range(min(len(a), len(b))):
		if a[i] < b[i]: return -1
		if a[i] > b[i]: return 1
	if len(a) < len(b): return -1
	if len(a) > len(b): return 1
	return 0

def addConfigTests(conf):	
	conf.AddTests(configure_tests)


############################################################################
# additional builders
############################################################################

def addDoxygen(env):
	def dox(target, source, env):
		cwd = os.getcwd()
		os.chdir(os.path.dirname(str(source[0])))
		sources = map(os.path.basename, map(str, source))
		result = os.system("doxygen %s" % " ".join(sources))
		os.chdir(cwd)
		return result
	#action = 'cd $SOURCE.dir && doxygen $SOURCES.file'
	action = dox
	env.Append(BUILDERS = {'Doxygen': Builder(action=action)})

def addCopyFile(env):
	def copy(target, source, env):
		shutil.copy(str(source[0]), str(target[0]))
	def report(target, source, env):
		source = str(source[0])
		target = str(target[0])
		return 'copying %s -> %s' % (source, target)
	CopyFile = Builder(action=Action(copy, report))
	env.Append(BUILDERS = {'CopyFile': CopyFile})


############################################################################
# SWIG support
############################################################################

SWIG = 'swig'

def checkSwig(context, req_version):
	ok = 0
	context.Message("Checking for swig %s..." % req_version)
	swig_in, swig_out, swig_err = os.popen3('%s -version' % SWIG, 't', 1024)
	if swig_err is not None:
		output = swig_err.readlines()
		output = " ".join(map(lambda x: x.strip(), output))
		match = re.search(r'SWIG Version (\d+\.\d+.\d+)', output)
		if match is not None:
			swig_version = match.groups()[0]
			if compareVersions(swig_version, req_version) >= 0:
				ok = 1
	if ok:
		context.Result("yes (%s)" % swig_version)
	else:
		context.Result("no")
	return ok 

configure_tests['checkSwig'] = checkSwig


def addSwigLib(env):
	# XXX this should work according to the scons dev docs, but builders
	# don't seem to have a Copy() method yet.
	#swiglib = env.SharedLibrary.Copy(SHLIBPREFIX = '', LIBS = env["SWLIBS"])
	#env.Append(BUILDERS = {'SwigLibrary': swiglib})
	pass

def addSwigBuild(env):
	action = '$SWIG $SWFLAGS $_CPPINCFLAGS -o ${TARGETS[0]} $SOURCES'
	SwigBuild = Builder(action=action,
			    src_suffix=".i")
	env.Append(BUILDERS = {'SwigBuild': SwigBuild})

def addSwigDep(env):
	def SwigScanner(node, env, path, arg=None):
		cmd = env.subst('$SWIG -MM $_CPPINCFLAGS %s' % str(node))
		stdin, stdout, stderr = os.popen3(cmd, 't')
		deps = ''.join(map(lambda x: x.strip(), stdout.readlines()))
		deps = map(lambda x: "#/"+x.strip(), deps.split('\\'))[1:]
		return deps
	scanner = Scanner(function = SwigScanner, skeys = ['.i'], recursive = 0)
	env.Append(SCANNERS = scanner)

def addSwigSupport(env):
	global SWIG
	SWIG = SCons.Util.WhereIs('swig')
	env['SWIG'] = SWIG
	addSwigDep(env)
	addSwigLib(env)
	addSwigBuild(env)


############################################################
# helper classes
############################################################

class OptionSet:
	class Empty: 
		pass
	def __init__(self):
		self.set = OptionSet.Empty()
		self.add = OptionSet.Empty()
	def __getattr__(self, x):
		d = self.__dict__
		if x in d.keys(): return d[x]
		return getattr(d['set'], x)
	def methods(self):
		return self.set, self.add
	def apply(self, env):
		for key, value in self.set.__dict__.iteritems():
				env[key] = value
		for key, value in self.add.__dict__.iteritems():
				env.Append(**{key: value})

class Globals:
	class Empty:
		pass
	def __init__(self):
		self.add = Globals.Empty()
	def set(self):
		for key, value in self.set.__dict__.iteritems():
			eval("%s=%s" % (key, value), globals(), globals())

