#ifndef __VIRTUALBATTLEFIELDSCENE_H__
#define __VIRTUALBATTLEFIELDSCENE_H__

#include <osg/Node>
#include <osg/StateSet>
#include <osg/GeoSet>
#include <osg/Material>
#include <osg/BlendFunc>
#include <osg/MatrixTransform>
#include <osg/PolygonMode>
#include <osg/Depth>
#include <osg/Notify>
#include <osg/Geode>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgUtil/SceneView>


/**
 * class VirtualBattlefieldScene
 *
 * @author unknown
 */
class VirtualBattlefieldScene
{
	public:
	   VirtualBattlefieldScene();
	   virtual ~VirtualBattlefieldScene();

	   int BuildScene();
	   int DrawScene();



	protected:

	osgUtil::SceneView* m_pView;
	osg::Group * m_pRootNode;

	osg::Node * m_pPlaneNode;
	osg::MatrixTransform * m_pPlaneTransform;
};

#endif
