# Copyright 2004-2005 Mark Rose <mkrose@users.sourceforge.net>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

"""
This module provides optional access to the csp::Object classes
registered by cspsim.  To load successfully, cspsim must have been
built in the current workspace.  If the cspsim module cannot be
loaded, none of the functions defined in this module will be
available.
"""

import os
import os.path

# Users of this module should not rely on it loading successfully,
# since it requires SimData and the CSPSim shared library as well
# as a functioning CSP Python module space.  This, always test
# ObjectInterface.ok before use.  If ok is false, functionality
# should degrade as gracefully as possible (e.g. this should never
# be a fatal error).
ok = 1

# Redirect csp module logging to a file if it isn't already.
os.environ.setdefault('CSPLOG_FILE', 'layout.log')

# Try to import csplib and cspsim.  On failure this module is disabled.
try:
	print 'Attempting to import CSP modules'
	import csp.csplib
	import csp.cspsim
except ImportError, e:
	print 'Error importing CSP modules', e
	ok = 0

if ok:
	import cLoader
	import Object

	#csp.csplib.log().setCategories(csp.csplib.Cat_ALL)
	#csp.csplib.log().setPriority(csp.csplib.Prio_ERROR)

	# Load the CSP interface registry.
	reg = csp.csplib.InterfaceRegistry.getInterfaceRegistry()
	interface_names = reg.getInterfaceNames()
	print 'CSP interface registry loaded (%d object classes)' % len(interface_names)

	# FIXME Setup the data load path for ObjectModel.  This should really be read
	# from the CSPSim.ini file.
	'''
	ini = csp.cspsim.findConfig()
	print 'Loading CSPSim.ini from %s' % ini
	from ConfigParser import ConfigParser
	config_parser = ConfigParser()
	config_parser.readfp(open(ini))
	data_path = config_parser.get('Paths', 'DataPath')
	if not os.path.isabs(data_path):
		#data_path = os.path.join(os.path.dirname(CSP.CSPSim.CSPSim.__file__), data_path)
		data_path = os.path.join(os.path.dirname(CSP.CSPSim.CSPSim.__file__), data_path)
	pathlist = os.pathsep.join([os.path.abspath(os.path.join(data_path, subdir)) for subdir in ('Images', 'Models', 'Fonts')])
	print pathlist
	'''

	datapath = os.path.join(csp.dir, 'data')
	datadirs = ('images', 'models', 'fonts', 'sounds')
	pathlist = os.pathsep.join([os.path.join(datapath, subdir) for subdir in datadirs])
	cLoader.setPathList(pathlist)

	# Create an OSG subgraph for the specified FeatureModel domtree node.
	def MakeFeatureModel(node):
		object = Object.MakeObject(node)
		return cLoader.getFeatureModel(object)

	# Create an OSG subgraph for the specified ObjectModel domtree node.  Uses the
	# full ObjectModel loader to create the subgraph.
	def MakeObjectModel(node):
		object = Object.MakeObject(node)
		return cLoader.getObjectModel(object)

