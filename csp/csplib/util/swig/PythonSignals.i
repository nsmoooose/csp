// Copyright 2007 Mark Rose <mkrose@users.sf.net>
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

// Input typemaps for functions taking sigc::slot arguments.  For example,
//
// %typemap(in) PYSLOT_IN(int, const char*)
// void connect(sigc::slot<int, const char*> handler);

%define TYPEMAP_PYSLOT_IN(...)
%typemap(in) sigc::slot< ##__VA_ARGS__ > { $1 = pyslot< ##__VA_ARGS__ >($input); }
%typemap(typecheck) sigc::slot< ##__VA_ARGS__ > { $1 = PyCallable_Check($input); }
%enddef

