// Combat Simulator Project
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

%module(directors="1") layout_module
%{
#include <osgGA/TrackballManipulator>
#include <csp/csplib/data/Object.h>
#include <csp/csplib/util/PythonSignals.h>
#include <csp/tools/layout2/cpp/CspLayoutApplication.h>
#include <csp/tools/layout2/cpp/FeatureGraph.h>
#include <csp/tools/layout2/cpp/LayoutNodes.h>
#include <csp/tools/layout2/cpp/ModelLoader.h>
#include <csp/tools/layout2/cpp/OsgGraphicsWindow.h>
#include <csp/tools/layout2/cpp/OsgTrackballGraphicsWindow.h>
#include <csp/tools/layout2/cpp/OsgModelWindow.h>
#include <csp/tools/layout2/cpp/OsgSceneWindow.h>
#include <csp/tools/layout2/cpp/OsgTerrainOverviewWindow.h>
#include <csp/tools/layout2/cpp/XmlNode.h>
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
%feature("director") XmlNode;
%feature("director") XmlNodeDocument;

#if defined(SWIGPYTHON)
%typemap(in) (int argc, char **argv)
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
#endif

#if defined(SWIGPYTHON)
%typemap(out) (osg::Vec3)
{
	PyObject* target = PyTuple_New(3);
	PyTuple_SetItem(target, 0, PyFloat_FromDouble($1.x()));
	PyTuple_SetItem(target, 1, PyFloat_FromDouble($1.y()));
	PyTuple_SetItem(target, 2, PyFloat_FromDouble($1.z()));
	$result = target;
}
#endif

namespace osg {
class Referenced {
protected:
	~Referenced() { }
};
}

namespace csp
{
%feature("ref")   Referenced "$this->_incref();"
%feature("unref") Referenced "$this->_decref();"
%ignore Referenced;
class Referenced {};
}

%include "csp/csplib/util/swig/PythonSignals.i"

TYPEMAP_PYSLOT_IN(void)

%include "csp/tools/layout2/cpp/CspLayoutApplication.h"
%include "csp/tools/layout2/cpp/LayoutNodes.h"
%include "csp/tools/layout2/cpp/FeatureGraph.h"
%include "csp/tools/layout2/cpp/ModelLoader.h"
%include "csp/tools/layout2/cpp/OsgGraphicsWindow.h"
%include "csp/tools/layout2/cpp/OsgTrackballGraphicsWindow.h"
%include "csp/tools/layout2/cpp/OsgModelWindow.h"
%include "csp/tools/layout2/cpp/OsgSceneWindow.h"
%include "csp/tools/layout2/cpp/OsgTerrainOverviewWindow.h"
%include "csp/tools/layout2/cpp/XmlNode.h"

%extend csp::layout::OsgGraphicsWindow {
	void connectToSetCurrent(sigc::slot<void> slot) {
		$self->SetCurrent.connect(slot);
	}

	void connectToSwapBuffers(sigc::slot<void> slot) {
		$self->SwapBuffers.connect(slot);
	}
}

%include "csp/tools/layout2/cpp/OsgModelWindow.h"