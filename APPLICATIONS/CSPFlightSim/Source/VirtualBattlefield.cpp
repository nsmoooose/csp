#include <sstream>

#include <osg/Fog>
#include <osg/Notify>
#include <osg/PolygonMode>

#include <osgDB/FileUtils>

#include <osgUtil/CullVisitor>

#include "LogStream.h"
#include "VirtualBattlefield.h"

extern int g_ScreenWidth;
extern int g_ScreenHeight;

extern double g_LatticeXDist;
extern double g_LatticeYDist;

extern osg::Node *makeSky( void );
extern osg::Node *makeBase( void );
extern osg::Node *makeTreesPatch( float xcen, float ycen, float spacing, float width, 
					 float height, VirtualBattlefield * pBattlefield);

const float SKY_RED = 0.1f;
const float SKY_GREEN = 0.1f;
const float SKY_BLUE = 0.6f;
const float SKY_ALPHA = 1.0f;

const float FOG_RED = 0.4f;
const float FOG_GREEN = 0.4f;
const float FOG_BLUE = 0.5f;
const float FOG_ALPHA = 1.0f;

const float MAX_VIEW_DISTANCE = 10000.0f;

VirtualBattlefield * g_pBattlefield = new VirtualBattlefield;


unsigned int VirtualBattlefield::latest_object_id = 0;


/**
 * struct MoveEarthySkyWithEyePointCallback
 *
 * @author unknown
 */
struct MoveEarthySkyWithEyePointCallback : public osg::Transform::ComputeTransformCallback
{
    /** Get the transformation matrix which moves from local coords to world coords.*/
    virtual bool computeLocalToWorldMatrix(osg::Matrix& matrix,const osg::Transform*, osg::NodeVisitor* nv) const 
    {
        osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
        if (cv)
        {
            osg::Vec3 eyePointLocal = cv->getEyeLocal();
            matrix.preMult(osg::Matrix::translate(eyePointLocal.x(),eyePointLocal.y(),0.0f));
			//matrix.preMult(osg::Matrix::translate(eyePointLocal.x(),eyePointLocal.y(),eyePointLocal.z()));
        }
        return true;
    }

    /** Get the transformation matrix which moves from world coords to local coords.*/
    virtual bool computeWorldToLocalMatrix(osg::Matrix& matrix,const osg::Transform*, osg::NodeVisitor* nv) const
    {
        osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
        if (cv)
        {
            osg::Vec3 eyePointLocal = cv->getEyeLocal();
			matrix.postMult(osg::Matrix::translate(-eyePointLocal.x(),-eyePointLocal.y(),0.0));
            //matrix.postMult(osg::Matrix::translate(-eyePointLocal.x(),-eyePointLocal.y(),-eyePointLocal.z()));
        }
        return true;
    }
};

VirtualBattlefield::VirtualBattlefield()
{
	m_pView = NULL;
	m_pActiveTerrainObject = NULL;

}

VirtualBattlefield::~VirtualBattlefield()
{

}

int VirtualBattlefield::create()
{
    return 1;
}

void VirtualBattlefield::Cleanup()
{
	if (m_pActiveTerrainObject)
		delete m_pActiveTerrainObject;
}


float VirtualBattlefield::getElevation( float x,float y ) const
{
	if (m_pActiveTerrainObject)
		return m_pActiveTerrainObject->GetElevation(x,y);
	else
		return 0.0;
}

void VirtualBattlefield::GetNormal(float x, float y, float & normalX, 
		float & normalY, float & normalZ ) const
{
	normalX = 0; normalY = 0; normalZ = 1;
	if (m_pActiveTerrainObject)
		m_pActiveTerrainObject->GetNormal(x,y, normalX, normalY, normalZ);
}


int VirtualBattlefield::buildScene()
{
    CSP_LOG(CSP_APP, CSP_INFO, "VirtualBattlefield::buildScene() ");
   
    int ScreenWidth = g_ScreenWidth;
    int ScreenHeight = g_ScreenHeight;

	// must be updated to match your file path configuration
#ifdef _WIN32
	std::string sep = ";";
#else
	std::string sep = ":";
#endif

	osgDB::setDataFilePathList("." + sep + "Images" + sep + "Models" + sep + "Fonts");

	// we don't need this on Linux since libs are usually
	// installed in /usr/local/lib/osgPlugins or /usr/lib/osgPlugins.
	// OSG can find itself the plugins.
#ifdef _WIN32
	osgDB::setLibraryFilePathList("../DemoPackage");
#endif

	/////////////////////////////////////
    //
	//(Un)comment to (enable) disable debug info from osg
	//
    //osg::setNotifyLevel(osg::DEBUG_INFO);
	//
    /////////////////////////////////////

    m_rpRootNode = osgNew osg::Group;
	m_rpRootNode->setName( "RootSceneGroup" );

	m_rpObjectRootNode = osgNew osg::Group;
	m_rpObjectRootNode->setName("ObjectRootSceneGraph.");

	osg::ClearNode* earthSky = osgNew osg::ClearNode;
    //earthSky->setRequiresClear(false); // we've got base and sky to do it.
    earthSky->setRequiresClear(true); // we've got base and sky to do it.
	
    // use a transform to make the sky and base around with the eye point.
    osg::Transform* transform = osgNew osg::Transform;
	
    // transform's value isn't knowm until in the cull traversal so its bounding
    // volume can't be determined, therefore culling will be invalid,
    // so switch it off, this cause all our parents to switch culling
    // off as well. But don't worry culling will be back on once underneath
    // this node or any other branch above this transform.
    transform->setCullingActive(false);
    
    // set the compute transform callback to do all the work of
    // determining the transform according to the current eye point.
    transform->setComputeTransformCallback(osgNew MoveEarthySkyWithEyePointCallback);
    
    // add the sky and base layer.
    transform->addChild(makeSky());  // bin number -2 so drawn first.
    transform->addChild(makeBase()); // bin number -1 so draw second.      
    
    // add the transform to the earth sky.
    earthSky->addChild(transform);
    
    // add to earth sky to the scene.
	m_rpRootNode->addChild(earthSky);

    m_pView = osgNew osgUtil::SceneView();
    m_pView->setDefaults();
    m_pView->setViewport(0,0,ScreenWidth,ScreenHeight);

    osg::Vec4 bkgColor;
    bkgColor[0] = SKY_RED;
    bkgColor[1] = SKY_GREEN;
    bkgColor[2] = SKY_BLUE;
    bkgColor[3] = SKY_ALPHA;
	m_pView->setBackgroundColor(bkgColor);
    m_pView->setComputeNearFarMode(osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR);
	m_pView->getCullVisitor()->setImpostorsActive(true);

	// Fog properties: start and end distances are read from Start.csp
    osg::StateSet * pFogState = osgNew osg::StateSet;
    osg::Fog* fog = osgNew osg::Fog;
    fog->setMode(osg::Fog::LINEAR);
    fog->setDensity(0.3f);

	osg::Vec4 fogColor;
	fogColor[0] = FOG_RED;
	fogColor[1] = FOG_GREEN; 
	fogColor[2] = FOG_BLUE; 
	fogColor[3] = FOG_ALPHA;
	fog->setColor(fogColor);
    pFogState->setAttributeAndModes(fog ,osg::StateAttribute::ON);
	
	// light properties
	osg::Light * pLight = osgNew osg::Light;

	pLight->setDirection( osg::Vec3(0,0,-1) );
	pLight->setAmbient( osg::Vec4(0.2f,0.2f,0.2f,1.0f) );
	pLight->setDiffuse( osg::Vec4(0.8f,0.8f,0.8f,1.0f) );
	pLight->setSpecular( osg::Vec4(0.75f,0.75f,0.75f,1.0f) );
	osg::StateSet * pLightSet = osgNew osg::StateSet;
	
	pFogState->setAttributeAndModes(pLight, osg::StateAttribute::ON);
	
	m_rpObjectRootNode->setStateSet(pLightSet);
	m_rpObjectRootNode->setStateSet(pFogState);
	
	//m_pView->setLightingMode(osgUtil::SceneView::SKY_LIGHT); // HEADLIGHT is default

	m_rpRootNode->addChild(m_rpObjectRootNode.get());

    m_pView->setSceneData(m_rpRootNode.get() );

	m_rpFrameStamp = osgNew osg::FrameStamp;

	m_pView->setFrameStamp(m_rpFrameStamp.get());

    osgDB::Registry::instance();

    osg::Camera * pCamera = m_pView->getCamera();
    pCamera->setNearFar(1.0f,20000);
	float aspect = (float)g_ScreenWidth/(float)g_ScreenHeight;
    pCamera->setPerspective(90.0f,aspect,1.0f,20000.0f);
    
    osg::Vec3  posVec(0, 0, 1000);
    osg::Vec3  lookVec(15, 15, 1000);
    osg::Vec3  upVec(0, 0, 1);
	
    pCamera->setLookAt(posVec, lookVec, upVec);
	
    return 1;
}


int VirtualBattlefield::drawScene()
{
    CSP_LOG(CSP_APP, CSP_DEBUG, "VirtualBattlefield::drawScene()...");
    
	
	for_each( objectList.begin(), objectList.end(), mem_fun(&BaseObject::updateScene) );

	osgUtil::CullVisitor * pCullVisitor = m_pView->getCullVisitor();
	pCullVisitor->setComputeNearFarMode(osgUtil::CullVisitor::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
	pCullVisitor->setCullingMode(osgUtil::CullVisitor::ENABLE_ALL_CULLING);
	m_pView->setCullVisitor(pCullVisitor);
  
	m_pView->app();

    m_pView->cull();

    m_pView->draw();

    glFinish();

	Demeter::Settings::GetInstance()->LogStats();

    return 1;
}

void VirtualBattlefield::OnUpdate(float dt)
{

  CSP_LOG(CSP_APP, CSP_DEBUG, "VirtualBattlefield::OnUpdate - entering" );

//  std::for_each( objectList.begin(), objectList.end(), &BaseObject::OnUpdate  );
  
  list<BaseObject *>::iterator i;
      
  for (i = objectList.begin() ; i != objectList.end() ; ++i)
    {
      BaseObject * object = *i;
      if (object)
	{
	  object->OnUpdate(dt);
	}
      else
      {
	    CSP_LOG( CSP_APP, CSP_WARN, "processOnUpdate - found null pointer in object list" );
      }
    }

  m_rpFrameStamp->setReferenceTime(m_rpFrameStamp->getReferenceTime() + dt);
  m_rpFrameStamp->setFrameNumber(m_rpFrameStamp->getFrameNumber() + 1);

  CSP_LOG(CSP_APP, CSP_DEBUG, "VirtualBattlefield::OnUpdate - leaving" );


}

void VirtualBattlefield::setCameraNode( osg::Node * pNode)
{

}

void VirtualBattlefield::setLookAt(StandardVector3 & eyePos, StandardVector3 & lookPos, StandardVector3 & upVec)
{
  CSP_LOG(CSP_APP, CSP_DEBUG, "VirtualBattlefield::setLookAt - eye: " << eyePos << ", look: " << lookPos << ", up: " << upVec);

  osg::Camera * pCamera = m_pView->getCamera();

  int XLatticePos = eyePos.x / g_LatticeXDist;
  int YLatticePos = eyePos.y / g_LatticeYDist;

  float localEyePosition_x = eyePos.x - g_LatticeXDist*XLatticePos;
  float localEyePosition_y = eyePos.y - g_LatticeYDist*YLatticePos;

  float localLookPosition_x = lookPos.x - g_LatticeXDist*XLatticePos;
  float localLookPosition_y = lookPos.y - g_LatticeYDist*YLatticePos;

  osg::Vec3 _localEye( localEyePosition_x,  localEyePosition_y,  eyePos.z) ;
  osg::Vec3 _localLook(localLookPosition_x, localLookPosition_y, lookPos.z ) ;
  osg::Vec3 _up (upVec.x, upVec.y, upVec.z );

  pCamera->setLookAt(_localEye, _localLook, _up);
  pCamera->ensureOrthogonalUpVector();

  if (m_pActiveTerrainObject)
		m_pActiveTerrainObject->setCameraPosition( eyePos.x, eyePos.y, eyePos.z );


  CSP_LOG(CSP_APP, CSP_DEBUG, "VirtualBattlefield::setLookAt - eye: " << pCamera->getEyePoint()  << 
	  ", look: " << pCamera->getCenterPoint()  << ", up: " << pCamera->getUpVector()  <<
	  ", near: " << pCamera->zNear() << ", far: " << pCamera->zFar() );

}

void VirtualBattlefield::GetLookAt(StandardVector3 & eyePos, StandardVector3 & lookPos, StandardVector3 & upVec) const
{
  osg::Camera * pCamera = m_pView->getCamera();
  
  osg::Vec3 _eye = pCamera->getEyePoint();
  eyePos = StandardVector3(_eye.x(), _eye.y(),_eye.z());
  osg::Vec3 _center = pCamera->getCenterPoint();
  lookPos = StandardVector3(_center.x(), _center.y(),_center.z());
  osg::Vec3 _up = pCamera->getUpVector();
  upVec = StandardVector3(_up.x(), _up.y(),_up.z());

  CSP_LOG(CSP_APP, CSP_DEBUG, "VirtualBattlefield::GetLookAt - eye: " << eyePos << ", look: " << lookPos << ", up: " << upVec);
}

void VirtualBattlefield::addObject(BaseObject * obj)
{
  CSP_LOG(CSP_APP, CSP_DEBUG, "ObjectManager::addObject, object_id: " << obj->getObjectID() );
  objectList.push_front(obj);
  ostringstream strg;
  strg << "Adding object " << obj->getObjectID() ;
  objectListHistory.push_back(strg.str());
}

void VirtualBattlefield::addController(BaseController * controller)
{

}

void VirtualBattlefield::addNodeToScene( osg::Node * pNode)
{

  CSP_LOG(CSP_APP, CSP_INFO, "VirtualBattlefield::addNodeToScene() - NodeName: " << pNode->getName() );
//  osgUtil::Optimizer optimzer;
//  optimzer.optimize(pNode, osgUtil::Optimizer::OptimizationOptions::ALL_OPTIMIZATIONS);

  m_rpObjectRootNode->addChild(pNode);
}

void VirtualBattlefield::removeNodeFromScene( osg::Node * pNode)
{
    m_rpRootNode->removeChild(pNode);
}

void VirtualBattlefield::setWireframeMode(bool flag)
{
    osg::StateSet* globalStateSet = m_pView->getGlobalStateSet();
    if (!globalStateSet)
    {
        globalStateSet = osgNew osg::StateSet;
        m_pView->setGlobalStateSet(globalStateSet);
    }

    osg::PolygonMode* polyModeObj = osgNew osg::PolygonMode;

    if (flag)
        polyModeObj->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
    else 
        polyModeObj->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
    globalStateSet->setAttribute(polyModeObj);

}

void VirtualBattlefield::setFogMode(bool flag)
{
//    osg::StateSet * pStateSet = m_rpRootNode->getStateSet();
    osg::StateSet * pStateSet = m_rpObjectRootNode->getStateSet();
    osg::StateAttribute * pStateAttr = pStateSet->getAttribute(osg::StateAttribute::FOG);

    if (flag)
        pStateSet->setAttributeAndModes(pStateAttr ,osg::StateAttribute::ON);
    else
        pStateSet->setAttributeAndModes(pStateAttr ,osg::StateAttribute::OFF);

//    m_rpRootNode->setStateSet(pStateSet);
    m_rpObjectRootNode->setStateSet(pStateSet);

}

void VirtualBattlefield::setFogStart(float value)
{
//    osg::StateSet * pStateSet = m_rpRootNode->getStateSet();
	osg::StateSet * pStateSet = m_rpObjectRootNode->getStateSet();
    osg::Fog * pFogAttr = (osg::Fog*)pStateSet->getAttribute(osg::StateAttribute::FOG);

    pFogAttr->setStart(value);

    pStateSet->setAttributeAndModes(pFogAttr ,osg::StateAttribute::ON);    
//    m_rpRootNode->setStateSet(pStateSet);
    m_rpObjectRootNode->setStateSet(pStateSet);

}

void VirtualBattlefield::setFogEnd(float value)
{
//    osg::StateSet * pStateSet = m_rpRootNode->getStateSet();
    osg::StateSet * pStateSet = m_rpObjectRootNode->getStateSet();
    osg::Fog * pFogAttr = (osg::Fog*)pStateSet->getAttribute(osg::StateAttribute::FOG);

    pFogAttr->setEnd(value);

    pStateSet->setAttributeAndModes(pFogAttr ,osg::StateAttribute::ON);    
    m_rpObjectRootNode->setStateSet(pStateSet);

}

void VirtualBattlefield::setViewDistance(float value)
{
    osg::Camera * pCamera = m_pView->getCamera();
    pCamera->setNearFar(1.0f,value);
    pCamera->setPerspective(60.0f,1.0,1.0f,value);


}

// look for the object given an id, return null if object
// is not found, in the future to speed this up a map could
// be used.
BaseObject * VirtualBattlefield::getObjectFromID( unsigned int ID)
{
  CSP_LOG(CSP_APP, CSP_DEBUG, "Getting ObjectFromID, ID: " << ID );
  
  std::list<BaseObject *>::iterator i;
  for (i = objectList.begin() ; i != objectList.end() ; ++i)
    {
      BaseObject * object = *i;
      if (ID == object->getObjectID())
	return object;
    }
  return NULL;

}

// look for an object given a name. return null if object is not found.
// this needs to be improved with a map.
BaseObject * VirtualBattlefield::getObjectFromName( string name )
{
  CSP_LOG(CSP_APP, CSP_DEBUG, "Getting ObjectFromName, name: " << name );
  std::list<BaseObject *>::iterator i;
  for (i = objectList.begin() ; i != objectList.end() ; ++i)
    {
      BaseObject * object = *i;
      if (name == object->getObjectName())
	    return object;
    }
  return NULL;



}

// removing object with ID. 
// change the deleteFlag to true.
void VirtualBattlefield::removeObjectWithID( unsigned int ID)
{
  CSP_LOG( CSP_APP, CSP_INFO, "removeObjectWithID - ID: " << ID );

  std::list<BaseObject *>::iterator i;
  for (i = objectList.begin(); i != objectList.end(); ++i)
  {
    BaseObject * object = *i;
    if (object)
	{
	  if (ID == object->getObjectID())
	  {
	      object->setDeleteFlag(true);
	  }
	}
  } 

}

void VirtualBattlefield::removeObjectsMarkedForDelete()
{
    std::list<BaseObject *>::iterator i;
      
  for (i = objectList.begin() ; i != objectList.end() ; ++i)
    {
      BaseObject * object = *i;
      if (object)
	{
	  if (object->getDeleteFlag() )
	    {
          CSP_LOG(CSP_APP, CSP_INFO, "removeObjectsMarkedForDelete() - Removing object " << object->getObjectID());

          ostringstream strg;
	      strg << "Removing object " << object->getObjectID();
	      objectListHistory.push_back(strg.str());

          removeNodeFromScene( object->getNode() );
	      i = objectList.erase(i);
	      delete (object);
	    }
	}
      else
	cerr << "removeObjectsMarkedForDelete - found null object in objectList" << endl;
    }
  


}


void VirtualBattlefield::removeAllObjects()
{
  std::list<BaseObject *>::iterator i;

  CSP_LOG(CSP_APP, CSP_INFO, "RemoveAllObjects()" );

  while(!objectList.empty())
  {
	BaseObject * object = objectList.front();
	objectList.pop_front();

    ostringstream strg;
	strg << "Removing object " << object->getObjectID();
	objectListHistory.push_back(strg.str());

    removeNodeFromScene( object->getNode() );
	delete object;
  }


}

void VirtualBattlefield::dumpAllObjects()
{
  cout << "dumpAllObjects() - Dumping List of Game Objects" << endl;
  list<BaseObject *>::iterator i;
  for (i = objectList.begin() ; i != objectList.end() ; ++i)
    {
      BaseObject * object = *i;
      if (object)
	object->dump();
      else
	cerr << "dumpAllObjects - Found null object in the object list." << endl;
    }

  cout << "dumpAllObjects() - Finished Dumping List of Game Objects" << endl;

}

// find the controller for this ID. return a null if the
// object is not found.
BaseController * VirtualBattlefield::getControllerFromID(unsigned int ID)
{
  list<BaseController *>::iterator i;
  for (i = controllerList.begin() ; i != controllerList.end() ; ++i)
    {
      BaseController * controller = *i;
      if (ID == controller->controller_id)
	return controller;
    }
  return NULL;
}



void VirtualBattlefield::dumpObjectHistory()
{
  CSP_LOG(CSP_APP, CSP_INFO, "DumpingObject history" );
  list<string>::iterator i;
  for (i = objectListHistory.begin(); i != objectListHistory.end(); ++i)
    {
      CSP_LOG(CSP_APP, CSP_INFO, *i);

    }
  CSP_LOG(CSP_APP, CSP_INFO, "Finished dumping object history" );
}

// process the OnUpdate of all game objects.
void VirtualBattlefield::updateAllObjects(float dt) // before: unsigned int dt ?!!
{
  cout << "processOnUpdate - Entering" << endl;

  list<BaseObject *>::iterator i;
      
  for (i = objectList.begin() ; i != objectList.end() ; ++i)
    {
      BaseObject * object = *i;
      if (object)
	{
	  object->OnUpdate(dt);
	}
      else
	cerr << "processOnUpdate - found null pointer in object list" << endl;
    }

  cout << "processOnUpdate - Leaving" << endl;

}

void VirtualBattlefield::initializeAllObjects()
{
  cout << "ObjectManager::initializeAllObjects" << endl;
  list<BaseObject *>::iterator i;
  i = objectList.begin();
  for (i = objectList.begin() ; i != objectList.end() ; ++i)
    {
      BaseObject * object = *i;
      if (object)
	{
	  object->initialize();
	}
      else
	cerr << "initializeGameObjects - null object found in objectList" << endl;
    }
}


// calculates a list of objects within range from a centralObject.
void VirtualBattlefield::getObjectsInRange(BaseObject * fromObj, float range, float view_angle  , int army, list<ObjectRangeInfo*> & rangeList  )
{
  list<BaseObject *>::iterator i;
  
  for (i = objectList.begin() ; i != objectList.end() ; ++i)
    {
      BaseObject *toObj = (BaseObject*)*i;
      if (fromObj->getObjectID() != toObj->getObjectID() && toObj->getArmy() == army)
	{
	  StandardVector3 distVect = toObj->getGlobalPosition() - fromObj->getGlobalPosition();
	  float dist = distVect.Length();
	  StandardVector3 distVectNormalized = Normalized(distVect);
	  float angle = angleBetweenTwoVectors(fromObj->getDirection(), distVectNormalized);
	  angle = angle*360.0/3.1415/2.0;
	  
	  //	  cout << "Distance between objects " << fromTankObj->object_id << " and "
	  //	       << toTankObj->object_id << " is " << dist << ", angle is " << angle*360.0/2.0/3.1415 << endl;
	  if (dist < range && fabs(angle) < view_angle/360.0*2.0*3.1415)
	    {
	      //   cout << "Object " << toTankObj->object_id << " is in sight from object " << fromTankObj->object_id << endl;

	      // todo use a better memory scheme
              ObjectRangeInfo * objectRangeInfo = new ObjectRangeInfo;
              objectRangeInfo->setObjectID( toObj->getObjectID() );
              objectRangeInfo->setRange( dist );
              objectRangeInfo->setDirection( distVectNormalized);
	      rangeList.push_front(objectRangeInfo);
	    }

	}
    }

}


bool VirtualBattlefield::doSphereTest( BaseObject * pObj1, BaseObject * pObj2)
{

    // return true if the distance between the two objects is less then then 
    // or equal there bounding sphere radius squared sum.

    // Calculate the radius squared sum.
    float r2 = pObj1->getBoundingRadius() * pObj1->getBoundingRadius()
        + pObj2->getBoundingRadius() * pObj2->getBoundingRadius();

    // Calculate the diff vector between the two
    StandardVector3 pDiff = pObj1->getGlobalPosition() - pObj2->getGlobalPosition();
    float p2 = Dot( pDiff, pDiff);

    if ( (p2 - r2) <= 0)
        return true;

    // Do other tests.


    return false;
 
}

void VirtualBattlefield::getObjectDistance(BaseObject * fromObject, BaseObject * toObject, float & distance, StandardVector3 & direction)
{

  StandardVector3 distVect = fromObject->getGlobalPosition() - toObject->getGlobalPosition();
  distance = distVect.Length();
  direction = Normalized(distVect);
  
}


void VirtualBattlefield::drawCockpit()
{

}

int VirtualBattlefield::getTerrainPolygonsRendered()
{
	if (m_pActiveTerrainObject)
		return m_pActiveTerrainObject->getTerrainPolygonsRendered();
	else
		return 0;

}

void VirtualBattlefield::setActiveTerrain( TerrainObject * pTerrainObject)
{
	m_pActiveTerrainObject = pTerrainObject; 

	float xPatch = 35000 + 448000;
	float yPatch = 5000 + 500000;

	if (m_pActiveTerrainObject)
		 m_pActiveTerrainObject->setCameraPosition( xPatch, yPatch, 1000);
	
	for (unsigned short i=0; i < 15;++i)
	{
		osg::Node * pTrees = makeTreesPatch( xPatch, yPatch + 256 * i, 100, 1000,
			100, this);
        pTrees->setName("Trees");
		addNodeToScene(pTrees);

	}
	

}


