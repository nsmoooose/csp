#!/usr/bin/python

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

##
# A simple script to regenerate the default input maps

import os, os.path, sys
from ConfigParser import ConfigParser

norm = os.path.normpath

import CSPSim
ini = CSPSim.findConfig()

config = ConfigParser()
try:
	config.read(ini)
	config_path = norm(config.get("Paths", "DataPath"))
except:
	print >>sys.stderr, "Unable to find DataPath in '%s', aborting" % config
	sys.exit(1)

cspinput = norm("../Tools/HID/cspinput")
if not os.path.exists(cspinput):
	print >>sys.stderr, "Can't find '%s', aborting." % cspinput
	sys.exit(1)

input_path = os.path.join(config_path, "Input")
if not os.path.exists(input_path):
	print >>sys.stderr, "'%s' directory not found, aborting." % input_path
	sys.exit(1)

cwd = os.getcwd()
os.chdir(norm("../Tools/HID"))

maps = ("gamescreen", "aircraft")
for map in maps:
	source = os.path.join("Examples", "%s.map" % map)
	target = os.path.join(input_path, "%s.hid" % map)
	os.system("python cspinput %s %s" % (source, target))
	print "Default input map created for %s.map; see Tools/HID/README for details." % map

os.chdir(cwd)

