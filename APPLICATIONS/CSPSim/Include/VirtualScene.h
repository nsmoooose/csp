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
 * @file VirtualScene.h
 *
 **/

#ifndef __VIRTUALBATTLEFIELDSCENE_H__
#define __VIRTUALBATTLEFIELDSCENE_H__


#include <SimData/Path.h>
#include <SimData/Vector3.h>
#include <SimData/Ref.h>

#include <osg/ref_ptr>

namespace osg {
	class Node;
	class Group;
	class Transform;
	class FrameStamp;
	class PositionAttitudeTransform;
}

namespace osgUtil {
	class SceneView;
}


#include "TerrainObject.h"

class Sky;
class DynamicObject;
class FalseHorizon;


/**
 * class VirtualScene
 *
 * @author unknown
 */

class VirtualScene: public simdata::Referenced
{
public:
	VirtualScene();
	virtual ~VirtualScene();

	int buildScene();
	void buildSky();

	int drawScene();

	void onUpdate(float dt);

	void addEffectUpdater(osg::Node *updater);
	void removeEffectUpdater(osg::Node *updater);
	void addParticleEmitter(osg::Node *emitter);
	void removeParticleEmitter(osg::Node *emitter);
	void addParticleSystem(osg::Node *system, osg::Node *program);
	void removeParticleSystem(osg::Node *system, osg::Node *program);
	void addObject(simdata::Ref<DynamicObject> object);
	void removeObject(simdata::Ref<DynamicObject> object);
	void addFeatureCell(osg::Node *feature);
	void removeFeatureCell(osg::Node *feature);
	void setNearObject(simdata::Ref<DynamicObject> object, bool isNear);

	void setLookAt(simdata::Vector3 & eyePos, simdata::Vector3 & lookPos, simdata::Vector3 & upVec);
	void getLookAt(simdata::Vector3 & eyePos, simdata::Vector3 & lookPos, simdata::Vector3 & upVec) const;
	void setWireframeMode(bool flag);
	void setFogMode(bool flag);
	void setFogStart(float value);
	void setFogEnd(float value);

	float getViewDistance() const { return m_ViewDistance; }
	void setViewDistance(float value);

	simdata::Vector3 const &getOrigin() { return m_Origin; }

	void setTerrain(simdata::Ref<TerrainObject>);
	simdata::Ref<TerrainObject> getTerrain() const { return m_Terrain; }
	int getTerrainPolygonsRendered();

	void setCameraNode(osg::Node *);
	float getViewAngle() const { return m_ViewAngle; }
	void setViewAngle(float);

	void spinTheWorld(bool spin);
	void resetSpin();
	double getSpin();

	void drawPlayerInterface();

protected:

	void _updateFog(simdata::Vector3 const &lookPos, simdata::Vector3 const &eyePos);

	simdata::Ref<TerrainObject> m_Terrain;

	osg::ref_ptr<osgUtil::SceneView> m_NearView;
	osg::ref_ptr<osgUtil::SceneView> m_FarView;
	osg::ref_ptr<osg::FrameStamp> m_FrameStamp;

	float m_ViewDistance;
	float m_ViewAngle;
	
	float m_FogStart;
	float m_FogEnd;
	bool m_FogEnabled;
	osg::Vec4 m_FogColor;

	bool m_SpinTheWorld;
	bool m_ResetTheWorld;

	simdata::Vector3 m_Origin;
	simdata::Vector3 m_SkyPoint;
	simdata::Vector3 m_TerrainOrigin;

	osg::ref_ptr<osg::Group> m_RootNode;

	osg::ref_ptr<osg::Transform> m_EyeTransform;
	osg::ref_ptr<Sky> m_Sky;
	osg::ref_ptr<osg::Group> m_SkyLights;

	osg::ref_ptr<osg::Group> m_NearGroup;
	osg::ref_ptr<osg::Group> m_NearObjectGroup;
	osg::ref_ptr<osg::Group> m_FogGroup;
	osg::ref_ptr<osg::Group> m_ObjectGroup;
	osg::ref_ptr<osg::Node> m_TerrainNode;
	osg::ref_ptr<osg::Group> m_FreeObjectGroup;
	osg::ref_ptr<osg::Group> m_GridObjectGroup;
	osg::ref_ptr<osg::Group> m_ParticleEmitterGroup;
	osg::ref_ptr<osg::Group> m_ParticleUpdaterGroup;
	osg::ref_ptr<osg::PositionAttitudeTransform> m_GlobalFrame;
	osg::ref_ptr<osg::PositionAttitudeTransform> m_FeatureGroup;
	osg::ref_ptr<osg::PositionAttitudeTransform> m_TerrainGroup;
};

#endif // __VIRTUALBATTLEFIELDSCENE_H__

