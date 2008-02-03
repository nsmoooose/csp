// CSPLayout
// Copyright 2003-2005 Mark Rose <mkrose@users.sourceforge.net>
//
// Based in part on osgpick sample code
// OpenSceneGraph - Copyright (C) 1998-2003 Robert Osfield
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


#ifndef __CSP_LAYOUT_CSPLAYOUTAPPLICATION_H__
#define __CSP_LAYOUT_CSPLAYOUTAPPLICATION_H__

#include <csp/csplib/util/Referenced.h>

namespace csp {
namespace layout {
	
class CspLayoutApplication {
public:
	// Set the pathlist (':' separated) for loading images, models, and fonts
	// when creating ObjectModels.
	static void setOpenSceneGraphPathList(const std::string &pathlist);

	static void setShaderPath(const std::string &path);
};

} // namespace layout
} // namespace csp


#endif // __CSP_LAYOUT_CSPLAYOUTAPPLICATION_H__
