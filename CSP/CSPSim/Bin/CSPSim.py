#!/usr/bin/python -O

# Combat Simulator Project - CSPSim
# Copyright (C) 2002 The Combat Simulator Project
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


import sys, os, os.path

#import Shell
#from SimData.Compile import Compiler, CompilerUsageError

def initDynamicLoading():
	"""Enable lazy loading of shared library modules if available"""
	if os.name == 'posix':
		import dl
		sys.setdlopenflags(dl.RTLD_GLOBAL|dl.RTLD_LAZY)

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
	print "              --compile-data  run the data compiler"
	print "              --config=path   path to config (.ini) file"
	print "              --log=classes   set the logging classes"
	print "              --slog=level    set the simdata logging level"
	print "              --dump-data     show the contents of sim.dar"
	print "              --help          help message"


def setLogClasses():
	if len(log_classes) == 0: return
	flags = 0
	for class_name in log_classes:
		try:
			class_flag = getattr(CSP, "CSP_"+class_name);
			flags = flags | class_flag
		except:
			print "Unrecognized log class:", class_name
	CSP.csplog().setLogClasses(flags)


def runCSPSim(args):
	if len(args):
		print "Unrecognized option '%s'" % args[0]
		print
		printUsage()
		print
		sys.exit(1)
	cachepath = CSP.getCachePath()
	dar = os.path.join(cachepath, "sim.dar")
	if not os.path.exists(dar):
		print
		print "Static data archive '%s' not found." % dar
		compileData([])
	import Shell
	app = CSP.CSPSim()
	setLogClasses()
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
	cachepath = CSP.getCachePath()
	dar = os.path.join(cachepath, "sim.dar")
	for arg in args:
		if arg.startswith('--dump-data='):
			dar = arg[12:]
	archive = SimData.DataArchive(dar, 1)
	archive.dump()
	archive.getObject("sim:terrain.balkan")
	archive.getObject("sim:theater.balkan")
	

def compileData(args):
	datapath = CSP.getDataPath()
	cachepath = CSP.getCachePath()
	dar = os.path.join(cachepath, "sim.dar")
	XML = os.path.join(datapath, "XML")
	CSP.csplog().setLogLevels(CSP.CSP_ALL, SimData.LOG_ALERT)
	#print "compile %s %s" % (XML, dar)
	try:
		from SimData.Compile import Compiler, CompilerUsageError
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
	compiler.compileAll()
	print


def loadSimData():
	"""Load the SimData module"""
	global SimData
	try:
		import SimData as SD
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
	global CSP
	try:
		import cCSP
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
	CSP = cCSP


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
	global log_classes

	initDynamicLoading()
	setDefaultJoystick()

	action = None

	# process command line options
	program = argv[0]
	all_args = argv[1:]
	log_classes = []
	other_args = []
	pause = 0
	simdata_loglevel = "ALERT"

	config = findConfig()

	for arg in all_args:
		if arg == '--compile-data':
			action = compileData
		elif arg == '--dump-data':
			action = dumpData
		elif arg.startswith('--dump-data='):
			action = dumpData
			other_args.append(arg)
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
		elif arg.startswith("--log="):
			log_classes.extend(arg[6:].split(':'))
		elif arg.startswith("--slog="):
			simdata_loglevel = arg[7:]
		else:
			other_args.append(arg)

	if action is None:
		action = runCSPSim

	loadSimData()
	SimData.log().setOutput("SimData.log")
	SimData.log().setLogLevels(SimData.LOG_ALL, SimData.LOG_DEBUG)

	loadCSP()

	try:
		simdata_loglevel = eval("SimData.LOG_%s" % simdata_loglevel.upper())
	except:
		print "Invalid SimData logging level, defaulting to 'ALERT'"
		simdata_loglevel = SimData.LOG_ALERT

	SimData.log().setLogLevels(SimData.LOG_ALL, simdata_loglevel)

	print "Loading configuration from '%s'." % config
	if not CSP.openConfig(config):
		print "Unable to open primary configuration file (%s)" % config
		sys.exit(0)

	if pause:
		print "Hit <ctrl-break> to temporarily exit and set breakpoints."
		print "When you are done, continue execution and hit <enter>."
		sys.stdin.readline()

	action(other_args)


if __name__ == "__main__":
	main(sys.argv)

