# This is a very simplistic setup script for windows.  It should be
# run before building the MSVC project.  Here's what it does:
#
#   checks the python version
#
#   checks that simdata is installed and the version is compatible
#
#   copies _cSimData.lib from the simdata package to the third-party
#   libs directory.


import sys

min_python_version = "2.2.0"
min_simdata_version = "0.4.0"

if len(sys.argv) == 2:
	command = sys.argv[1]
	if command == "check_version":
		old = sys.version[:5] < min_python_version
		if old:
			print
			print "CSPSim requires Python version %s or higher. (current version is %s)" % \
	                    (min_python_version, sys.version[:5])
			print
		sys.exit(old)
		
try:
	from CSP import SimData
	if SimData.getVersion() < min_simdata_version:
		print
		print "SimData version %s or greater is required, but version %s is installed." % (min_simdata_version, SimData.getVersion())
		print "SimData is available from CVS at http://sourceforge.net/projects/csp"
		print
		sys.exit(1)
except:
	print
	print "Unable to load SimData; please check that it is properly installed."
	print "SimData is available from CVS at http://sourceforge.net/projects/csp"
	print
	sys.exit(1)


from distutils import sysconfig, dir_util
from distutils.file_util import copy_file
from distutils.errors import DistutilsFileError, DistutilsInternalError

#from distutils.dir_util import mkpath
import os, os.path, string

join = os.path.join
dst = join(join(join("..", ".."), "THIRDPARTYLIBS"), "lib")

if not os.path.isdir(dst):
	print
	print "Path '%s' not found.  This path is the default path for third-party libraries" % dst
	print "needed by CSPSim.  You must create this directory before proceeding."
	print
	sys.exit(1)

lib = sysconfig.get_python_lib()
src = join(join(lib, "SimData"), "_cSimData.lib")
dst = join(lib, "_cSimData.lib")
print "%s => %s" % (src, dst)
try:
	copy_file(src, dst)
except:
	print
	print "ERROR: unable to copy file."
	print
	sys.exit(1)
sys.exit(0)

