#ifndef __VIRTUALBATTLEFIELD_H__
#define __VIRTUALBATTLEFIELD_H__

#include <list>

#include "Terrain.h"
#include "DemeterDrawable.h"

#include <osg/Node>
#include <osg/StateSet>
#include <osg/GeoSet>
#include <osg/Material>
#include <osg/BlendFunc>
#include <osg/Transform>
#include <osg/PolygonMode>
#include <osg/Depth>
#include <osg/Notify>
#include <osg/Geode>

#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#include <osgUtil/Optimizer>
#include <osgUtil/SceneView>

//#include <osgGA/CameraManipulator>

#include "BaseObject.h"
#include "ObjectRangeInfo.h"
#include "BaseController.h"
#include "TerrainObject.h"


const int MAX_NUM_VISIBLE_TRIANGLES = 30000; // Chosen based on the expected number of triangles that will be visible on-screen at any one time (the terrain mesh will typically have far more triangles than are seen at one time, especially with dynamic tessellation)


/**
 * class VirtualBattlefield
 *
 * @author unknown
 */
class VirtualBattlefield
{

public:
    VirtualBattlefield();
    virtual ~VirtualBattlefield();

    int create();
    void Cleanup();
    
    int buildScene();
    int drawScene();

    void addObject(BaseObject * pObject);
    void OnUpdate(float dt);

    BaseObject * getObjectFromID( unsigned int ID);
    BaseObject * getObjectFromName( string name );
    BaseController * getControllerFromID(unsigned int ID);
    void removeObjectWithID( unsigned int ID);
    void removeObjectsMarkedForDelete();
    void removeAllObjects();
    void dumpAllObjects();
    void dumpObjectHistory();
    void updateAllObjects(float dt);
    void initializeAllObjects();
    void getObjectsInRange(BaseObject * fromObject, float range, float view_angle  , int army, std::list<ObjectRangeInfo*> & rangeList  );
    void addController(BaseController * controller);
    unsigned int getNewObjectID() { return ++latest_object_id; }
    list<BaseObject *> getObjectList() { return objectList; }
    bool doSphereTest( BaseObject * pObj1, BaseObject * pObj2);
    void getObjectDistance(BaseObject * fromObject, BaseObject * toObject, float & distance, StandardVector3 & direction);

    void addNodeToScene( osg::Node * pNode);
    void removeNodeFromScene( osg::Node * pNode);


    void setLookAt(StandardVector3 & eyePos, StandardVector3 & lookPos, StandardVector3 & upVec);
	void GetLookAt(StandardVector3 & eyePos, StandardVector3 & lookPos, StandardVector3 & upVec) const;
    void setWireframeMode(bool flag);
    void setFogMode(bool flag);
    void setFogStart(float value);
    void setFogEnd(float value);
    void setViewDistance(float value);


    float getElevation( float x,float y ) const;
    void GetNormal(float x, float y, float & normalX, 
		float & normalY, float & normalZ ) const;

	void drawCockpit();

	void setCameraNode( osg::Node * pNode);
	void setActiveTerrain( TerrainObject * pTerrainObject);

	int getTerrainPolygonsRendered();

public:



protected:

	osgUtil::SceneView* m_pView;
	osg::ref_ptr<osg::Group> m_rpRootNode;
	osg::ref_ptr<osg::Group> m_rpObjectRootNode;
	osg::ref_ptr<osg::FrameStamp> m_frameStamp;

    osg::Node * m_pTerrainNode;
    osg::Transform * m_pTerrainTransform;

    std::list<BaseObject *> objectList;
    static unsigned int latest_object_id;
    std::list<string> objectListHistory;
    list<BaseController *> controllerList;


	TerrainObject * m_pActiveTerrainObject;

	int ConsoleFont;
};








#endif
