// Combat Simulator Project
// Copyright (C) 2007 The Combat Simulator Project
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

#include <csp/cspwf/ControlCallback.h>
#include <csp/cspwf/Serialization.h>
#include <csp/cspwf/StyleBuilder.h>
#include <csp/cspwf/WindowManager.h>

#include <osg/AlphaFunc>
#include <osg/BlendFunc>
#include <osg/Group>
#include <osg/LightModel>
#include <osg/MatrixTransform>
#include <osg/TexEnv>
#include <osgUtil/SceneView>
#include <osgUtil/IntersectVisitor>

#include <SDL/SDL.h>

namespace csp {

namespace wf {

WindowManager::WindowManager(osg::State* state, int screenWidth, int screenHeight) : 
	m_ScreenWidth(screenWidth), m_ScreenHeight(screenHeight), m_Group(new osg::Group) {

	Size screenSize;
	float screenScale;
	calculateScreenSizeAndScale(screenSize, screenScale);
	if(screenScale != 1.0) {
		osg::ref_ptr<osg::MatrixTransform> scaledGroup = new osg::MatrixTransform;
		osg::Matrix scale;
		scale.makeScale(osg::Vec3(screenScale, screenScale, screenScale));
		scaledGroup->setMatrix(scale);
		m_Group = scaledGroup.get();
	}

	osg::ref_ptr<osg::DisplaySettings> displaySettings = new osg::DisplaySettings();
	displaySettings->setDefaults();

	osgUtil::SceneView *sv = new osgUtil::SceneView(displaySettings.get());
	sv->setDefaults(osgUtil::SceneView::COMPILE_GLOBJECTS_AT_INIT);
	sv->setState(state);
	sv->setViewport(0, 0, m_ScreenWidth, m_ScreenHeight);

	// left, right, bottom, top, zNear, zFar
	sv->setProjectionMatrixAsOrtho(0, m_ScreenWidth, m_ScreenHeight, 0, -1000, 1000);
	sv->setComputeNearFarMode(osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR);
	sv->setLightingMode(osgUtil::SceneView::NO_SCENEVIEW_LIGHT);
	osg::ref_ptr<osg::StateSet> globalStateSet = new osg::StateSet;
	globalStateSet->setGlobalDefaults();
	globalStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	globalStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	globalStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	// create a light model to eliminate the default ambient light.
	osg::ref_ptr<osg::LightModel> light_model = new osg::LightModel();
	light_model->setAmbientIntensity(osg::Vec4(0.0, 0.0, 0.0, 1.0));
	globalStateSet->setAttributeAndModes(light_model.get(), osg::StateAttribute::ON);

	// set up an alphafunc by default to speed up blending operations.
	osg::ref_ptr<osg::AlphaFunc> alphafunc = new osg::AlphaFunc;
	alphafunc->setFunction(osg::AlphaFunc::GREATER, 0.0f);
	globalStateSet->setAttributeAndModes(alphafunc.get(), osg::StateAttribute::ON);

	// set up an texture environment by default to speed up blending operations.
	osg::ref_ptr<osg::TexEnv> texenv = new osg::TexEnv;
	texenv->setMode(osg::TexEnv::MODULATE);
	globalStateSet->setTextureAttributeAndModes(0, texenv.get(), osg::StateAttribute::ON);

    osg::ref_ptr<osg::BlendFunc> blendFunction = new osg::BlendFunc;
    globalStateSet->setAttributeAndModes(blendFunction.get());
	
	sv->setGlobalStateSet(globalStateSet.get());
	sv->getCullVisitor()->setImpostorsActive(true);
	sv->getCullVisitor()->setComputeNearFarMode(osgUtil::CullVisitor::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
	sv->getCullVisitor()->setCullingMode(osgUtil::CullVisitor::ENABLE_ALL_CULLING);
	// sv->setCullMask(SceneMasks::CULL_ONLY | SceneMasks::NORMAL);
	// sv->getUpdateVisitor()->setTraversalMask(SceneMasks::UPDATE_ONLY | SceneMasks::NORMAL);
	sv->getCamera()->setClearMask(GL_DEPTH_BUFFER_BIT);

	m_View = sv;

	// eye, center, up
	osg::Matrix view_matrix;
	view_matrix.makeLookAt(osg::Vec3(0, 0, 100.0), osg::Vec3(0.0, 0.0, 0.0), osg::Vec3(0, 1, 0));
	m_View->setViewMatrix(view_matrix);

	m_View->setSceneData(m_Group.get());
}

WindowManager::~WindowManager() {
}

osgUtil::SceneView* WindowManager::getSceneView() {
	return m_View.get();
}

bool WindowManager::onClick(int x, int y) {
	ClickEventArgs event(x, y);
	Control* control = getControlAtPosition(x, y);
	if(control != NULL) {
		control->onClick(event);
	}
	return event.handled;
}

bool WindowManager::onMouseMove(int x, int y, int /*dx*/, int /*dy*/) {
	// Save the new mouse position.
	m_MousePosition = Point(x, y);

	Control* newHoverControl = getControlAtPosition(x, y);
	if(newHoverControl != m_HoverControl.get()) {
		removeStateAndRebuildGeometry("hover", m_HoverControl.get());
		addStateAndRebuildGeometry("hover", newHoverControl);
	}
	m_HoverControl = newHoverControl;
	return false;
}

void WindowManager::removeStateAndRebuildGeometry(const std::string& state, Control* control) {
	if(control == NULL) {
		return;
	}

	Ref<Style> currentStyle = StyleBuilder::buildStyle(control);
	control->removeState(state);
	Ref<Style> newStyle = StyleBuilder::buildStyle(control);
	if(!currentStyle->equals(newStyle.get())) {
		control->buildGeometry();
	}
}

void WindowManager::addStateAndRebuildGeometry(const std::string& state, Control* control) {
	if(control == NULL) {
		return;
	}

	Ref<Style> currentStyle = StyleBuilder::buildStyle(control);
	control->addState(state);
	Ref<Style> newStyle = StyleBuilder::buildStyle(control);
	if(!currentStyle->equals(newStyle.get())) {
		control->buildGeometry();
	}
}

Point WindowManager::getMousePosition() const {
	return m_MousePosition;
}

Control* WindowManager::getControlAtPosition(int x, int y) {
	if (m_Group->getNumChildren() > 0) {
		osg::Vec3 var_near;
		osg::Vec3 var_far;
		const int height = static_cast<int>(m_View->getViewport()->height());
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

Window* WindowManager::getById(const std::string& id) {
	WindowVector::iterator window = m_Windows.begin();
	for(;window != m_Windows.end();++window) {
		if((*window)->getId() == id) {
			return window->get();
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

	// We do wish to handle alignment when the window is displayed for
	// the first time.
	Point windowLocation = window->getLocation();
	const Size windowSize = window->getSize();
	Size screenSize = getScreenSize();

	Ref<Style> windowStyle = StyleBuilder::buildStyle(window);
	if(windowStyle->getHorizontalAlign()) {
		if(*windowStyle->getHorizontalAlign() == "left") {
			windowLocation.x = 0;
		}
		else if(*windowStyle->getHorizontalAlign() == "center") {
			windowLocation.x = (screenSize.width / 2) - (windowSize.width / 2);
		}
		else if(*windowStyle->getHorizontalAlign() == "right") {
			windowLocation.x = screenSize.width - windowSize.width;
		}
	}
	if(windowStyle->getVerticalAlign()) {
		if(*windowStyle->getVerticalAlign() == "top") {
			windowLocation.y = 0;
		}
		else if(*windowStyle->getVerticalAlign() == "middle") {
			windowLocation.y = (screenSize.height / 2) - (windowSize.height / 2);
		}
		else if(*windowStyle->getVerticalAlign() == "bottom") {
			windowLocation.y = screenSize.height - windowSize.height;
		}
	}
	Ref<Style> style = window->getStyle();
	style->setLeft(Style::UnitValue(Style::Pixels, windowLocation.x));
	style->setTop(Style::UnitValue(Style::Pixels, windowLocation.y));

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
	Size size;
	float scale;
	calculateScreenSizeAndScale(size, scale);
	return size;
}

void WindowManager::calculateScreenSizeAndScale(Size& size, float& scale) const {
	Size screenSize(m_ScreenWidth, m_ScreenHeight);

	if(screenSize.width >= 1024 && screenSize.height >= 768) {
		size = screenSize;
		scale = 1.0;
		return;
	}

	// Well the user has a low screen resolution. We do wish to scale down
	// all user interface elements into smaler controls. We current don't have
	// support for rescaling of controls. So lets fake a higher screen
	// resolution and calculate a scale transformation.
	size = screenSize;
	double minimumRatio = 1024.0 / 768.0;
	double aspectRatio = screenSize.width / screenSize.height;
	if(aspectRatio > minimumRatio) {
		size.width *= (768.0 / screenSize.height);
		size.height *= (768.0 / screenSize.height);
		scale = screenSize.height / 768.0;
	}
	else {
		size.width *= (1024.0 / screenSize.width);
		size.height *= (1024.0 / screenSize.width);
		scale = screenSize.width / 1024.0;
	}
}

void WindowManager::onUpdate(float /*dt*/) {
}

void WindowManager::onRender() {
	m_View->update();
	m_View->cull();
	m_View->draw();
}

} // namespace wf

} // namespace csp
