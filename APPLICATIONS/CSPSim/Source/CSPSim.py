#!/usr/bin/python -O

import sys, os.path

#import Shell
#from SimData.Compile import Compiler, CompilerUsageError

# enable lazy loading of shared library modules if available
if hasattr(sys, "setdlopenflags"):
	sys.setdlopenflags(0x101)
	

def printUsage():
	print "Combat Simulator Project - CSPSim"
	print
	print "  Primary options:"
	print "              --compile-data  run the data compiler"
	print "              --config=path   path to config (.ini) file"
	print "              --help          help message"


def runCSPSim(args):
	if len(args):
		print "Unrecognized option '%s'" % args[0]
		print
		printUsage()
		print
		sys.exit(1)
	datapath = CSP.getDataPath()
	dar = os.path.join(datapath, "Sim.dar")
	if not os.path.exists(dar):
		print
		print "Static data archive '%s' not found." % dar
		compileData([])
	import Shell
	app = CSP.CSPSim()
	app.init()
	app.setShell(Shell.Shell())
	app.run()
	app.cleanup()
	print "CSPSim normal exit."


def compileData(args):
	datapath = CSP.getDataPath()
	dar = os.path.join(datapath, "Sim.dar")
	XML = os.path.join(datapath, "XML")
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

SimData.log().set_output("SimData.log")
SimData.log().setLogLevels(SimData.LOG_ALL, SimData.LOG_DEBUG)
import CSP
SimData.log().setLogLevels(SimData.LOG_ALL, SimData.LOG_ALERT)

if not CSP.openConfig(config):
	print "Unable to open primary configuration file (%s)" % config
	sys.exit(0)

action(other_args)

