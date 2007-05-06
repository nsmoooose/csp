// Combat Simulator Project
// Copyright (C) 2002 The Combat Simulator Project
// http://csp.sourceforge.net
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file WindowManager.cpp
 *
 **/

#include <csp/cspsim/Animation.h>
#include <csp/cspsim/CSPSim.h>
#include <csp/cspsim/SceneConstants.h>
#include <csp/cspsim/wf/ControlCallback.h>
#include <csp/cspsim/wf/Serialization.h>
#include <csp/cspsim/wf/WindowManager.h>

#include <osg/BlendFunc>
#include <osg/Group>
#include <osgUtil/SceneView>
#include <osgUtil/IntersectVisitor>

#include <SDL/SDL.h>

CSP_NAMESPACE

namespace wf {

WindowManager::WindowManager(osgUtil::SceneView* view) 
	: m_View(view), m_Group(new osg::Group) {
	m_View->setSceneData(m_Group.get());

    osg::StateSet *stateSet = m_Group->getOrCreateStateSet();
    stateSet->setRenderBinDetails(100, "RenderBin");
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);    

    osg::ref_ptr<osg::BlendFunc> blendFunction = new osg::BlendFunc;
    stateSet->setAttributeAndModes(blendFunction.get());
}

WindowManager::WindowManager() : m_Group(new osg::Group) {
	const int screenWidth = CSPSim::theSim->getSDLScreen()->w;
	const int screenHeight = CSPSim::theSim->getSDLScreen()->h;

	osgUtil::SceneView *sv = new osgUtil::SceneView();
	sv->setDefaults(osgUtil::SceneView::COMPILE_GLOBJECTS_AT_INIT);
	sv->setViewport(0, 0, screenWidth, screenHeight);
	
	// left, right, bottom, top, zNear, zFar
	//sv->setProjectionMatrixAsOrtho(-screenWidth/2, screenWidth/2, screenHeight/2, -screenHeight/2, -1000, 1000);
	sv->setProjectionMatrixAsOrtho(0, screenWidth, screenHeight, 0, -1000, 1000);
	sv->setComputeNearFarMode(osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR);
	// override default HEADLIGHT mode, we provide our own lights.
	sv->setLightingMode(osgUtil::SceneView::NO_SCENEVIEW_LIGHT);
	// all scene views share a common gl context
	//sv->setState(m_GlobalState.get());
	//sv->setGlobalStateSet(m_GlobalStateSet.get());
	//sv->setFrameStamp(m_FrameStamp.get());
	// default cull settings
	sv->getCullVisitor()->setImpostorsActive(true);
	sv->getCullVisitor()->setComputeNearFarMode(osgUtil::CullVisitor::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
	sv->getCullVisitor()->setCullingMode(osgUtil::CullVisitor::ENABLE_ALL_CULLING);
	sv->setCullMask(SceneMasks::CULL_ONLY | SceneMasks::NORMAL);
	// default update settings
	sv->getUpdateVisitor()->setTraversalMask(SceneMasks::UPDATE_ONLY | SceneMasks::NORMAL);
	sv->getRenderStage()->setClearMask(GL_DEPTH_BUFFER_BIT);    	
	
	m_View = sv;
	
	// eye, center, up
	osg::Matrix view_matrix;
	view_matrix.makeLookAt(osg::Vec3(0, 0, 100.0), osg::Vec3(0.0, 0.0, 0.0), osg::Vec3(0, 1, 0));
	m_View->setViewMatrix(view_matrix);
	
	m_View->setSceneData(m_Group.get());

    osg::StateSet *stateSet = m_Group->getOrCreateStateSet();
    stateSet->setRenderBinDetails(100, "RenderBin");
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);    

    osg::ref_ptr<osg::BlendFunc> blendFunction = new osg::BlendFunc;
    stateSet->setAttributeAndModes(blendFunction.get());
}

WindowManager::~WindowManager() {
}

bool WindowManager::onClick(int x, int y) {
	ClickEventArgs event(x, y);
	Control* control = getControlAtPosition(x, y);
	if(control != NULL) {
		control->onClick(event);
	}
	return event.handled;
}

bool WindowManager::onMouseMove(int x, int y, int dx, int dy) {
	return false;
#if 0
	// Save the new mouse position.
	m_MousePosition = Point(x, y);
	
	Control* newHoverControl = getControlAtPosition(x, y);
	
	// If there is no control on the current mouse position and there
	// was a hover control before. Then we need to rebuild the geometry
	// for that control to reflect style changes.
	if(newHoverControl == NULL && m_HoverControl.valid()) {
		m_HoverControl->removeState("hover");		
		m_HoverControl->buildGeometry();
		m_HoverControl = NULL;
	}
	// If the last hover control is null and we have a new one then build
	// geometry for the new hover control.
	else if(newHoverControl != NULL && !m_HoverControl.valid()) {
		m_HoverControl = newHoverControl;
		m_HoverControl->addState("hover");
		m_HoverControl->buildGeometry();
	}
	else if(newHoverControl != NULL && m_HoverControl != newHoverControl) {
		Ref<Control> oldHover = m_HoverControl;
		m_HoverControl = newHoverControl;
		
		oldHover->buildGeometry();
		m_HoverControl->buildGeometry();
		
		oldHover->removeState("hover");
		m_HoverControl->addState("hover");
	}
	return false;
#endif
}

Point WindowManager::getMousePosition() const {
	return m_MousePosition;
}

Control* WindowManager::getControlAtPosition(int x, int y) {
	if (m_Group->getNumChildren() > 0) {
		assert(m_Group->getNumChildren() == 1);
		osg::Vec3 var_near;
		osg::Vec3 var_far;
		const int height = m_View->getViewport()->height();
		if (m_View->projectWindowXYIntoObject(x, height - y, var_near, var_far)) {
			osgUtil::IntersectVisitor iv;
			osg::ref_ptr<osg::LineSegment> line_segment = new osg::LineSegment(var_near, var_far);
			iv.addLineSegment(line_segment.get());
			m_View->getSceneData()->accept(iv);
			osgUtil::IntersectVisitor::HitList &hits = iv.getHitList(line_segment.get());
			if (!hits.empty()) {
				osg::NodePath const &nearest = hits[0]._nodePath;
				// TODO should we iterate in reverse?
				for (osg::NodePath::const_reverse_iterator iter = nearest.rbegin(); iter != nearest.rend(); ++iter) {
					osg::Node *node = *iter;
					osg::NodeCallback *callback = node->getUpdateCallback();
					if (callback) {
						ControlCallback *controlCallback = dynamic_cast<ControlCallback*>(callback);
						// TODO set flags for click type, possibly add position if we can determine a useful
						// coordinate system.
						if (controlCallback) {
							return controlCallback->getControl();
						}
					}
				}
			}
		}
	}
	return NULL;
}

void WindowManager::show(Window* window) {
	// We must initialize the window and all it's child controls. 
	// We must assign the window to this instance of the window manager.
	// It makes it possible for windows to open new or close existing
	// windows.
	window->setWindowManager(this);
	
	// Force this container to align all child controls.
	window->layoutChildControls();

	// Build the actual geometry of all controls that is going to be
	// displayed.
	window->buildGeometry();
	
	// Add the window into the tree of nodes in osg. This will make
	// the window visible for the user in the next render.
	m_Group->addChild(window->getNode());
	
	// Also store a reference to the window.
	m_Windows.push_back(window);	
}

void WindowManager::close(Window* window) {
	Ref<Window> myWindow = window;
	
	// Lets remove the node that represents the window. This will
	// remove the window on the next rendering.
	osg::Group* windowGroup = window->getNode();
	m_Group->removeChild(windowGroup);
	
	// We must also remove our reference to the Window object.
	WindowVector::iterator iteratedWindow = m_Windows.begin();
	for(;iteratedWindow != m_Windows.end();++iteratedWindow) {
		if(iteratedWindow->get() == window) {
			m_Windows.erase(iteratedWindow);
			break;
		}
	}
	
	// Detach the window from the window manager by assigning
	// a NULL window manager.
	myWindow->setWindowManager(NULL);
	myWindow->dispose();
}

void WindowManager::closeAll() {
	closeByType<Window>();
}


Size WindowManager::getScreenSize() const {
	return Size(CSPSim::theSim->getSDLScreen()->w, CSPSim::theSim->getSDLScreen()->h);
}

void WindowManager::onUpdate(float dt) {
}

void WindowManager::onRender() {
	m_View->update();
	m_View->cull();
	m_View->draw();
}

} // namespace wf

CSP_NAMESPACE_END
