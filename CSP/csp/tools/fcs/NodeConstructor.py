# Copyright 2005 The Combat Simulator Project
# http://csp.sourceforge.net
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

import sys
import time

from math import radians, degrees

RadiansToDegrees = degrees(1.0)
DegreesToRadians = radians(1.0)


def _simple(type, key, val):
	print '\t\t\t<%s name="%s">%s</%s>' % (type, key, val, type)


def BeginNetwork(g):
	global network
	network = g


class BaseType:
	def out(self, key):
		pass


class Node:
	def _out(self):
		c = self.__class__
		if Node in c.__bases__ or c == Node:
			return
		else:
			try:
				opname = c._node
			except:
				opname = "%s" % c.__bases__[0].__name__
			print
			if self.__class__.__doc__:
				print '\t\t<!-- %s -->' % self.__class__.__doc__
			print '\t\t<Object class="%s">' % opname
			print '\t\t\t<Key name="id">%s</Key>' % c.__name__
		opts = self._parse()
		opts = self._options(opts)
		self._defaults(opts)
		print '\t\t</Object>'
		if OutputChannel in c.__bases__:
			OutputChannel.__ALL__.append(c)

	def _parse(self):
		d = self.__class__.__dict__
		return dict(filter(lambda x: x[0][0]!='_', d.iteritems()))

	def _defaults(self, d):
		keys = d.keys()
		keys.sort()
		for key in keys:
			val = d[key]
			if key.startswith("input"):
				if isinstance(val, _forward):
					val = val()
				elif isinstance(val, str):
					name = val
					val = network.get(name)
					if val is None:
						error("In module '%s', node '%s' references undefined input node '%s'" % (self.__class__.__module__, self.__class__.__name__, name))
				try:
					_simple("Key", key, val.__name__)
				except:
					print val
					raise
				self.__class__.__dict__.setdefault('__INPUTS__', []).append(val)
			elif key.startswith("channel"):
				_simple("String", key, val)
			elif isinstance(val, BaseType):
				val.out(key)
			elif isinstance(val, str):
				_simple("Key", key, val)
			else:
				_simple("Float", key, val)

	def _options(self, d):
		return d



class Adder(Node): pass
class Adder3(Node): pass
class LagFilter(Node): pass
class LeadLagFilter(Node): pass
class LeadFilter(Node): pass
class InputScalarChannel(Node): pass
class InputVectorXChannel(Node): pass
class InputVectorYChannel(Node): pass
class InputVectorZChannel(Node): pass
class OutputChannel(Node): __ALL__ = []
class Schedule1(Node): pass
class Schedule2(Node): pass
class Scale(Node): pass
class Lesser(Node): pass
class Greater(Node): pass
class Switch(Node): pass
class BooleanSwitch(Node): pass
class Constant(Node): pass
class Integrator(Node): pass
class Divide(Node): pass
class Multiply(Node): pass


def error(msg):
	print >>sys.stderr, '\nError:', msg
	sys.exit(1)

class _forward:
	def __init__(self, id):
		self.id = id
	def __call__(self):
		return network[self.id]

def forward(ids):
	ids = ids.split()
	for id in ids:
		network[id] = _forward(id)


networks = {}
def RegisterNetwork(g):
	global networks
	networks[g['__name__']] = g


def GenerateNetwork(source):
	global networks
	global network
	network = {}
	[network.update(n) for n in networks.values()]
	print '<?xml version="1.0" standalone="no"?>'
	print '<!-- '
	print '     *** WARNING: THIS IS A GENERATED FILE ***'
	print '     Think twice before editing this file directly!'
	print '     Generated from %s at %s' % (source, time.ctime())
	print '-->'
	print '<Object class="FlightControlSystem">'
	print '\t<List name="nodes">'

	nodes = {}
	for g in network.values():
		try:
			if not issubclass(g, Node): continue
		except:
			continue
		nodes[g.__name__] = g

	"""
	old_names = [x.strip() for x in open('sort').readlines()]
	for name in old_names:
		name = {'de.l':'de_left', 'de.r':'de_right',
				'q_bar':'qbar','control.pitch':'pitch_control','control.yaw':'yaw_control','control.roll':'roll_control',
				'deflection.left_elevator':'left_elevator_deflection','deflection.right_elevator':'right_elevator_deflection'}.get(name, name)
		if nodes.get(name):
			nodes[name]()._out()
			del nodes[name]
	"""

	names = nodes.keys()
	names.sort()
	for name in names:
		nodes[name]()._out()

	print '\t</List>'
	print '</Object>'

	for node in OutputChannel.__ALL__:
		__MarkAlive(node)

	first = 1
	for name in names:
		c = nodes[name]
		if Node in c.__bases__ or c == Node: continue
		if not getattr(c, '__ALIVE__', 0):
			if first:
				print >>sys.stderr, "\nDEAD NODES:"
				first = 0
			print >>sys.stderr, "... %s" % name

def __MarkAlive(node):
	if getattr(node, '__ALIVE__', 0): return
	node.__ALIVE__ = 1
	for input in getattr(node, '__INPUTS__', ()):
		__MarkAlive(input)

def __GenerateNetwork(globals):
	global network
	network = globals
	for g in network.values():
		try:
			if not issubclass(g, Node): continue
		except:
			continue
		g()._out()

def EndNetwork():
	for g in network.values():
		try:
			if not issubclass(g, Node): continue
		except:
			continue
		g()._out()


class table(BaseType):
	def __init__(self, breaks, values, spacing, mode="linear"):
		if isinstance(breaks, str): breaks = breaks.split()
		if isinstance(values, str): values = values.split()
		breaks = map(float, breaks)
		values = map(float, values)
		self.mode = mode
		self.breaks = breaks
		self.values = values
		self.spacing = spacing
		if len(breaks) < 1: raise('No values in table')
		if len(values) != len(breaks): raise('Incorrect number of values')
		sorted_breaks = breaks[:]
		sorted_breaks.sort()
		if sorted_breaks != breaks:
			raise('Breaks are out of order')
	def out(self, key):
		print '\t\t\t<Table1 name="%s" method="%s">' % (key, self.mode)
		print '\t\t\t\t<Breaks0 spacing="%s">%s</Breaks0>' % (self.spacing, ' '.join(map(str, self.breaks)))
		print '\t\t\t\t<Values>%s</Values>' % ' '.join(map(str, self.values))
		print '\t\t\t</Table1>'

