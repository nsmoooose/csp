
/* -*-c++-*- 
 *
 * CSPEdit - Copyright (C) 2003 Mark Rose
 *
 * Based on OpenSceneGraph - Copyright (C) 1998-2003 Robert Osfield 
 *
 * This library is open source and may be redistributed and/or modified under  
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or 
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * OpenSceneGraph Public License for more details.
*/

/* osgpick sample
* demonstrate use of osgUtil/PickVisitor for picking in a HUD or
* in a 3d scene,
*/

#include "FeatureNodes.h"

#include <osgUtil/Optimizer>
#include <osgDB/ReadFile>
#include <osgProducer/Viewer>

#include <osg/Material>
#include <osg/Geode>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Projection>
#include <osg/MatrixTransform>

#include <osgText/Text>


class View;

// class to handle events with a pick
class PickHandler : public osgGA::GUIEventHandler {
	enum {
		NONE,
		SELECT_ONE,
		SELECT_EXTEND,
		SELECT_REMOVE,
	};
public: 
	PickHandler(View* viewer);

	~PickHandler() {}

	bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us);

	virtual bool pick(const osgGA::GUIEventAdapter& ea, int mode=SELECT_ONE);
    
protected:

	void deselect();
	FeatureNode *walkUp(osg::Node *node);

	osgProducer::Viewer* _viewer;
	View *_view;
};



