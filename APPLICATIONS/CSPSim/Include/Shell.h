// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002 The Combat Simulator Project
// http://csp.sourceforge.net
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file Shell.h
 *
 * Trivial wrapper for Python scripting from C++
 * 
 */

#ifndef __SHELL_H__
#define __SHELL_H__

#include <Python.h>

class PyShell {
public:
	PyShell() {
		m_Shell = 0;
	}

	~PyShell() {
		if (m_Shell) Py_DECREF(m_Shell);
	}

	/**
	 * Bind to a python Shell object.
	 *
	 * No error checking is done at the moment to ensure that the
	 * object passed is of the correct class.
	 *
	 * @param shell A python Shell instance.
	 */
	void bind(PyObject *shell) {
		if (m_Shell) Py_DECREF(m_Shell);
		m_Shell = shell;
		if (m_Shell) Py_INCREF(m_Shell);
	}

	/**
	 * Execute a command in the python shell
	 * 
	 * @param command The command to evaluate or execute.
	 * @return The result is returned as a string.
	 */
	std::string run(std::string const &command) {
		PyObject *result;
		result = PyObject_CallMethod(m_Shell, "run", "s", command.c_str());
		if (result != NULL) {
			return PyString_AsString(result);
		}
		// should never reach this point since the python method traps
		// exceptions and returns the error and  traceback as a string
		return "";
	}
protected:
	PyObject *m_Shell;
};

#endif // __SHELL_H__

