#ifndef __CSP_LAYOUT_OSGSCENEWINDOW_H__
#define __CSP_LAYOUT_OSGSCENEWINDOW_H__

#include <csp/csplib/util/Ref.h>
#include <csp/tools/layout2/cpp/OsgGraphicsWindow.h>

namespace csp {
namespace layout {

class FeatureGraph;
class Scene;
	
/* This is a graphical window that we can draw 3D into. It is called
from a wxPython window (wx.glcanvas.GLCanvas inherited class). This 
makes it possible to integrate wxPython and open scene graph. */
class OsgSceneWindow : public OsgGraphicsWindow {
public:
	OsgSceneWindow();

	void moveCameraToHome();

	FeatureGraph* graph();

	/** The frame method has been overriden to be able to generate a grid to the view. */
	virtual void Frame();
private:
	Ref<Scene> m_Scene;

};

} // namespace layout
} // namespace csp

#endif // __CSP_LAYOUT_OSGSCENEWINDOW_H__
