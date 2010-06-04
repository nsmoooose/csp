#pragma once

#include <osgGA/GUIEventHandler>
#include <osgGA/GUIActionAdapter>


class inputHandler : public osgGA::GUIEventHandler
{
public:
	inputHandler(void);
	~inputHandler(void);

	bool inputHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
};

