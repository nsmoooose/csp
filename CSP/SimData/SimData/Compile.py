#!/usr/bin/python

# SimDataCSP: Data Infrastructure for Simulations
# Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
# 
# This file is part of SimDataCSP.
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

import SimData

if __name__ == "__main__":
	if hasattr(sys, "setdlopenflags"):
		sys.setdlopenflags(0x101)
	try:
		import SimData
	except:
		print "Unable to find the SimData python module.  This file"
		print "should be located in ../../Source.  Make sure you"
		print "have compiled SimData before using this program.  See"
		print "the README file in the main SimData directory for"
		print "more information or ask for help on the forums at"
		print "http://csp.sourcforge.net/forum"
		sys.exit(1)
	SimData.log().setLogLevels(SimData.LOG_ALL, SimData.LOG_ALERT)
	original_path = sys.path[:]
	new_path = ""
	try:
		f = open(".csp_path", "rt");
		path = f.readline().strip()
		sys.path.append(path)
	except:
		pass
	while 1:
		try:
			import cCSP
			break
		except:
			print
			print "Unable to find the CSP python module.  Assuming you"
			print "have compiled a recent version of CSPSim, please"
			print "provide a path (relative or absolute) to the CSP.py"
			print "module.  This file is usually located in the Source"
			print "directory of CSPSim."
			print "==> ",
			new_path = sys.stdin.readline().strip()
			print
			if not new_path:
				sys.exit(1)
			sys.path = original_path[:]
			sys.path.append(new_path) 
	if new_path:
		f = open(".csp_path", "wt")
		print >>f, new_path


from Parse import ObjectXMLArchive, XMLSyntax
from SimData import DataArchive, hash_string

from Debug import *

import os.path, time

class Cache:
	def __init__(self, fn):
		self.timestamp = None
		self.source = None
		if os.path.exists(fn):
			try:
			    # open archive for reading, but disable chaining
				self.source = DataArchive(fn, 1, 0)
				self.timestamp = os.path.getmtime(fn)
			except:
				self.source = None
				self.timestamp = None

	def close(self):
		self.source = None

	def getObject(self, id):
		if self.source is not None:
			try:
				return self.source.getObject(id)
			except:
				pass
		return None

	def isNewer(self, fn):
		if self.timestamp is None: 
			return 1
		try:
			mtime = os.path.getmtime(fn)
		except:
			return 1
		return mtime > self.timestamp


class CompilerUsageError:
	def __init__(self, msg=None):
		self.msg = msg
	def getMessage(self):
		if self.msg is None: 
			return ""
		return "Usage error: %s\n" % self.msg
	def __repr__(self):
		return self.getMessage()
		
		
class Compiler:

	def compileAll(self):
		print
		print "Compiling '%s' from '%s'" % (self.outfile, self.infile)
		DEBUG(1, "Opening input archive")
		prefix = os.path.splitext(os.path.basename(self.outfile))[0]
		master = ObjectXMLArchive(prefix, self.infile);
		DEBUG(1, "Loading all objects")
		resetWarnings()
		if not self.rebuild:
			cache = Cache(self.outfile)
			master.setCache(cache)
		all = master.loadAll()
		if not self.rebuild:
			cache.close()
		warnings, level = getWarnings()
		DEBUG(1, "XML parse completed.")
		if warnings > 0:
			print "%d warnings (severity=%d)." % (warnings, level)
			if level > 0: 
				if level > getWarningLevel():
					print "Run with '--warn=%d' to see all warnings." % level
				if not self.force:
					print "Please fix these warnings and recompile."
					print "Compiled data archive NOT written!"
					sys.exit(1)
		paths = master.getPaths()
		DEBUG(1,"Compiling all objects")
		compiled = DataArchive(self.outfile, 0)
		for id in all.keys():
			object = all[id]
			DEBUG(2, "Compiling object '%s' [%s] %s" % (id, hash_string(id), str(object)))
			# objects cached from the original archive are Pointers, so we need to
			# dereference them.  this should be cleaned up a bit.
			try:
				object = object.__get__()
			except:
				pass
			compiled.addObject(object, id)
		self.dumpBadPaths(all, paths)
		print "Finished."

	def dumpBadPaths(self, all, paths):
		if getWarningLevel() < 1: return
		badpaths = filter(lambda x, a=all: not a.has_key(x), paths.keys())
		if len(badpaths) > 0 and getWarningLevel() >= 1:
			print
			print "Found %d broken path(s):" % len(badpaths)
			idx = 0
			for path in badpaths:
				idx = idx + 1
				print "%03d: Path '%s'" % (idx, path)
				objects = {}
				for obj in paths[path]: objects[obj] = 1
				print "   : defined in '" + "'\n    defined in '".join(objects.keys()) + "'"
			print
			
	def printUsage(self, out=sys.stdout):
		print >>out, "SimData XML Compiler"
		if self.standalone:
			print >>out, self._name, "[--warn=level] [--debug=level] [--rebuild] sourcepath output"
		else:
			print >>out, ""
			print >>out, "  Options:"
			print >>out, "              --warn=level     show warning message (< level)"
			print >>out, "              --debug=level    show debug messages (< level)"
			print >>out, "              --rebuild        rebuild entire archive"
			print >>out, "              --force          ignore warnings"
			print >>out, "              --help           help message"

	def usage(self, msg=None):
		if self.standalone:
			if msg is not None:
				print "Usage error:", msg
			self.printUsage()
			sys.exit(1)
		else:
			raise CompilerUsageError(msg)
			
	def parse(self, args):
		self._name = args[0]
		args = args[1:]
		for arg in args:
			if arg.startswith('-'):
				if arg.startswith('--warn='):
					try:
						level = int(arg[7:])
					except:
						self.usage("invalid option '%s'" % arg)
					setWarningLevel(level)
					if level > 0:
						SimData.log().setLogLevels(SimData.LOG_ALL, SimData.LOG_WARNING)
				elif arg.startswith('--debug='):
					try:
						level = int(arg[8:])
					except:
						self.usage("invalid option '%s'" % arg)
					setDebugLevel(level)
					if level > 1:
						SimData.log().setLogLevels(SimData.LOG_ALL, SimData.LOG_TRACE)
					elif level > 0:
						SimData.log().setLogLevels(SimData.LOG_ALL, SimData.LOG_DEBUG)
				elif arg == '--rebuild':
					self.rebuild = 1
				elif arg == '--force':
					self.force = 1
				elif arg in ("--help", "-h", "-help"):
					self.usage()
				else:
					self.usage("unknown option '%s'" % arg)
			else:
				if self.infile is None:
					self.infile = arg
				elif self.outfile is None:
					self.outfile = arg
				else:
					self.usage("invalid argument '%s'" % arg)
		if self.infile is None or self.outfile is None:
			self.usage("no input path or output archive specified")
					
	def __init__(self, standalone=0):
		setWarningLevel(1)
		self.rebuild = 0
		self.force = 0
		self.infile = None
		self.outfile = None
		self._name = ""
		self.standalone = standalone

if __name__ == "__main__":
	c = Compiler(1)
	c.parse(sys.argv)
	c.compileAll()

