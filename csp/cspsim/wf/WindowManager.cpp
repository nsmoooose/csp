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
#include <csp/cspsim/wf/WindowManager.h>
#include <csp/cspsim/wf/themes/Default.h>

#include <osg/Group>
#include <osgUtil/SceneView>
#include <osgUtil/IntersectVisitor>

CSP_NAMESPACE

namespace wf {

WindowManager::WindowManager(osgUtil::SceneView* view) : m_View(view) {
	m_Group = new osg::Group;
	m_View->setSceneData(m_Group.get());

    osg::StateSet *stateSet = m_Group->getOrCreateStateSet();
    stateSet->setRenderBinDetails(100, "RenderBin");
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
    
    m_Theme = new themes::Default;
}

WindowManager::~WindowManager() {
}

bool WindowManager::pick(int x, int y) {
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
				for (osg::NodePath::const_iterator iter = nearest.begin(); iter != nearest.end(); ++iter) {
					osg::Node *node = *iter;
					osg::NodeCallback *callback = node->getUpdateCallback();
					if (callback) {
						AnimationCallback *anim = dynamic_cast<AnimationCallback*>(callback);
						// TODO set flags for click type, possibly add position if we can determine a useful
						// coordinate system.
						if (anim && anim->pick(0)) return true;
					}
				}
			}
		}
	}
	return false;
}

void WindowManager::show(Window* window) {
	window->buildGeometry(this);
	m_Group->addChild(window->getNode());
	m_Windows.push_back(window);
}

void WindowManager::close(Window* window) {
	osg::Group* windowGroup = window->getNode();
	m_Group->removeChild(windowGroup);
	
	WindowVector::iterator iteratedWindow = m_Windows.begin();
	for(;iteratedWindow != m_Windows.end();++iteratedWindow) {
		if(iteratedWindow->get() == window) {
			m_Windows.erase(iteratedWindow);
			return;
		}
	}
}

const Theme& WindowManager::getTheme() const {
	return (*m_Theme.get());
}

} // namespace wf

CSP_NAMESPACE_END
