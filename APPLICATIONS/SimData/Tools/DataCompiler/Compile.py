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

if __name__ == "__main__":
	if hasattr(sys, "setdlopenflags"):
		sys.setdlopenflags(0x101)
	#sys.path.append("../../Source");
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
			import CSP
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


class Compiler:

	def compileAll(self):
		print
		print "Compiling '%s' from '%s'" % (self.outfile, self.infile)
		print "Opening output archive"
		compiled = DataArchive(self.outfile, 0)
		print "Opening input archive"
		master = ObjectXMLArchive(self.infile);
		print "Loading all objects"
		resetWarnings()
		all = master.loadAll()
		warnings, level = getWarnings()
		print "XML parse completed."
		if warnings > 0:
			print "%d warnings (severity=%d)." % (warnings, level)
			if level > 0: 
				print "Please fix these warnings and recompile."
				print "Compiled data archive NOT written!"
				sys.exit(1)
		paths = master.getPaths()
		print "Compiling all objects"
		for id in all.keys():
			object = all[id]
			DEBUG(2, "Compiling object '%s' [%s] %s" % (id, hash_string(id), str(object)))
			compiled.addObject(object, id)
		self.dumpBadPaths(all, paths)

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
			
	def usage(self, msg=None):
		print
		if msg is not None:
			print "Usage error:", msg
		print "SimData XML Compiler"
		print self._name, "[--warn=level] [--debug=level] sourcepath output"
		sys.exit(1)
		
	def parse(self, args):
		args = args[1:]
		for arg in args:
			if arg.startswith('--'):
				try:
					if arg.startswith('--warn='):
						setWarningLevel(int(arg[7:]))
					elif arg.startswith('--debug='):
						setDebugLevel(int(arg[8:]))
					else:
						self.usage("unknown option '%s'" % arg)
				except:
						self.usage("invalid option '%s'" % arg)
			else:
				if self.infile is None:
					self.infile = arg
				elif self.outfile is None:
					self.outfile = arg
				else:
					self.usage("invalid argument '%s'" % arg)
		if self.infile is None or self.outfile is None:
			self.usage()
					
	def __init__(self, args):
		setWarningLevel(1)
		self.infile = None
		self.outfile = None
		self._name = args[0]
		self.parse(args)
		self.compileAll()

if __name__ == "__main__":
	import profile
	profile.run('Compiler(sys.argv)', 'profile.out')

