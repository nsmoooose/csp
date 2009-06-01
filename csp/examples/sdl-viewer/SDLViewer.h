#ifndef __CSPSIM_SDLVIEWER_H__
#define __CSPSIM_SDLVIEWER_H__

#include <osgViewer/Viewer>

namespace osg {
	class Node;
}

namespace csp {

class SDLViewer {
public:
	SDLViewer();
	
	virtual void initialize();
	
	virtual osgViewer::Viewer* getViewer();
	
	void run();
	
private:
	bool m_Initialized;
	osgViewer::Viewer m_Viewer;
	osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> m_GraphicsWindow;
};

}
#endif // __CSPSIM_SDLVIEWER_H__
