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

#include <osg/Depth>
#include <osg/MatrixTransform>

#include "ScreenInfoManager.h"

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



void ScreenInfoManager::set2dScene(osg::Group* rootNode, int ScreenWidth, int ScreenHeight)
{
    osg::ref_ptr<Framerate> framerate = new Framerate(0,ScreenHeight);
	//float textWidth = bitmapFont->getWidth(pause->getEncodedText());
	osg::ref_ptr<ScreenInfo> pause = new ScreenInfo(ScreenWidth - 5 * 8, ScreenHeight,"PAUSE", "PAUSE");
	pause->setStatus(false);
	osg::ref_ptr<GeneralStats> generalStats = new GeneralStats(0, ScreenHeight / 3);
	osg::ref_ptr<ObjectStats> objectStats = new ObjectStats(0, 2 * ScreenHeight / 3);

	rootNode->addChild(framerate.get());
	rootNode->addChild(pause.get());
	rootNode->addChild(generalStats.get());
	rootNode->addChild(objectStats.get());


    // now add a depth attribute to the scene to force it to draw on top.
    osg::Depth* depth = new osg::Depth;
    depth->setRange(0.0,0.0);
    
    osg::StateSet* rootState = new osg::StateSet();
    rootState->setAttribute(depth);
    rootState->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    
    rootNode->setStateSet(rootState);
}

class FindNamedNodeVisitor : public osg::NodeVisitor
{
    public :
        FindNamedNodeVisitor() : osg::NodeVisitor( TRAVERSE_ALL_CHILDREN )
        {
			setNodeMaskOverride(1);
            _found_node = 0L;
        }

        virtual void apply( osg::Node &node )
        {
            if( node.getName() == _name_to_find )
            _found_node = &node;
            traverse(node);
        }

        void setNameToFind( std::string const & name ) { _name_to_find = name; }
        osg::Node *foundNode() { return _found_node; }

    private :
        std::string _name_to_find;
        osg::Node *_found_node;
};

ScreenInfo * ScreenInfoManager::getScreenInfo(std::string const & name)
{
	FindNamedNodeVisitor nv;
	nv.setNameToFind(name);
	nv.apply( *m_modelview_abs );
	return dynamic_cast<ScreenInfo *>(nv.foundNode());
}

void ScreenInfoManager::setStatus(std::string const & name, bool bvisible)
{
	ScreenInfo * sci = getScreenInfo(name);
	if (sci)
		sci->setStatus(bvisible);
}

bool ScreenInfoManager::getStatus(std::string const & name)
{
	ScreenInfo * sci = getScreenInfo(name);
	if (sci)
		return sci->getStatus();
	else
		return false;
}
