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


## @file SimData.py
## @auther Mark Rose <mrose@stm.lbl.gov>
##
## @module SimData
## 
## Provides minimal glue code for interfacing between C++ and Python.   
## Most of the hard work is done by SWIG.  Python Objects must derive 
## from SimData.Object and be registered with the ObjectRegistry by 
## calling RegisterObject(class, version_major, version_minor) right
## after the class definition.  For example:
## 
## class MyObjectClass(SimData.Object):
##    _REQUIRED_ = "var1 var2 var3"
##    _OPTIONAL_ = "var4 var5"
## 	  ... rest of the class definition ...
##
## RegisterObject(MyObjectClass, 0, 0)
##
## Define _REQUIRED_ and _OPTIONAL_ as strings listing the reqired
## and optional member variables for serialization from XML.  
## Variable names must be separated by whitespace.


import cSimData
import exceptions, types

version = cSimData.getVersion()


# FIXME is there a more robust means of getting the class name as a 
# string?  str(class) changed from python2.1 to 2.2 for new-style 
# classes...
def getClassName(_class):
	rep = str(_class)
	if rep.startswith('<class '):
		# new-style:
		fullname = rep.split()[1][:-2] # extract name from <class 'name'>
	else:
		# old-style:
		fullname = rep
	parts = fullname.split('.')
	return parts[-1]


## The C++ InterfaceRegistry.
#g_InterfaceRegistry = cSimData.cvar.g_InterfaceRegistry
g_InterfaceRegistry = cSimData.InterfaceRegistry.getInterfaceRegistry()

hash_string = cSimData.hash_string


## Python object class wrapper for use by the ObjectRegistry.
##
## This class mirrors the C++ ObjectProxy<> template class;
## there is no need for templates in python since we can just 
## store the class directly.

class PyInterfaceProxy(cSimData.InterfaceProxy):
	def __init__(self, classname, _class, _baseinterface, interface):
		cSimData.InterfaceProxy.__init__(self, classname, _class._hash)
		self._classname = classname
		self._class = _class
		self._baseinterface = _baseinterface
		self.interface_by_variable = {}
		self.interface_by_name = {}
		for i in interface:
			if self.variableExists(i.name):
				msg = "variable %s multiply defined" % i.name
				msg = msg + " in interface to class %s or parent interface." % classname
				raise msg
			self.interface_by_variable[i.variable] = i
			self.interface_by_name[i.name] = i
	def createObject(self):
		return Link(self._class())
#	def createObject(self, disown=1):
		# Reference counting the newly created object is a bit subtle.
		# if returned to python as an object, we would want python to
		# retain ownership.  The only caller of createObject(), however,
		# should be the DataArchive class, which wraps the resulting
		# object in a Link<>.  The Link maintains its own
		# reference count and deletes the C++ object (and its associated 
		# Python object) when the referencecount goes to zero.
		#
		# Keep in mind that if you use this method elsewhere and don't
		# wrap the object in a Link<>, you will be responsible 
		# for deleting the object (which can only be done from C++).
#		o = self._class()
#		if disown:
#			o = o.__disown__()
#		return o
	def getClassHash(self):
		return self._class._hash
	def getClassName(self):
		return self._class._name
	def verifyExists(self, object, name):
		if not self.hasAttribute(object, name):
			err = "Member variable '%s' of object (class %s) undefined in '%s' interface." % (variable, object.__class__, self.getClassName())
			raise err
		return self.interface_by_name[name].variable
	def get(self, object, name):
		variable = self.verifyExists(object, name)
		return getattr(object, variable)
	def set(self, object, name, value):
		variable = self.verifyExists(object, name)
		set_default = 1
		if isinstance(name, types.StringType):
			var = self.get(object, name)
			if isinstance(var, cSimData.Enum):
				var.fromString(value)
				set_default = 0
		if set_default:
			setattr(object, variable, value)
	def set_enum(self, object, name, value):
		self.get(object, name).fromString(value)
	def push_back(self, object, name, value):
		variable = self.verifyExists(object, name)
		member = self.get(object, name)
		if isinstance(member, types.ListType) and not isinstance(member, List):
			err = "Use SimData.List rather than built-in list for %s::%s" % (object.__class__, variable)
			raise err
		if isinstance(member, List):
			try:
				member.append(value)
			except:
				print "Appending to %s::%s." % (object.__class__, variable)
				raise
		else:
			cSimData.InterfaceProxy.push_back(self, object, name, value)	
	def clear(self, object, name):
		member = self.get(object, name)
		if isinstance(member, types.ListType) and not isinstance(member, List):
			variable = self.verifyExists(object, name)
			err = "Use SimData.List rather than built-in list for %s::%s" % (object.__class__, variable)
			raise err
		if isinstance(member, List):
			member.clear()
			#self.set(object, name, List()) #del member[:]
			#self.set(object, name, List()) #[])
		else:
			cSimData.InterfaceProxy.clear(self, object, name)	
	def hasAttribute(self, object, name):
		variable = self.interface_by_name[name].variable
		return hasattr(object, variable)	
	def getVariableNames(self):
		names = self.interface_by_name.keys()
		if self._baseinterface is not None:
			names.extend(self._baseinterface.getVariableNames())
		return names
	def getRequiredNames(self):
		required = lambda x, n=self.interface_by_name: n[x].required
		names = filter(required, self.getVariableNames())
		if self._baseinterface is not None:
			names.extend(self._baseinterface.getRequiredNames())
		return names
	def variableExists(self, name):
		if self.interface_by_name.has_key(name): return 1
		if self._baseinterface is not None:
			return self._baseinterface.variableExists(name)
		return 0
	def variableRequired(self, name):
		if self.interface_by_name.has_key(name): 
			return self.interface_by_name[name].required
		if self._baseinterface is not None:
			return self._baseinterface.variableRequired(name)
		return 0


## Register an object interface class with the global InterfaceRegistry.
##
## Call this function for each Python Object class you define to create 
## an interface and register it with the InterfaceRegistry so that the 
## correct classes can be created when loading objects from an archive.
## _class is the class name, major and minor are version numbers.  Note 
## that any change of the major version number makes the class incompatible 
## with previously archived versions of the class, forcing recompilation 
## of the archive.

class XML:
	def __init__(self, name, variable, required):
		self.name = name
		self.variable = variable
		self.required = required


def XML_INTERFACE(_class, major, minor, *args):

	_baseclass = None
	_baseinterface = None
	if len(args):
		if isinstance(args[0], types.ClassType):
			_baseclass, args = args[0], args[1:]
			_baseclass = getClassName(_baseclass)
		elif isinstance(args[0], types.StringType):
			_baseclass, args = args[0], args[1:]
		if _baseclass is not None:
			_baseinterface = g_InterfaceRegistry.getInterface(_baseclass)
	classname = getClassName(_class)
	fullname = "%s:%d.%d" % (classname, major, minor)
	hashname = "%s:%d" % (classname, major)
	
	_class._name = fullname 
	_class._hash = hash_string(hashname)
	_class.getClassName = lambda x: x.__class__._name
	_class.getClassHash = lambda x: x.__class__._hash
	proxy = PyInterfaceProxy(classname, _class, _baseinterface, args).__disown__()


# Export all cSimData classes
from cSimData import *

## Specialized Link class for Python.  This class behaves like
## Object, but automatically combines C++ and Python reference 
## counting to handle object lifetime in a sane way.  Both C++
## and Python createObject methods return Link class objects
## under Python, so it should never be necessary to handle a
## raw Object instance.
class Link(cSimData.Object):
	def __init__(self, obj):
		# NOTE: skip base class __init__
		# preserve a pointer to the C++ object
		self.this = obj.this
		# increase the reference count of the C++ object
		self.ptr = LinkBase(obj)
		# disown the C++ object from Python, C++ reference
		# counting by LinkBase is now responsible for
		# the C++ object's lifetime.  when self.ptr is
		# destroyed, the reference count will decrease.
		self.thisown = 0
		try:
			obj.__disown__()
		except:
			obj.thisown = 0
		

## Override createObject to return a Link(Object)
cSimData.InterfaceProxy.createObject = \
	lambda self, method=cSimData.InterfaceProxy.createObject: Link(method(self))


class List(cSimData.ListBase, list):
	def __init__(self, type):
		self.type = type
		if type is int:
			self.unpack_item = lambda p: p.unpack_int()
		elif type is float:
			self.unpack_item = lambda p: p.unpack_double()
		elif type is str:
			self.unpack_item = lambda p: p.unpack_string()
		elif issubclass(type, cSimData.BaseType):
			def unpack_item(p, type=type):
				o = type()
				o.unpack(p)
				return o
			self.unpack_item = unpack_item 
		else:
			err = "Unrecognized type for SimData.List: %s" % type 
			raise err
		list.__init__(self)
		cSimData.ListBase.__init__(self)
	def append(self, x):
		if not isinstance(x, self.type):
			err = "Appending %s to SimData.List of type %s." % (type(x), self.type)
			raise exceptions.TypeError, err
		list.append(self, x)
	push_back = append
	def clear(self):
		del self[:]
	def pack(self, p):
		p.pack(len(self))
		map(p.pack, self)
	def unpack(self, p):
		del self[:]
		n = p.unpack_int()
		for i in range(n):
			self.append(self.unpack_item(p))
	def __repr__(self):
		return list.__repr__(self)
