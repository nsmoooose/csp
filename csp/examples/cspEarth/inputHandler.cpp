#include <osgViewer\Viewer>
#include <osgGA\GUIEventAdapter>
#include <osgGA\GUIActionAdapter>

#include <iostream>

#include "inputHandler.h"

extern void dumpSceneToDisk();

inputHandler::inputHandler(void)
{
	std::cout << "Initializing inputHandler" << std::endl;
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
				dumpSceneToDisk();
				
				return true;
			}

            default:
                return false;
        }
}