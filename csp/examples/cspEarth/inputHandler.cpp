#include <osgViewer\Viewer>
#include <osgGA\GUIEventAdapter>
#include <osgGA\GUIActionAdapter>
#include <osgEarthUtil\EarthManipulator>

#include <iostream>

#include "inputHandler.h"

extern void dumpSceneToDisk();
extern void showNodeNames();
extern osgEarthUtil::EarthManipulator* getManipulator();


static osgEarthUtil::Viewpoint VPs[] = {
    osgEarthUtil::Viewpoint( "Africa",        osg::Vec3d(    0.0,   0.0, 0.0 ), 0.0, -90.0, 10e6 ),
    osgEarthUtil::Viewpoint( "California",    osg::Vec3d( -121.0,  34.0, 0.0 ), 0.0, -90.0, 6e6 ),
    osgEarthUtil::Viewpoint( "Europe",        osg::Vec3d(    0.0,  45.0, 0.0 ), 0.0, -90.0, 4e6 ),
    osgEarthUtil::Viewpoint( "Washington DC", osg::Vec3d(  -77.0,  38.0, 0.0 ), 0.0, -90.0, 1e6 ),
    osgEarthUtil::Viewpoint( "Australia",     osg::Vec3d(  135.0, -20.0, 0.0 ), 0.0, -90.0, 2e6 )
};

inputHandler::inputHandler(void)
{
	//std::cout << "Initializing inputHandler" << std::endl;
}


inputHandler::~inputHandler(void)
{
}

bool inputHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
    if (!viewer) return false;

        switch(ea.getEventType())
        {
            case(osgGA::GUIEventAdapter::KEYUP):
			{
				if (ea.getKey()==osgGA::GUIEventAdapter::KEY_F1)
				// dump root node of scene to disk
				{
					dumpSceneToDisk();				
					return true;
				}
				if (ea.getKey()==osgGA::GUIEventAdapter::KEY_F3)
				// dump root node of scene to disk
				{
					showNodeNames();
					return true;
				}
				if (ea.getKey() >= '1' && ea.getKey() <= '5')
				{
					getManipulator()->setViewpoint( VPs[ea.getKey()-'1'], 4.0 );
					//std::cout << "setting viewpoint " << ea.getKey()-'1' << std::endl;
					return true;
				}

            default:
                return false;
			}
        }
}
