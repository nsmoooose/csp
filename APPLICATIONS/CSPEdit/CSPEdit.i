// CSPEdit - Copyright (C) 2003 Mark Rose
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

%module CSPEdit
%{
#include "View.h"
%}

%include "typemaps.i"
%include "std_string.i"

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


%include "View.h"
%include "FeatureNodes.h"

