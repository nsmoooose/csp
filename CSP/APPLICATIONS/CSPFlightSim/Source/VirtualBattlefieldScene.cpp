#include "stdinc.h"

#include "VirtualBattlefieldScene.h"

#include <osg/Vec4>
#include <osg/Fog>
#include <osg/Group>
#include <osg/Transform>
#include <osg/Matrix>

#include <osgDB/Registry>
#include <osgDB/ReadFile>

#include <osgUtil/Optimizer>

using namespace std;

VirtualBattlefieldScene * g_pBattleScene = new VirtualBattlefieldScene;

const float SKY_RED = 0.5f;
const float SKY_GREEN = 0.75f;
const float SKY_BLUE = 1.0f;
const float SKY_ALPHA = 1.0f;
const float MAX_VIEW_DISTANCE = 5000.0f;

VirtualBattlefieldScene::VirtualBattlefieldScene()
{
	m_pView = NULL;
	m_pRootNode = NULL;


}

VirtualBattlefieldScene::~VirtualBattlefieldScene()
{


}

int VirtualBattlefieldScene::BuildScene()
{
    int ScreenWidth = 800;
    int ScreenHeight = 600;

    m_pRootNode = osgNew osg::Group;

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


 //   m_pRootNode = new osg::Group;

   // osg::Node* rootnode = osgDB::readNodeFiles("f4e_l.3ds");

    string f4_str = "f4e_l.3ds";
    vector<std::string> v;
    v.push_back(f4_str);

    m_pPlaneNode = osgDB::readNodeFiles(v);

    m_pPlaneTransform = osgNew osg::MatrixTransform;
    osg::Matrix mat;
    mat.makeTranslate(5000, 0, 1000);

    m_pPlaneTransform->setMatrix(mat);
    m_pPlaneTransform->addChild(m_pPlaneNode);

    m_pView->setSceneData(m_pRootNode);


    osgDB::Registry::instance();
    //osgDB:: setDataFilePathList(".");

    osg::Camera * pCamera = m_pView->getCamera();
    pCamera->setNearFar(1.0f,100);
    pCamera->setPerspective(60.0f,1.0,1.0f,100);

    
    
    osg::Vec3  posVec(5025, 0, 1000);
    osg::Vec3  lookVec(5000, 0, 1000);
    osg::Vec3  upVec(0, 0, 1);

    pCamera->setLookAt(posVec, lookVec, upVec);

    m_pRootNode->addChild(m_pPlaneTransform);

    // run optimization over the scene graph
//    osgUtil::Optimizer optimzer;
//    optimzer.optimize(m_pRootNode);


 
    return 1;
}



int VirtualBattlefieldScene::DrawScene()
{
    m_pView->cull();
    m_pView->draw();
    glFinish();

    return 1;
}