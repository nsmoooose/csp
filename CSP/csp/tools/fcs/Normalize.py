#!/usr/bin/env python

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

"""
A tool for comparing FlightControlSystem XML definitions.  Useful
for determining differences between hand-edited xml files and ones
generate from NodeConstructor definitions.

Suggested usage:

Normalize.py in1.xml > out1.txt
Normalize.py in2.xml > out2.txt
diff out1.txt out2.txt
"""

import sys

record = 0
object = []
id = ''
objects = {}
comment = 0

# skip the header
for i in range(3): sys.stdin.readline()

# extract the node definitions to the 'object' dictionary as
# { node_id : [line1, ...], ... }
for line in sys.stdin:
	left = line.strip()
	if line.find('<!--') >= 0:
		assert not comment
		comment = 1
	if line.find('-->') >= 0:
		assert comment
		comment = 0
		continue
	if comment: continue
	if left.startswith("<Object "):
		assert not record
		record = 1
	if record:
		object.append(line)
		if left.startswith('<Key name="id"'): id = left
	if left.startswith("</Object>") and record:
		record = 0
		assert id and not objects.has_key(id)
		objects[id] = object
		object = []
		id = ''

# print out the node definitions in alphabetical order
keys = objects.keys()
keys.sort()
for key in keys:
	print ''.join(objects[key])
