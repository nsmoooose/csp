#!/usr/bin/python

# Combat Simulator Project
# Copyright (C) 2002-2005 Mark Rose <mkrose@users.sf.net>
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
import time

if __name__ == "__main__":
	print 'Do not run compile.py directly.'
	sys.exit(1)

from csp import csplib
from csp.tools.data.parse import ObjectXMLArchive, XMLSyntax
from csp.tools.data.debug import *


class Cache:
	def __init__(self, fn):
		self.timestamp = None
		self.source = None
		if os.path.exists(fn):
			try:
				# open archive for reading, but disable chaining
				self.source = csplib.DataArchive(fn, 1, 0)
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
			except csplib.libexc, e:
				e.clear()
				DEBUG(2, "Exception reading cache for object %s:\n%s" % (id, e))
		return None

	def isNewer(self, fn):
		if self.timestamp is None:
			return 1
		try:
			mtime = os.path.getmtime(fn)
		except:
			return 1
		return mtime > self.timestamp


class Compiler:
	def compileAll(self):
		DEBUG(0, "Compiling '%s' from '%s'" % (self.outfile, self.infile))
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
			DEBUG(0, "%d warnings (severity=%d)." % (warnings, level))
			if level > 0 and not self.force:
				DEBUG(0, "Compiled data archive NOT written!")
				return warnings, level
		paths = master.getPaths()
		DEBUG(1,"Compiling all objects")
		compiled = csplib.DataArchive(self.outfile, 0)
		hash_string = csplib.hash_string
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
		DEBUG(0, "Finished.")
		return warnings, level

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

	def __init__(self, infile, outfile, rebuild=0, force=0, debug_level=0, warning_level=1):
		setWarningLevel(warning_level)
		setDebugLevel(debug_level)
		self.infile = infile
		self.outfile = outfile
		self.rebuild = rebuild
		self.force = force

