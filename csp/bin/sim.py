#!/usr/bin/python

# Combat Simulator Project
# Copyright (C) 2002-2005 The Combat Simulator Project
# http://csp.sourceforge.net
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

"""
Combat Simulator Project : simulation startup script

Usage: %(prog)s [options]
"""

import sys
import os
import os.path


def error(message, die=0):
	sys.stderr.write(message + '\n')
	if die: sys.exit(1)

def fail(message):
	error(message, die=1)

def relativePath(a, b=None):
	"""
	Construct a relative path from a to b.  If b is not specified, returns
	the relative path from the current directory to a.
	"""
	if b is None:
		b = a
		a = os.path.curdir
	a = os.path.normpath(os.path.abspath(a))
	b = os.path.normpath(os.path.abspath(b))
	a = a.split(os.path.sep)
	b = b.split(os.path.sep)
	for i in range(min(len(a), len(b))):
		if a[i] != b[i]: break
	if i == 0:
		return os.path.join(*b)
	return os.path.join(*(['..'] * (len(a) - i) + list(b[i:])))


def checkModuleSpace():
	"""
	Check that the bootstrap module works correctly and that the top-level csp
	package can be imported.
	"""
	try:
		import csp as _csp
	except ImportError:
		fail('Unable to import the csp bootstrap module.  Check that you have run\n'
		     'setup.py successfully to initialize the workspace.')

	global csp
	csp = _csp


def importModules():
	"""Import csplib and cspsim modules, checking for errors."""
	try:
		import csp.csplib as _csplib
	except ImportError, e:
		reportImportError(e, 'csp.csplib')
	csp.csplib = _csplib

	try:
		import csp.cspsim as _cspsim
	except ImportError, e:
		reportImportError(e, 'csp.cspsim')
	csp.cspsim = _cspsim


def reportImportError(exception, module):
	msg = '   %s' % str(exception).strip()
	if len(msg) > 60:
		msg = msg.replace(": ", ":\n   ")
	msg = ('Error importing %s\n%s\n\n' % (module, msg))
	if msg.find('symbol') >= 0:
		msg += ('Unresolved symbols often indicate missing libraries or incorrect link options.\n\n'
		        'Please verify that %s and all of its dependencies have been properly installed\n'
		        'on your system.  See the README file in the distribution for details.\n' % module)
	if msg.find('No module named') >= 0:
		msg += ('Some required files appear to be missing.  Please verify that you have\n'
		        'successfully built all modules.  See the README file for details.\n')
	msg += ('\nIf you are having trouble, ask for help on the forum at\n'
	        '     http://csp.sf.net/forum\n')
	fail(msg)


def findConfig(ini, prefer=None, report=1):
	"""Find and open the cspsim.ini file, searching several standard locations."""
	if prefer is not None and os.path.exists(prefer):
		return prefer
	paths = ['.', '~/.cspsim', '/etc/cspsim']
	searched = []
	for path in paths:
		path = os.path.expanduser(os.path.join(path, ini))
		if os.path.exists(path):
			return os.path.abspath(path)
		searched.append(path)
	if report:
		error('Unable to find configuration file; searched:')
		for path in searched: error(' - ' + path)
	return None


def compileData(xml, dar, rebuild=0, level=0):
	from csp.tools.data.compile import Compiler
	compiler = Compiler(xml, dar, rebuild=rebuild, debug_level=level, warning_level=level)
	warnings, level = compiler.compileAll()
	if warnings > 0 and level > 0:
		print "Run with '--warn=%d' to see all warnings." % level
		print "Please fix all warnings and recompile."
		sys.exit(1)


def setDefaultJoystick():
	"""Provide a default value for the SDL joystick environment variable"""
	if os.name == 'posix' and not os.environ.has_key("SDL_JOYSTICK_DEVICE"):
		# try a reasonable default for linux
		if os.path.exists("/dev/input/js0"):
			os.environ["SDL_JOYSTICK_DEVICE"]="/dev/input/js0"


def ensureDefaultHID(hid_file):
	"""
	Verify that the specified hid input definition exists.  If not, generate it from
	from the example input maps in csp/tools/hid/examples
	"""
	input_path = os.path.join(csp.dir, 'data', 'input')
	if not os.path.exists(input_path): return
	output = os.path.join(input_path, '%s.hid' % hid_file)
	if not os.path.exists(output):
		from csp.tools.hid.compiler import MapCompiler
		tooldir = os.path.join(csp.dir, 'tools', 'hid')
		example = os.path.join(tooldir, 'examples', '%s.map' % hid_file)
		MapCompiler.compile(example, output, include_path=tooldir)
		if os.path.exists(output):
			print 'Default input map %s created; see tools/hid/README for details.' % relativePath(output)
		else:
			print 'Unable to create default input map %s; see tools/hid/README for details.' % relativePath(output)


def createTestObjects(sim, file):
	"""
	Run the specified script to add test objects to the simulation.  At least one
	vehicle must be added.
	"""
	if file:
		if file.endswith('.py'): file = file[:-3]
		try:
			mod = __import__(file, {}, {}, [])
		except ImportError:
			fail('Unable to import test objects file "%s"' % file)
		try:
			mod.create(sim)
		except Exception, e:
			fail('Error executing objects script "%s":\n  %s' % (file, e))


def loadExtensionModules():
	"""
	Load extension modules.  Extension modules will eventually be loaded automatically
	by the sim as part of the theater initialization, but this feature is not yet fully
	implemented.  For now we load demeter and chunklod by hand at startup.
	"""
	if os.name == 'posix':
		modules = [
			'../modules/chunklod/.bin/libchunklod.so',
			'../modules/demeter/.bin/libdemeter.so',
		]
	else:
		modules = [
			r'..\modules\chunklod\.bin\chunklod.dll',
			r'..\modules\demeter\.bin\demeter.dll',
		]
	for module in modules:
		if not csp.csplib.ModuleLoader.load(module):
			fail('Unable to load required extension module "%s"' % module)


def setLogCategory(categories):
	if not categories: return
	categories = categories.split(',')
	mask = 0
	for cat in categories:
		invert = 0
		if cat.startswith('-'):
			cat = class_name[1:]
			invert = 1
		try:
			catmask = getattr(csp.csplib, "cLogCategory_" + cat.upper());
			if invert:
				mask = mask & ~catmask
			else:
				mask = mask | catmask
		except:
			fail("Unrecognized --logcat value: %s" % cat)
	csp.csplib.log().setCategories(mask)


def parseLogPriority(priority):
	if priority is None: return 2
	try:
		logpri = int(priority)
	except ValueError:
		logpri = getattr(csp.csplib, "cLogPriority_" + priority.upper(), None)
	if logpri is None:
		fail('Unknown log priority %s' % priority)
	return logpri


def setLogPriority(priority):
	if priority is not None:
		priority = parseLogPriority(priority)
		if priority is not None:
			csp.csplib.log().setPriority(priority)


def pauseForDebugger():
	"""
	Print a help message for attaching gdb to this process and wait for keyboard
	input.  Should be called after the cspsim module has been loaded if requested.
	"""
	print
	print "All extension modules have been loaded and the simulation is now paused"
	print "to give you an opportunity to attach a debugging session.  Under GNU/Linux"
	print "run the following command from a separate shell to attach a debug session:"
	print
	print "   gdb python %d" % os.getpid()
	print
	print "This will run gdb and attach to the simulation process.  Once gdb finishes"
	print "loading symbols for all the modules, you can set breakpoints as needed"
	print "and then enter 'c' to continue."
	print
	print "Finally, return to this window and press <enter> to resume."
	print
	sys.stdin.readline()


def dumpDataArchive(dar):
	archive = csp.csplib.DataArchive(dar, 1)
	archive.dump()


def main(args):
	if args:
		error('Unrecognized argument(s): ' % ' '.join(args))
		csp.base.app.usage()
		return 1

	options = csp.base.app.options
	config = findConfig('sim.ini', prefer=options.ini)
	if config is None:
		fail('Try setting --ini')

	loadExtensionModules()
	setDefaultJoystick()

	if options.pause:
		pauseForDebugger()

	csp.cspsim.openConfig(config, 1)
	cachepath = csp.cspsim.getCachePath()
	dar = os.path.join(cachepath, "sim.dar")

	if options.dumpdata:
		dumpDataArchive(dar)
		return 0

	sim = csp.cspsim.CSPSim()

	# logging will have already been configured from the ini file at this point,
	# so we can safely override the settings.
	setLogCategory(options.logcat)
	setLogPriority(options.logpri)

	if options.compile or options.rebuild or not os.path.exists(dar):
		level = max(0, 2 - parseLogPriority(options.logpri))
		compileData(options.xml, dar, rebuild=options.rebuild, level=level)
		if options.compile or options.rebuild: return 0

	ensureDefaultHID('aircraft')
	ensureDefaultHID('gamescreen')

	try:
		sim.init()
		createTestObjects(sim, options.objects)
		sim.run()
	finally:
		sim.cleanup()

	print 'Normal exit.'
	return 0


if __name__ == '__main__':
	os.environ.setdefault('CSPLOG_FILE', 'sim.log')
	os.environ.setdefault('CSPLOG_PRIORITY', '2')
	checkModuleSpace()
	importModules()

	import csp.base.app
	csp.base.app.addOption('--ini', metavar='INIFILE', default=None, help='specify the .ini file path')
	csp.base.app.addOption('--xml', metavar='XMLPATH', default='../data/xml', help='specify the xml data path')
	csp.base.app.addOption('-c', '--compile', action='store_true', default=False, help='compile the data archive')
	csp.base.app.addOption('-r', '--rebuild', action='store_true', default=False, help='rebuilde the data archive')
	csp.base.app.addOption('--logpri', metavar='LEVEL', default=None, help='set log priority level (e.g, "info")')
	csp.base.app.addOption('--logcat', metavar='CATLIST', default=None, help='set log category filter (e.g., "object,physics")')
	csp.base.app.addOption('--pause', action='store_true', default=False, help='pause on startup for attaching a debugger')
	csp.base.app.addOption('--dumpdata', action='store_true', default=False, help='dump the contents of the data archive')
	csp.base.app.addOption('--objects', metavar='PATH', default='test_objects.py', help='python script to place test objects')
	csp.base.app.start()
