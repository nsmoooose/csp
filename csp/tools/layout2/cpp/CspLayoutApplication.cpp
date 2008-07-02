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

#include <csp/cspsim/ObjectModel.h>
#include <csp/cspsim/Shader.h>
#include <csp/tools/layout2/cpp/CspLayoutApplication.h>

void csp::layout::CspLayoutApplication::setOpenSceneGraphPathList(const std::string &pathlist) {
	csp::ObjectModel::setDataFilePathList(pathlist);
}

void csp::layout::CspLayoutApplication::setShaderPath(const std::string &path) {
	Shader::instance()->setShaderPath(path);
}
