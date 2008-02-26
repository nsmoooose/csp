// CSPLayout
// Copyright 2003-2005 Mark Rose <mkrose@users.sourceforge.net>
//
// Based in part on osgpick sample code
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

#include <csp/tools/layout2/cpp/DynamicGrid.h>
#include <csp/tools/layout2/cpp/FeatureGraph.h>
#include <csp/tools/layout2/cpp/Scene.h>
#include <osg/Group>
// #include <osg/LightSource>

class csp::layout::Scene::Implementation {
public:
	osg::ref_ptr<osg::Group> m_RootNode;
	osg::ref_ptr<DynamicGrid> m_DynamicGrid;
	FeatureGraph* m_FeatureGraph;
	
	Implementation() : m_FeatureGraph(new FeatureGraph) {
		m_RootNode = new osg::Group;

		/*
		osg::ref_ptr<osg::LightSource> light = new osg::LightSource();
		light->setLight(new osg::Light());
		light->getLight()->setDiffuse(osg::Vec4(1,1,1,1));
		light->getLight()->setPosition(osg::Vec4(0,0,1,0));
		light->getLight()->setDirection(osg::Vec3(0,0,-1));
		light->getLight()->setLightNum(3);
		light->setLocalStateSetModes(osg::StateAttribute::ON);
		m_RootNode->addChild(light.get());
		*/

		m_RootNode->addChild(m_FeatureGraph->getScene().get());


		m_DynamicGrid = new DynamicGrid;
		m_RootNode->addChild(m_DynamicGrid.get());
	}

private:
};

csp::layout::Scene::Scene() : m_Implementation(new Implementation()) {
}

csp::layout::Scene::~Scene() {
	delete m_Implementation;
}

void csp::layout::Scene::createNewScene() {
}

osg::ref_ptr<osg::Group> csp::layout::Scene::getRootNode() {
	return m_Implementation->m_RootNode;
}

void csp::layout::Scene::updateDynamicGrid(osg::Vec3 target, osg::Vec3 cameraPosition) {
	m_Implementation->m_DynamicGrid->setLook(target, cameraPosition);
}

csp::layout::FeatureGraph* csp::layout::Scene::graph() {
	return m_Implementation->m_FeatureGraph;
}
