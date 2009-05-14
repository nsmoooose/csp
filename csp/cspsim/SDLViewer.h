#ifndef __CSPSIM_SDLVIEWER_H__
#define __CSPSIM_SDLVIEWER_H__

#include <csp/csplib/util/Ref.h>
#include <osgViewer/Viewer>

namespace csp {

class SDLViewer : public Referenced {
public:
	SDLViewer(int width, int height);
	
	virtual osgViewer::Viewer* getViewer();
	
	void frame();
	
private:
	osgViewer::Viewer m_Viewer;
	osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> m_GraphicsWindow;
};

}
#endif // __CSPSIM_SDLVIEWER_H__
