#!/usr/bin/python

# Combat Simulator Project - CSPSim
# Copyright (C) 2002, 2004 The Combat Simulator Project
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


import sys
import os
import os.path

try:
	import CSP
except ImportError:
	print 'Unable to import CSP bootstrap module.  Check that you have run'
	print 'CSP/setup.py successfully to initialize the workspace.'
	sys.exit(1)


#import Shell
#from SimData.Compile import Compiler, CompilerUsageError


#def initDynamicLoading():
#	"""Enable lazy loading of shared library modules if available"""
#	if os.name == 'posix':
#		import dl
#		sys.setdlopenflags(dl.RTLD_GLOBAL|dl.RTLD_LAZY)


def setDefaultJoystick():
	"""Provide a default value for the SDL joystick environment variable"""
	if not os.environ.has_key("SDL_JOYSTICK_DEVICE"):
		# try a reasonable default for linux
		if os.path.exists("/dev/input/js0"):
			os.environ["SDL_JOYSTICK_DEVICE"]="/dev/input/js0"


def printUsage():
	print "Combat Simulator Project - CSPSim"
	print
	print "  Primary options:"
	print "    --compile-data       run the data compiler"
	print "    --config=path        path to config (.ini) file"
	print "    --logcat=categories  set the logging categories (colon-delimited)"
	print "                         examples: --logcat=APP:TERRAIN:PHYSICS"
	print "                                   --logcat=ALL:-NETWORK  (all but network)"
	print "    --logpri=priority    set the logging priority threshold (overrides .ini)"
	print "                         examples: --logpri=INFO"
	print "                                   --logpri=5"
	print "    --pause              pause on startup for attaching a debug session"
	print "    --dump-data          show the contents of sim.dar"
	print "    --client-node        run networking test client node"
	print "    --echo-server-node   run networking test echo server node"
	print "    --help               help message"


def setLogCategory():
	if len(log_categories) == 0: return
	flags = 0
	for class_name in log_categories:
		invert = 0
		if class_name.startswith('-'):
			class_name = class_name[1:]
			invert = 1
		try:
			class_flag = getattr(cCSP, "CSP_"+class_name);
			if invert:
				flags = flags & ~class_flag
			else:
				flags = flags | class_flag
		except:
			print "Unrecognized log class:", class_name
	cCSP.csplog().setLogCategory(flags)


def setLogPriority():
	cCSP.csplog().setTimeLogging(1)
	if log_priority is None: return
	cCSP.csplog().setLogPriority(log_priority)
	SimData.log().setLogPriority(log_priority)


def parseLogPriority(priority):
	try:
		priority = int(priority)
	except ValueError:
		priority = getattr(SimData, "LOG_" + priority.upper(), None)
	return priority


def runCSPSim(args):
	if len(args):
		print "Unrecognized option '%s'\n" % args[0]
		printUsage()
		print
		sys.exit(1)

	cachepath = cCSP.getCachePath()
	dar = os.path.join(cachepath, "sim.dar")
	if not os.path.exists(dar):
		print
		print "Static data archive '%s' not found." % dar
		compileData([])

	import Shell
	app = cCSP.CSPSim()

	# logging will have already been configured from the ini file at this point,
	# so we can safely override the settings.
	setLogCategory()
	setLogPriority()

	app.init()

	try:
		import TestObjects
		TestObjects.create(app)
	except ImportError:
		pass
	except Exception, e:
		print "Error in TestObjects.py:", e

	app.setShell(Shell.Shell())
	app.run()
	app.cleanup()
	print "CSPSim normal exit."


def dumpData(args):
	cachepath = cCSP.getCachePath()
	dar = os.path.join(cachepath, "sim.dar")
	for arg in args:
		if arg.startswith('--dump-data='):
			dar = arg[12:]
	archive = SimData.DataArchive(dar, 1)
	archive.dump()


def compileData(args):
	datapath = cCSP.getDataPath()
	cachepath = cCSP.getCachePath()
	dar = os.path.join(cachepath, "sim.dar")
	XML = os.path.join(datapath, "XML")
	cCSP.csplog().setLogCategory(cCSP.CSP_ALL)
	cCSP.csplog().setLogPriority(SimData.LOG_ALERT)
	#print "compile %s %s" % (XML, dar)
	try:
		from CSP.SimData.Compile import Compiler, CompilerUsageError
	except:
		print
		print "ERROR: unable to load the SimData data compiler module."
		print
		print "Please verify the SimData has been installed correctly in your Python"
		print "site-local directory.  See the SimData README file for instructions."
		print
		sys.exit(1)
	compiler = Compiler()
	try:
		compiler.parse([""] + args + [XML, dar])
	except CompilerUsageError, e:
		if e.getMessage():
			print e
		printUsage()
		print
		compiler.printUsage()
		print
		sys.exit(1)
	try:
		compiler.compileAll()
	except RuntimeError, e:
		print 'Aborting'
		sys.exit(1)
	print


def runClientNode(args):
	print "CSPSim.py - runClientNode - Starting Test Client Node..."
	print "CSPSim.py - runClientNode - calling loadCSP"
	loadCSP()
	print "CSPSim.py - runClientNode - calling CSP.ClientNode"
	app = cCSP.ClientNode()
	print "CSPSim.py - runClientNode - calling app.run"
	app.run()


def runEchoServerNode(args):
	print "Starting Test Echo Server Node..."
	loadCSP()
	app = cCSP.EchoServerNode()
	app.run()


def loadSimData():
	"""Load the SimData module"""
	global SimData
	os.environ.setdefault('SIMDATA_LOGFILE', 'SimData.log')
	try:
		import CSP.SimData as SD
	except Exception, e:
		msg = str(e)
		if len(msg) > 60:
			msg = msg.replace(": ", ":\n  ")
		print """
	ERROR: Unable to import SimData.

	%s
	""" % msg,

		if str(e).find("symbol") >= 0:
			print """
	Unresolved symbols often indicate missing libraries or improper link options.
	""",
		print """
	Please verify that SimData has been properly installed on your system.  See
	the README file in the SimData distribution for details.
	"""
		sys.exit(1)
	SimData = SD


def loadCSP():
	global cCSP
	try:
		import CSP.CSPSim.cCSP
	except Exception, e:
		msg = str(e)
		if len(msg) > 60:
			msg = msg.replace(": ", ":\n  ")
		print """
	ERROR: Unable to import cCSP.py

	%s
	""" % msg,

		if str(e).find("No module named") >= 0:
			print """
	Some required files appear to be missing.  Please verify that you have
	successfully built CSPSim.  See the README for details.  If you are
	still having trouble, ask for help on the forums at

		http://csp.sourceforge.net/forum

	"""
		else:
			print """
	See the README files for additional information.  If you are still having
	trouble, ask for help on the forums at

		http://csp.sourceforge.net/forum

	"""
		sys.exit(1)
	cCSP = CSP.CSPSim.cCSP


def findConfig():
	# do our best to find the correct configuration file
	config_paths = [".", "~/.cspsim", "/etc/cspsim", "../Data"]

	config = "CSPSim.ini"
	for path in config_paths:
		path = os.path.join(path, "CSPSim.ini")
		path = os.path.expanduser(path)
		if os.path.exists(path):
			config = path
			break
	return config


def main(argv):
	global log_categories
	global log_priority

	#initDynamicLoading()
	setDefaultJoystick()

	action = None

	# process command line options
	program = argv[0]
	all_args = argv[1:]
	log_categories = []
	log_priority = None
	log_priority_arg = None
	other_args = []
	pause = 0

	config = findConfig()

	for arg in all_args:
		if arg == '--compile-data':
			action = compileData
		elif arg == '--dump-data':
			action = dumpData
		elif arg.startswith('--dump-data='):
			action = dumpData
			other_args.append(arg)
		elif arg == '--client-node':
			action = runClientNode
		elif arg == '--echo-server-node':
			action = runEchoServerNode
		elif arg == '--pause':
			pause = 1
		elif arg in ("--help", "-h", "-help"):
			if action == None:
				print
				printUsage()
				print
				sys.exit(1)
			else:
				other_args.append(arg)
		elif arg.startswith("--config="):
			config = arg[9:]
		elif arg.startswith("--logcat="):
			log_categories.extend(arg[9:].split(':'))
		elif arg.startswith("--logpri="):
			log_priority_arg = arg[9:]
		else:
			other_args.append(arg)

	if action is None:
		action = runCSPSim


	loadSimData()

	if log_priority_arg is not None:
		log_priority = parseLogPriority(log_priority_arg)
		if log_priority is None:
			print "Invalid logging priority, using .ini setting."

    # capture Object class registration and other errors when CSP loads
	SimData.log().setLogCategory(SimData.LOG_ALL)
	SimData.log().setLogPriority(SimData.LOG_DEBUG)

	loadCSP()

	print "Loading configuration from '%s'." % config
	if not cCSP.openConfig(config):
		print "Unable to open primary configuration file (%s)" % config
		sys.exit(0)

	if pause:
		print
		print "CSPSim has loaded all extension modules, and is now paused to give you"
		print "an opportunity to attach a debugging session.  Under GNU/Linux run the"
		print "following command from a separate shell to attach a debug session:"
		print
		print "   gdb python %d" % os.getpid()
		print
		print "This will run gdb and attach to the CSPSim process.  Once gdb finishes"
		print "loading symbols for all the modules, you can set breakpoints as needed"
		print "and then enter 'c' to continue."
		print
		print "Finally, return to this window and press <enter> to resume CSPSim."
		print
		try:
			sys.stdin.readline()
		except KeyboardInterrupt:
			sys.exit(1)

	action(other_args)


if __name__ == "__main__":
	main(sys.argv)

