#!/usr/bin/python2.4

# Copyright 2007 Mark Rose <mkrose@users.sf.net>
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

# Generates boilerplate sigc::functor wrappers for Python callables with
# different numbers of arguments.  Redirect the output to pysig.h.

import re

class Generator:
	"""
	A simple template-based code generator with support for variable
	length arguments.  Understands the following template substititions:

		[[expr]]
			Evaluated Python expression with $N replaced by the current
			iteration value.
		[[TEMPLATE]]
			Equivalent to [[TEMPLATE,0,1]]
		[[TEMPLATE,LOW,HIGH]]
			Repeat TEMPLATE with iteration values in the range [LOW, HIGH).
			LOW and HIGH parameters can be simple numeric constants or
			arbitrary Python expressions involving $N.
	"""
	def __init__(self):
		self._symbols = {}

	def define(self, symbol, value, sep=''):
		"""
		Define a new template named 'symbol' with text 'value'.  If specified,
		'sep' will be inserted between repetitions of this template.
		"""
		self._symbols[symbol] = (value, sep)

	def _substitute(self, expr, value):
		"""
		Returns expr evaluated as a Python expression after substituting the
		current iteration value for $N.
		"""
		return eval(expr.strip().replace('$N', str(value)))

	def generate(self, template, value):
		"""Returns the substituted template for $N = value."""
		s = re.split(r'\[\[(.*?)\]\]', template)
		output = []
		for idx, part in enumerate(s):
			if idx % 2:
				if ',' in part:
					symbol, low, high = part.split(',')
					symbol = symbol.strip()
					low = self._substitute(low, value)
					high = self._substitute(high, value)
				elif '$' in part:
					output.append(str(self._substitute(part, value)))
					continue
				else:
					symbol = part.strip()
					low, high = 0, 1
				rep = []
				template, sep = self._symbols[symbol]
				for n in range(low, high):
					rep.append(self.generate(template, n))
				output.append(sep.join(rep))
			else:
				output.append(part)
		return ''.join(output)

pysig = Generator()


HEADER = r"""
// Generated code --- do not edit!

#ifndef PYSIG_H__
#define PYSIG_H__

#include <Python.h>
#include <sigc++/functors/slot.h>
#include "csp/csplib/util/PythonCasts.h"
#include "csp/csplib/util/PythonSwig.h"

struct PyError {};

template <typename T_return>
class pyslotbase : public sigc::functor_base {
public:
	typedef T_return result_type;
protected:
	pyslotbase(PyObject* handler) : _handler(handler) { Py_INCREF(_handler); }
	pyslotbase(pyslotbase const& other) : _handler(other._handler) { Py_INCREF(_handler); }
	~pyslotbase() { Py_DECREF(_handler); }
	PyObject* _handler;
	[[PYSLOTBASE.CALL,0,$N]]
private:
	pyslotbase& operator=(pyslotbase const& other);
};

[[PYSLOT,0,$N]]

template <class T_return[[T_ARGLIST_NIL,0,$N-1]]>
struct pyslot : public pyslot[[$N-1]]<T_return[[P_ARGLIST,0,$N-1]]> {
	pyslot(PyObject* handler) : pyslot[[$N-1]]<T_return[[P_ARGLIST,0,$N-1]]>(handler) {}
};
[[PYSLOT_OVERLOAD,0,$N-1]]
#endif // PYSIG_H__
"""

pysig.define('PYSLOT',
"""
template <class T_return[[T_ARGLIST,0,$N]]>
struct pyslot[[$N]] : public pyslotbase<T_return> {
	pyslot[[$N]](PyObject* handler) : pyslotbase<T_return>(handler) {}
	T_return operator()([[A_ARGLIST,0,$N]]) { return this->call(this->_handler[[PYCAST,0,$N]]); }
};
""")

pysig.define('T_ARGLIST_NIL', ", class T_arg[[$N]] = sigc::nil")
pysig.define('T_ARGLIST', ", class T_arg[[$N]]")
pysig.define('P_ARGLIST', ", T_arg[[$N]]")
pysig.define('A_ARGLIST', "T_arg[[$N]] arg[[$N]]", sep=", ")
pysig.define('ARGLIST', ", arg[[$N]]")
pysig.define('PYCAST', ", py_cast(arg[[$N]])")

pysig.define('O', "O")
pysig.define('PYOBJLIST', ", PyObject* arg[[$N]]")
pysig.define('PYDECREF', "\n		Py_DECREF(arg[[$N]]);")

pysig.define('PYSLOTBASE.CALL',
"""	T_return call(PyObject* func[[PYOBJLIST,0,$N]]) {
		PyObject* res = PyObject_CallFunction(func, "[[O,0,$N]]"[[ARGLIST,0,$N]]);[[PYDECREF,0,$N]]
		return py_decref_cast<T_return>(res);
	}
""")

pysig.define('PYSLOT_OVERLOAD',
"""
template <class T_return[[T_ARGLIST,0,$N]]>
struct pyslot<T_return[[P_ARGLIST,0,$N]]> : public pyslot[[$N]]<T_return[[P_ARGLIST,0,$N]]> {
	pyslot(PyObject* handler) : pyslot[[$N]]<T_return[[P_ARGLIST,0,$N]]>(handler) {}
};
""")


# Generate the pysig header for up to seven arguments, writing to stdout.
print pysig.generate(HEADER, 8)

