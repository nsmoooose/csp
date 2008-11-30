#include <csp/cspwf/ControlCallback.h>
#include <csp/cspwf/Serialization.h>
#include <csp/cspwf/StyleBuilder.h>
#include <csp/cspwf/WindowManagerSceneView.h>

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

WindowManagerSceneView::WindowManagerSceneView(osg::State* state, int screenWidth, int screenHeight) {
	m_ScreenWidth = screenWidth;
	m_ScreenHeight = screenHeight;
	m_Group = new osg::Group;

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

WindowManagerSceneView::~WindowManagerSceneView() {
}

Control* WindowManagerSceneView::getControlAtPosition(int x, int y) {
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

void WindowManagerSceneView::onUpdate(float /*dt*/) {
}

void WindowManagerSceneView::onRender() {
	m_View->update();
	m_View->cull();
	m_View->draw();
}

osgUtil::SceneView* WindowManagerSceneView::getSceneView() {
	return m_View.get();
}

} // namespace wf
} // namespace csp
