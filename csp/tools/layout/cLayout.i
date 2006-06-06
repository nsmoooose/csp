// CSP Layout - Copyright 2003-2005 Mark Rose <mkrose@users.sourceforge.net>
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, US

%module(directors="1") cLayout
%{
#include <csp/tools/layout/View.h>
#include <csp/tools/layout/LayoutNodes.h>
#include <csp/tools/layout/FeatureGraph.h>
#include <csp/tools/layout/InsertDeleteCommand.h>
%}

%include "typemaps.i"
%include "std_string.i"
%include "std_vector.i"
%include "std_list.i"

%template(vector_LayoutNodePtr) std::vector<LayoutNode*>;
%template(list_LayoutNodePtr) std::list<LayoutNode*>;

%feature("director") ViewCallback;
%feature("director") GraphCallback;
%feature("director") LayoutNodeVisitor;

%typemap(python, in) (int argc, char **argv)
{
   /* Check if is a list */
   if (PyList_Check($input))
   {
      int i;
      $1 = PyList_Size($input);
      $2 = (char **) malloc(($1+1)*sizeof(char *));
      for (i = 0; i < $1; i++)
      {
         PyObject *o = PyList_GetItem($input,i);
         if (PyString_Check(o))
         {
            $2[i] = PyString_AsString(PyList_GetItem($input,i));
         }
         else
         {
            PyErr_SetString(PyExc_TypeError,"list must contain strings");
            free($2);
            return NULL;
         }
      }
      $2[i] = 0;
   }
   else
   {
      PyErr_SetString(PyExc_TypeError,"not a list");
      return NULL;
   }
}

%typemap(python, out) (osg::Vec3)
{
	PyObject* target = PyTuple_New(3);
	PyTuple_SetItem(target, 0, PyFloat_FromDouble($1.x()));
	PyTuple_SetItem(target, 1, PyFloat_FromDouble($1.y()));
	PyTuple_SetItem(target, 2, PyFloat_FromDouble($1.z()));
	$result = target;
}

namespace osg {
class Referenced {
protected:
	~Referenced() { }
};
}

// Suppress director warning:
//   FeatureGraph.h:30: Warning(515):
//     Target language argument 'osg::Matrix const &to_world' discards const
//     in director method GraphCallback::onActivate.
// Basically, just be careful not to modify to_world when subclassing
// GraphCallback  in Python.
%warnfilter(515);

%include "csp/tools/layout/LayoutNodes.h"
%include "csp/tools/layout/FeatureGraph.h"
%include "csp/tools/layout/Undo.h"
%include "csp/tools/layout/InsertDeleteCommand.h"
%include "csp/tools/layout/View.h"

