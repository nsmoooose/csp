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
 * @file VirtualBattlefieldScene.h
 *
 **/

#ifndef __VIRTUALBATTLEFIELDSCENE_H__
#define __VIRTUALBATTLEFIELDSCENE_H__

#include <osg/Node>
#include <osg/MatrixTransform>

#include <osg/Depth>
#include <osgDB/FileUtils>
#include <osgUtil/SceneView>


/**
 * class VirtualBattlefieldScene
 *
 * @author unknown
 */

class VirtualBattlefieldScene {
public:
	VirtualBattlefieldScene();
	virtual ~VirtualBattlefieldScene();

	int buildScene();
	int drawScene();

	void onUpdate(float dt);

	void addNodeToScene( osg::Node * pNode);
	void removeNodeFromScene( osg::Node * pNode);

	void setLookAt(simdata::Vector3 & eyePos, simdata::Vector3 & lookPos, simdata::Vector3 & upVec);
	void getLookAt(simdata::Vector3 & eyePos, simdata::Vector3 & lookPos, simdata::Vector3 & upVec) const;
	void setWireframeMode(bool flag);
	void setFogMode(bool flag);
	void setFogStart(float value);
	void setFogEnd(float value);

	float getViewDistance() const { return m_ViewDistance; }
	void setViewDistance(float value);

	void drawPlayerInterface();

	void setCameraNode(osg::Node * pNode);
	void setActiveTerrain(TerrainObject *pTerrainObject);

	int getTerrainPolygonsRendered();

	float getViewAngle() const { return m_ViewAngle; }
	void setViewAngle(float);

protected:

	osgUtil::SceneView* m_pView;
	osg::ref_ptr<osg::Group> m_rpRootNode;
	osg::ref_ptr<osg::Group> m_rpObjectRootNode;
	osg::ref_ptr<osg::FrameStamp> m_rpFrameStamp;

	osg::Node * m_pTerrainNode;

	float m_ViewDistance;
	float m_ViewAngle;

	simdata::Pointer<TerrainObject> m_ActiveTerrainObject;

};

#endif // __VIRTUALBATTLEFIELDSCENE_H__

