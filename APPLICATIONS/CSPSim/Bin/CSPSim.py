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

# enable lazy loading of shared library modules if available
if os.name == 'posix':
	import dl
	sys.setdlopenflags(dl.RTLD_GLOBAL|dl.RTLD_LAZY)

#if hasattr(sys, "setdlopenflags"):
#	sys.setdlopenflags(0x101)
	
# is the SDL joystick environment variable isn't set, try a standard value
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
	datapath = CSP.getDataPath()
	dar = os.path.join(datapath, "sim.dar")
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


def compileData(args):
	datapath = CSP.getDataPath()
	dar = os.path.join(datapath, "sim.dar")
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


action = None
config = "../Data/CSPSim.ini"
if os.path.exists("CSPSim.ini") or not os.path.exists(config):
	config = "CSPSim.ini"



# process command line options
program = sys.argv[0]
all_args = sys.argv[1:]
log_classes = []
other_args = []

for arg in all_args:
	if arg == '--compile-data':
		action = compileData
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
	else:
		other_args.append(arg)

if action is None:
	action = runCSPSim

# load simdata
try:
	import SimData
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

SimData.log().setOutput("SimData.log")
SimData.log().setLogLevels(SimData.LOG_ALL, SimData.LOG_DEBUG)
try:
	import cCSP as CSP
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

SimData.log().setLogLevels(SimData.LOG_ALL, SimData.LOG_ALERT)

if not CSP.openConfig(config):
	print "Unable to open primary configuration file (%s)" % config
	sys.exit(0)

action(other_args)


