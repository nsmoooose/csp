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


#ifndef __CSP_LAYOUT_MODELLOADER_H__
#define __CSP_LAYOUT_MODELLOADER_H__

#include <osg/ref_ptr>
#include <string>

namespace osg { class Node; }
namespace csp { class Object; }

namespace csp {
namespace layout {

// Retrieve the scene graph for an ObjectModel
osg::ref_ptr<osg::Node> getObjectModel(csp::Object *object);

// Retrieve the scene graph for an FeatureModel
osg::ref_ptr<osg::Node> getFeatureModel(csp::Object *object);

}	// End namespace layout
}	// End namespace csp

#endif // __CSP_LAYOUT_MODELLOADER_H__
