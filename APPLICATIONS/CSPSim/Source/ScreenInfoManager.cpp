// Combat Simulator Project - FlightSim Demo
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

#include "ScreenInfoManager.h"

#include <osg/Depth>
#include <osg/MatrixTransform>


void set2dScene(osg::Group *rootNode, int ScreenWidth, int ScreenHeight)
{
	const unsigned short offsetpos = 11;
	osg::ref_ptr<Framerate> framerate = new Framerate(offsetpos,ScreenHeight - offsetpos);
	osg::ref_ptr<ScreenInfo> pause = new ScreenInfo(ScreenWidth-5*offsetpos,ScreenHeight-offsetpos,"PAUSE", "PAUSE");
	osg::ref_ptr<GeneralStats> generalStats = new GeneralStats(offsetpos, ScreenHeight / 3);

	rootNode->addChild(framerate.get());
	rootNode->addChild(pause.get());
	rootNode->addChild(generalStats.get());

	osg::StateSet *rootState = rootNode->getOrCreateStateSet();
	rootState->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	rootState->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
}

ScreenInfoManager::ScreenInfoManager(int ScreenWidth, int ScreenHeight)
{
	setMatrix(osg::Matrix::ortho2D(0,ScreenWidth,0,ScreenHeight));

	m_modelview_abs = new osg::MatrixTransform;
	m_modelview_abs->setReferenceFrame(osg::Transform::RELATIVE_TO_ABSOLUTE);
	m_modelview_abs->setMatrix(osg::Matrix::identity());

	set2dScene(m_modelview_abs,ScreenWidth,ScreenHeight);

	addChild(m_modelview_abs);
	setCullingActive(true);
}

void ScreenInfoManager::changeObjectStats(int ScreenWidth, int ScreenHeight,simdata::Ref<DynamicObject> const& vehicle)
{
	ScreenInfo *os = getScreenInfo("OBJECT STATS");
	if (os)
		m_modelview_abs->removeChild(os);
	osg::ref_ptr<ObjectStats> objectStats = new ObjectStats(12, 2 * ScreenHeight / 3,vehicle);
	m_modelview_abs->addChild(objectStats.get());
}

class FindNamedNodeVisitor : public osg::NodeVisitor
{
    public :
        FindNamedNodeVisitor() : osg::NodeVisitor( TRAVERSE_ALL_CHILDREN )
        {
			setNodeMaskOverride(1);
            _found_node = 0L;
        }

        virtual void apply( osg::Node& node )
        {
            if( node.getName() == _name_to_find )
            _found_node = &node;
            traverse(node);
        }

        void setNameToFind( std::string const& name ) { _name_to_find = name; }
        osg::Node *foundNode() { return _found_node; }

    private :
        std::string _name_to_find;
        osg::Node *_found_node;
};

ScreenInfo* ScreenInfoManager::getScreenInfo(std::string const& name)
{
	FindNamedNodeVisitor nv;
	nv.setNameToFind(name);
    m_modelview_abs->accept(nv);
	return dynamic_cast<ScreenInfo*>(nv.foundNode());
}

void ScreenInfoManager::setStatus(std::string const& name, bool bvisible)
{
	ScreenInfo *sci = getScreenInfo(name);
	if (sci)
		sci->setStatus(bvisible);
}

bool ScreenInfoManager::getStatus(std::string const& name)
{
	ScreenInfo *sci = getScreenInfo(name);
	if (sci)
		return sci->getStatus();
	else
		return false;
}

