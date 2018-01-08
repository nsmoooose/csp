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

from . import Object
from csp.tools.layout2 import layout_module

def MakeFeatureModel(node):
	"""Create an OSG subgraph for the specified FeatureModel domtree node."""
	object = Object.MakeObject(node)
	return layout_module.getFeatureModel(object)

def MakeObjectModel(node):
	"""Create an OSG subgraph for the specified ObjectModel domtree node. Uses the
	full ObjectModel loader to create the subgraph."""
	object = Object.MakeObject(node)
	return layout_module.getObjectModel(object)

