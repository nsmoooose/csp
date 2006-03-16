// Combat Simulator Project
// Copyright (C) 2002-2005 The Combat Simulator Project
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

#ifndef __CSPSIM_VIRTUALBATTLEFIELDSCENE_H__
#define __CSPSIM_VIRTUALBATTLEFIELDSCENE_H__

#include <csp/cspsim/TerrainObject.h>

#include <csp/csplib/data/Path.h>
#include <csp/csplib/data/Vector3.h>
#include <csp/csplib/util/Ref.h>

#include <osg/ref_ptr>
#include <osg/Vec4>

namespace osg { class Node; }
namespace osg { class Group; }
namespace osg { class Transform; }
namespace osg { class FrameStamp; }
namespace osg { class PositionAttitudeTransform; }
namespace osg { class State; }
namespace osg { class StateSet; }
namespace osg { class DisplaySettings; }

namespace osgUtil { class SceneView; }


CSP_NAMESPACE

class Sky;
class FalseHorizon;
class FeatureTile;
class FeatureGroup;
class DynamicObject;

/**
* The purpose of this simple class is to provide a new opengl context id
* to each sceneview. sceneview's context ids are tracked.
*/
class ContextIDFactory: public Referenced {
	unsigned int m_NextContextID;
	typedef std::map<const osgUtil::SceneView *, unsigned int> ContextIDSet;
	ContextIDSet m_ContextIDSet;
public:
	/**
	* initialize m_NextContextID to 0
	*/
	ContextIDFactory();

	/**
	* assigns m_NextContextID to scene_view's context id and stores it or returns 
	its assigned
	* context id
	*/
	unsigned int getOrCreateContextID(osgUtil::SceneView *scene_view);
};


/**
 * A class that manages the 3D scene.  Currently created by CSPSim and used as a
 * singleton.  See CSPSim::getScene().
 */
class VirtualScene: public Referenced
{
public:
	VirtualScene(int width, int height);
	virtual ~VirtualScene();

	void buildScene();
	void buildSky();

	int drawScene();

	void onUpdate(float dt);

	void addEffectUpdater(osg::Node *updater);
	void removeEffectUpdater(osg::Node *updater);
	void addParticleEmitter(osg::Node *emitter);
	void removeParticleEmitter(osg::Node *emitter);
	void addParticleSystem(osg::Node *system, osg::Node *program);
	void removeParticleSystem(osg::Node *system, osg::Node *program);
	void addObject(Ref<DynamicObject> object);
	void removeObject(Ref<DynamicObject> object);
	void setNearObject(Ref<DynamicObject> object, bool isNear);

	void removeFeature(Ref<FeatureGroup> feature);
	void addFeature(Ref<FeatureGroup> feature);

	/** Called by the SceneManager to update the view point.
	 *  Do not call this method directly; instead use Battlefield->setCamera()
	 *  to update the view point.
	 */
	void _setLookAt(const Vector3& eyePos,const Vector3& lookPos,const Vector3& upVec);

	void getLookAt(Vector3 & eyePos, Vector3 & lookPos, Vector3 & upVec) const;

	void setWireframeMode(bool flag);
	bool getWireframeMode() const { return m_Wireframe; }
	void setFogMode(bool flag);
	void setFogStart(float value);
	void setFogEnd(float value);

	float getViewDistance() const { return m_ViewDistance; }
	void setViewDistance(float value);

	Vector3 const &getOrigin() const { return m_Origin; }
	Vector3 getFeatureOrigin(Ref<FeatureGroup> const &feature) const;

	void setTerrain(Ref<TerrainObject>);
	inline Ref<TerrainObject> getTerrain() const { return m_Terrain; }
	int getTerrainPolygonsRendered();

	void setCameraNode(osg::Node *);
	float getViewAngle() const { return m_ViewAngle; }
	void setViewAngle(float);
	float getNearPlane() const {return m_NearPlane;}
	void setNearPlane(float value);
	float getAspect() const {return m_Aspect;}
	void setAspect(float value);
	void getViewport(int& x,int& y,int& width,int& height);
	ContextIDFactory* const getContextIDFactory() const {return m_ContextIDFactory.get();}

	void setLabels(bool show);
	bool getLabels() const;

	void spinTheWorld(bool spin);
	void resetSpin();
	double getSpin();

	bool pick(int x, int y);

	void drawPlayerInterface();

	osg::Group *getInfoGroup();

private:

	typedef osg::ref_ptr<FeatureTile> FeatureTileRef;
	typedef std::map<int, FeatureTileRef> FeatureTileMap;
	FeatureTileMap m_FeatureTiles;

	typedef Ref<DynamicObject> DynamicObjectRef;
	typedef std::vector<DynamicObjectRef> DynamicObjectList;
	DynamicObjectList m_DynamicObjects;

	int _getFeatureTileIndex(Ref<FeatureGroup> feature) const;
	void _updateOrigin(Vector3 const &origin);

	osgUtil::SceneView *makeSceneView(unsigned mask);

	void init();
	void createSceneViews();

	void createVeryFarView();
	void createFarView();
	void createNearView();
	void createInfoView();

	void drawVeryFarView();
	void drawFarView();
	void drawNearView();
	void drawInfoView();

protected:

	void _updateFog(Vector3 const &lookPos, Vector3 const &eyePos);

	Ref<TerrainObject> m_Terrain;

	osg::ref_ptr<osgUtil::SceneView> m_InfoView;
	osg::ref_ptr<osgUtil::SceneView> m_NearView;
	osg::ref_ptr<osgUtil::SceneView> m_FarView;
	osg::ref_ptr<osgUtil::SceneView> m_VeryFarView;

	osg::ref_ptr<osg::State> m_GlobalState;
	osg::ref_ptr<osg::StateSet> m_GlobalStateSet;
	osg::ref_ptr<osg::DisplaySettings> m_DisplaySettings;

	osg::ref_ptr<osg::FrameStamp> m_FrameStamp;
	Ref<ContextIDFactory> m_ContextIDFactory;

	float m_ViewDistance;
	float m_ViewAngle;
	float m_NearPlane; // of the near view
	float m_Aspect; // of the near view
	
	float m_FogStart;
	float m_FogEnd;
	bool m_FogEnabled;
	osg::Vec4 m_FogColor;

	bool m_Wireframe;

	bool m_SpinTheWorld;
	bool m_ResetTheWorld;

	double m_FeatureTileSize;
	double m_FeatureTileScale;

	const int m_ScreenWidth;
	const int m_ScreenHeight;

	unsigned m_NodeMask;

	Vector3 m_Origin;
	Vector3 m_SkyPoint;
	Vector3 m_TerrainOrigin;

	osg::ref_ptr<Sky> m_Sky;
	osg::ref_ptr<osg::Group> m_SkyLights;

	// the root nodes for each sceneview
	osg::ref_ptr<osg::Group> m_VeryFarGroup;
	osg::ref_ptr<osg::Group> m_FarGroup;
	osg::ref_ptr<osg::Group> m_NearGroup;
	osg::ref_ptr<osg::Group> m_InfoGroup;

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

	// root node for sound effects (the listener)
	osg::ref_ptr<osg::PositionAttitudeTransform> m_SoundRoot;
};

CSP_NAMESPACE_END

#endif // __CSPSIM_VIRTUALBATTLEFIELDSCENE_H__

