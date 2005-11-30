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


import exceptions
import os.path
import sys
import types

bin = os.path.join(os.path.dirname(__file__), '.bin')
sys.path.insert(0, bin)

import csplib as _csplib
from csplib import *

version = _csplib.getVersion()

## The C++ InterfaceRegistry.
g_InterfaceRegistry = _csplib.InterfaceRegistry.getInterfaceRegistry()

# Bring wrapped exception type into scope and give it some sugar.
libexc = _csplib._csplib.libexc
def __libexc_clear(self): self.args[0].clear()
def __libexc_type(self): return self.args[0].getType()
def __libexc_message(self): return self.args[0].getMessage()
def __libexc_trace(self): return self.args[0].getTrace()
def __libexc_has_trace(self): return self.args[0].hasTrace()
def __libexc_details(self,no_trace=0): return self.args[0].details(no_trace)
libexc.clear = __libexc_clear
libexc.type = __libexc_type
libexc.message = __libexc_message
libexc.trace = __libexc_trace
libexc.has_trace = __libexc_has_trace
libexc.details = __libexc_details

hash_string = _csplib.newhasht_cstring

## Specialized Link class for Python.  This class behaves like
## Object, but automatically combines C++ and Python reference
## counting to handle object lifetime in a sane way.  Both C++
## and Python createObject methods return Link class objects
## under Python, so it should never be necessary to handle a
## raw Object instance.
class Link(Object):
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
InterfaceProxy.createObject = \
	lambda self, method=InterfaceProxy.createObject: Link(method(self))


##################################################################################
# Code for extending csplib in Python, copied from SimData but not yet integrated.

#class List(cSimData.ListBase, list):
#	def __init__(self, type):
#		self.type = type
#		if type is int:
#			self.unpack_item = lambda p: p.unpack_int()
#		elif type is float:
#			self.unpack_item = lambda p: p.unpack_double()
#		elif type is str:
#			self.unpack_item = lambda p: p.unpack_string()
#		elif issubclass(type, cSimData.BaseType):
#			def unpack_item(p, type=type):
#				o = type()
#				o.unpack(p)
#				return o
#			self.unpack_item = unpack_item
#		else:
#			err = "Unrecognized type for SimData.List: %s" % type
#			raise err
#		list.__init__(self)
#		cSimData.ListBase.__init__(self)
#	def append(self, x):
#		if not isinstance(x, self.type):
#			err = "Appending %s to SimData.List of type %s." % (type(x), self.type)
#			raise exceptions.TypeError, err
#		list.append(self, x)
#	push_back = append
#	def clear(self):
#		del self[:]
#	def pack(self, p):
#		p.pack(len(self))
#		map(p.pack, self)
#	def unpack(self, p):
#		del self[:]
#		n = p.unpack_int()
#		for i in range(n):
#			self.append(self.unpack_item(p))
#	def __repr__(self):
#		return list.__repr__(self)


## Python object class wrapper for use by the ObjectRegistry.
##
## This class mirrors the C++ ObjectProxy<> template class;
## there is no need for templates in python since we can just
## store the class directly.

#>>class _____PyInterfaceProxy(cSimData.InterfaceProxy):
#>>	def __init__(self, classname, _class, _baseinterface, interface):
#>>		cSimData.InterfaceProxy.__init__(self, classname, _class._hash)
#>>		self._classname = classname
#>>		self._class = _class
#>>		self._baseinterface = _baseinterface
#>>		self.interface_by_variable = {}
#>>		self.interface_by_name = {}
#>>		for i in interface:
#>>			if self.variableExists(i.name):
#>>				msg = "variable %s multiply defined" % i.name
#>>				msg = msg + " in interface to class %s or parent interface." % classname
#>>				raise msg
#>>			self.interface_by_variable[i.variable] = i
#>>			self.interface_by_name[i.name] = i
#>>	def createObject(self):
#>>		return Link(self._class())
#>>#	def createObject(self, disown=1):
#>>		# Reference counting the newly created object is a bit subtle.
#>>		# if returned to python as an object, we would want python to
#>>		# retain ownership.  The only caller of createObject(), however,
#>>		# should be the DataArchive class, which wraps the resulting
#>>		# object in a Link<>.  The Link maintains its own
#>>		# reference count and deletes the C++ object (and its associated
#>>		# Python object) when the referencecount goes to zero.
#>>		#
#>>		# Keep in mind that if you use this method elsewhere and don't
#>>		# wrap the object in a Link<>, you will be responsible
#>>		# for deleting the object (which can only be done from C++).
#>>#		o = self._class()
#>>#		if disown:
#>>#			o = o.__disown__()
#>>#		return o
#>>	def getClassHash(self):
#>>		return self._class._hash
#>>	def getClassName(self):
#>>		return self._class._name
#>>	def verifyExists(self, object, name):
#>>		if not self.hasAttribute(object, name):
#>>			err = "Member variable '%s' of object (class %s) undefined in '%s' interface." % (variable, object.__class__, self.getClassName())
#>>			raise err
#>>		return self.interface_by_name[name].variable
#>>	def get(self, object, name):
#>>		variable = self.verifyExists(object, name)
#>>		return getattr(object, variable)
#>>	def set(self, object, name, value):
#>>		variable = self.verifyExists(object, name)
#>>		set_default = 1
#>>		if isinstance(name, types.StringType):
#>>			var = self.get(object, name)
#>>			if isinstance(var, cSimData.Enum):
#>>				var.fromString(value)
#>>				set_default = 0
#>>		if set_default:
#>>			setattr(object, variable, value)
#>>	def set_enum(self, object, name, value):
#>>		self.get(object, name).fromString(value)
#>>	def push_back(self, object, name, value):
#>>		variable = self.verifyExists(object, name)
#>>		member = self.get(object, name)
#>>		if isinstance(member, list):
#>>			try:
#>>				# FIXME using python lists precludes type checking
#>>				member.append(value)
#>>			except:
#>>				print "Appending to %s::%s." % (object.__class__, variable)
#>>				raise
#>>		else:
#>>			cSimData.InterfaceProxy.push_back(self, object, name, value)
#>>	def clear(self, object, name):
#>>		member = self.get(object, name)
#>>		if isinstance(member, list):
#>>			# FIXME using python lists precludes type checking
#>>			del member[:]
#>>			#member.clear()
#>>		else:
#>>			cSimData.InterfaceProxy.clear(self, object, name)
#>>	def hasAttribute(self, object, name):
#>>		variable = self.interface_by_name[name].variable
#>>		return hasattr(object, variable)
#>>	def getVariableNames(self):
#>>		names = self.interface_by_name.keys()
#>>		if self._baseinterface is not None:
#>>			names.extend(self._baseinterface.getVariableNames())
#>>		return names
#>>	def getRequiredNames(self):
#>>		required = lambda x, n=self.interface_by_name: n[x].required
#>>		names = filter(required, self.getVariableNames())
#>>		if self._baseinterface is not None:
#>>			names.extend(self._baseinterface.getRequiredNames())
#>>		return names
#>>	def variableExists(self, name):
#>>		if self.interface_by_name.has_key(name): return 1
#>>		if self._baseinterface is not None:
#>>			return self._baseinterface.variableExists(name)
#>>		return 0
#>>	def variableRequired(self, name):
#>>		if self.interface_by_name.has_key(name):
#>>			return self.interface_by_name[name].required
#>>		if self._baseinterface is not None:
#>>			return self._baseinterface.variableRequired(name)
#>>		return 0
#>>
#>>
#>>## Register an object interface class with the global InterfaceRegistry.
#>>##
#>>## Call this function for each Python Object class you define to create
#>>## an interface and register it with the InterfaceRegistry so that the
#>>## correct classes can be created when loading objects from an archive.
#>>## _class is the class name, major and minor are version numbers.  Note
#>>## that any change of the major version number makes the class incompatible
#>>## with previously archived versions of the class, forcing recompilation
#>>## of the archive.
#>>
#>>class _____XML:
#>>	def __init__(self, name, variable, required):
#>>		self.name = name
#>>		self.variable = variable
#>>		self.required = required
#>>
#>>
#>>def _____XML_INTERFACE(_class, major, minor, *args):
#>>
#>>	_baseclass = None
#>>	_baseinterface = None
#>>	if len(args):
#>>		if isinstance(args[0], types.ClassType):
#>>			_baseclass, args = args[0], args[1:]
#>>			_baseclass = _baseclass.__name__
#>>		elif isinstance(args[0], types.StringType):
#>>			_baseclass, args = args[0], args[1:]
#>>		if _baseclass is not None:
#>>			_baseinterface = g_InterfaceRegistry.getInterface(_baseclass)
#>>	classname = getClassName(_class)
#>>	fullname = "%s:%d.%d" % (classname, major, minor)
#>>	hashname = "%s:%d" % (classname, major)
#>>
#>>	_class._name = fullname
#>>	_class._hash = hash_string(hashname)
#>>	_class.getClassName = lambda x: x.__class__._name
#>>	_class.getClassHash = lambda x: x.__class__._hash
#>>	proxy = PyInterfaceProxy(classname, _class, _baseinterface, args).__disown__()


#############################################################################
# NEW STYLE INTERFACE
#############################################################################

# class SIMDATA_XML:
# 	typeString = {
# 		int: "type::int32",
# 		float: "type::float",
# 		str: "type::string"
# 	}
# 	def __init__(self, name, type, var, req):
# 		self.name = name
# 		self.type = type
# 		self.var = var
# 		self.req = req
# 		if issubclass(type, BaseType):
# 			self.typeString = type().typeString()
# 		else:
# 			self.typeString = SIMDATA_XML.typeString[type]
#
# class ObjectInterface(cSimData.ObjectInterfaceBase):
# 	"""@brief Interface for simdata::Object derived classes implemented in
# 	Python.
#
# 	This is an internal class that provides an interfacing for accessing
# 	and introspecting object member variables that can be initialized
# 	from external (XML) data sources.  The interface is equivalent to
# 	the C++ ObjectInterface class, but the implementation is entirely
# 	Python specific.
# 	"""
#
# 	def __init__(self):
# 		"""Default constructor."""
# 		self.__accessors = {}
#
# 	def _def(self, d):
# 		"""Define a new member variable accessor.
#
# 		This is an internal method used by the SIMDATA_XML function.  Do
# 		not call it directly.
# 		"""
# 		self.__accessors[d.name] = d
#
# 	def variableExists(self, name):
# 		"""Test if the interface defines a particular  member variable.
#
# 		@param name The external name of the member variable to test.
# 		"""
# 		return self.__accessors.has_key(name)
#
# 	def variableRequired(self, name):
# 		"""Test if a particular member variable must be defined in external
# 		data sources.
#
# 		Raises IndexError if the variable does not exist.
#
# 		@param name The external name of the member variable to test.
# 		"""
# 		return self.__accessors[name].req
#
# 	def set(self, obj, name, value):
# 		"""Set a member variable of a given object.
#
# 		Assigns a new value to a particular member varible of the given
# 		object.  The variable need not be previously defined, although
# 		it is strongly recommended that all externally accessible member
# 		variables be initialized during object construction.  Future
# 		implementations may test for such initialization and raise
# 		exceptions when assigning to uninitialized members.  Assigned
# 		values are tested for type compatibility.  Type mismatches
# 		currently result in an assertion exception, although a more
# 		specific exception will be used eventually.
#
# 		@param obj The object to modify.
# 		@param name The external name of the member variable to set.
# 		@param value The value to assign.
# 		"""
# 		a = self.__accessors[name]
# 		assert(isinstance(value, a.type))
# 		setattr(obj, a.var, value)
#
# 	def get(self, obj, name):
# 		"""Get the value of a member variable of a given object.
#
# 		Returns the value of a particular member variable of the
# 		given object.  Calling this method with unknown or uninitialized
# 		members will raise index and attribute exceptions, respectively.
#
# 		@param obj The object to access.
# 		@param name The external name of the member variable to get.
# 		@returns The value of the member variable.
# 		"""
# 		a = self.__accessors[name]
# 		return getattr(obj, a.var)
#
# 	def push_back(self, obj, name, value):
# 		"""Append a value to a member variable list of a given object.
# 		"""
# 		a = self.__accessors[name]
# 		getattr(obj, a.var).append(value)
#
# 	def clear(self, obj, name):
# 		"""Clear a member variable list of a given object.
# 		"""
# 		a = self.__accessors[name]
# 		assert(isinstance([], a.type))
# 		setattr(obj, a.var, [])
#
# 	def getVariables(self):
# 		"""Get a list of external names of all accessible member variables."""
# 		return self.__accessors.keys()
#
# 	def variableType(self, name):
# 		"""Get the type string of a particular member variable.
#
# 		@param name The external name of the member variable.
# 		"""
# 		a = self.__accessors[name]
# 		return a.typeString
#
#
# def __IF_init(self, IF):
# 	IF.__super.__init__(self, IF.__super)
# 	IF.__obi = ObjectInterface()
# 	map(IF.__obi._def, IF.__defs)
# 	self.addInterface(IF.__obi)
# 	print "Interface", IF.__classname, "registered."
#
# def __IF_getClassName(self):
# 	return self.__classname
#
# def __IF_getClassHash(self):
# 	return self.__classhash
#
# def __IF_createObject(self):
# 	return self.__class()
#
# __IF_methods = {
# 	'__init__': __IF_init,
# 	'getClassName': __IF_getClassName,
# 	'getClassHash': __IF_getClassHash,
# 	'createObject': __IF_createObject,
# }
#
#
# def SIMDATA_INTERFACE(cl, v, su, defs, cln = None, abstract=0):
# 	if su is None:
# 		baselist = (InterfaceProxy,)
# 		suif = InterfaceProxy
# 	else:
# 		baselist = (su.IF,)
# 		suif = su.IF
# 	if cln is None: cln = cl.__name__
# 	iname = "%s__INTERFACE" % cln
# 	methods = __IF_methods.copy()
# 	i = type(iname, baselist, methods)
# 	i.__version = v
# 	i.__super = suif
# 	i.__class = cl
# 	i.__classname = cln
# 	i.__classhash = "%s:%d" % (cln, int(v))
# 	i.__defs = defs
# 	i.__abstract = abstract
# 	if not abstract:
# 		i.__singleton = i(i)
# 	else:
# 		i.__singleton = None
# 	cl.IF = i
#



