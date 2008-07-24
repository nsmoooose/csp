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
 * @file ScreenInfoManager.cpp
 *
 **/

#include <csp/cspsim/ScreenInfoManager.h>

#include <osg/Depth>
#include <osg/MatrixTransform>
#include <osgText/Text>

namespace csp {

void set2dScene(osg::Group *root_node, int screen_width, int screen_height) {
	const unsigned short offsetpos = 11;
	osg::ref_ptr<Framerate> framerate = new Framerate(offsetpos, screen_height - offsetpos);
	osg::ref_ptr<ScreenInfo> record = new ScreenInfo(screen_width-15*offsetpos, screen_height-offsetpos, "RECORD", "RECORD");
	osg::ref_ptr<GeneralStats> generalStats = new GeneralStats(offsetpos, screen_height / 3);
	osg::ref_ptr<MessageList> messageBox = new MessageList(offsetpos, screen_height / 2, 4, 4.0);

	root_node->addChild(framerate.get());
	root_node->addChild(record.get());
	root_node->addChild(generalStats.get());
	root_node->addChild(messageBox.get());

	osg::StateSet *rootState = root_node->getOrCreateStateSet();
	rootState->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	rootState->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
}

ScreenInfoManager::ScreenInfoManager(int screen_width, int screen_height) {
	setMatrix(osg::Matrix::ortho2D(0, screen_width, 0, screen_height));

	m_modelview_abs = new osg::MatrixTransform;
	m_modelview_abs->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	m_modelview_abs->setMatrix(osg::Matrix::identity());

	set2dScene(m_modelview_abs, screen_width, screen_height);

	addChild(m_modelview_abs);
	setCullingActive(true);
}

void ScreenInfoManager::changeObjectStats(int /*screen_width*/, int screen_height, Ref<DynamicObject> const& vehicle)
{
	bool visible = getScreenInfo("GENERAL STATS")->getStatus();
	ScreenInfo* os = getScreenInfo("OBJECT STATS");
	if (os) {
		visible = os->getStatus();
		m_modelview_abs->removeChild(os);
	}
	osg::ref_ptr<ObjectStats> objectStats = new ObjectStats(12, 2 * screen_height / 3, vehicle);
	objectStats->setStatus(visible);
	m_modelview_abs->addChild(objectStats.get());
}

class FindNamedNodeVisitor: public osg::NodeVisitor {
	std::string m_NameToFind;
	osg::Node* m_FoundNode;
public:
	FindNamedNodeVisitor():
		osg::NodeVisitor(TRAVERSE_ALL_CHILDREN),
		m_FoundNode(0)
	{
		setNodeMaskOverride(0x1);
	}
	FindNamedNodeVisitor(const std::string& name):
		osg::NodeVisitor(TRAVERSE_ALL_CHILDREN),
		m_NameToFind(name),
		m_FoundNode(0)
	{
		setNodeMaskOverride(0x1);
	}
	virtual void apply (osg::Node& node) {
		if (node.getName() == m_NameToFind) {
			m_FoundNode = &node;
		} else {
			traverse(node);
		}
	}
	void setNameToFind(const std::string& name) {
		m_NameToFind = name;
	}
	osg::Node* foundNode() {
		return m_FoundNode;
	}
};

ScreenInfo* ScreenInfoManager::getScreenInfo(std::string const& name) {
	FindNamedNodeVisitor nv(name);
	m_modelview_abs->accept(nv);
	return dynamic_cast<ScreenInfo*>(nv.foundNode());
}

void ScreenInfoManager::setStatus(std::string const& name, bool visible) {
	ScreenInfo* sci = getScreenInfo(name);
	if (sci) sci->setStatus(visible);
}

bool ScreenInfoManager::getStatus(std::string const& name) {
	ScreenInfo *sci = getScreenInfo(name);
	if (sci) {
		return sci->getStatus();
	} else {
		return false;
	}
}

void ScreenInfoManager::addMessage(std::string const &line) {
	MessageList *mbox = dynamic_cast<MessageList*>(getScreenInfo("MESSAGE BOX"));
	if (mbox) {
		mbox->addLine(line);
	}
}

} // namespace csp

