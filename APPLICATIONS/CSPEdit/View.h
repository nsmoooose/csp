// -*-c++-*- 
//
// CSPLayout - Copyright (C) 2003 Mark Rose
//
// Based on osgpick sample code
// OpenSceneGraph - Copyright (C) 1998-2003 Robert Osfield 
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, US


#include "FeatureNodes.h"

#include <osg/Group>
#include <string>
#include <vector>
#include <Producer/Mutex>

namespace osgProducer {
	class Viewer;
}


class View {
public:
	View();
	~View();
	int init(int argc, char **argv);
	void run();
	void quit();
	void lock() { m_Edit.lock(); }
	void unlock() { m_Edit.unlock(); }
	void add(FeatureNode *node) { m_Group.addChild(node); }
	void remove(FeatureNode *node) { m_Group.removeChild(node); }
	void setCamera(float x0, float y0, float z0, float x1, float y1, float z1);
	FeatureGroup *getRoot() { return &m_Group; }
	osgProducer::Viewer *getViewer() { return m_Viewer; }
private:
	//osg::ref_ptr<osg::Group> m_Group;
	osgProducer::Viewer *m_Viewer;
	FeatureGroup m_Group;
	Producer::Mutex m_Edit;
	bool m_Quit;
};


