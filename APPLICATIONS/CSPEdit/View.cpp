// -*-c++-*- 
//
// CSPEdit - Copyright (C) 2003 Mark Rose
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


#include "View.h"
#include "PickHandler.h"

#include <Python.h>
#include <unistd.h>

#include <osgUtil/Optimizer>
#include <osgDB/ReadFile>
#include <osgProducer/Viewer>

#include <osg/Material>
#include <osg/Geode>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Projection>
#include <osg/MatrixTransform>

#include <iostream>
#include <cstdio>


View::View(): m_Viewer(0) {
}

int View::init(int argc, char **argv) {
	// use an ArgumentParser object to manage the program arguments.
	osg::ArgumentParser arguments(&argc, argv);

	// set up the usage document, in case we need to print out how to use this program.
	std::string description, usage;
	description = arguments.getApplicationName() +
		      " is a utility for composing theater features.";
	usage = arguments.getApplicationName() +
		" [options]";
	arguments.getApplicationUsage()->setDescription(description);
	arguments.getApplicationUsage()->setCommandLineUsage(usage);
	arguments.getApplicationUsage()->addCommandLineOption("-h or --help","Display this information");
	// construct the viewer.
	m_Viewer = new osgProducer::Viewer(arguments);

	// set up the value with sensible default event handlers.
	m_Viewer->setUpViewer( osgProducer::Viewer::STANDARD_SETTINGS & 
	                      ~osgProducer::Viewer::ESCAPE_SETS_DONE);

	// get details on keyboard and mouse bindings used by the viewer.
	m_Viewer->getUsage(*arguments.getApplicationUsage());

	// if user request help write it out to cout.
	if (arguments.read("-h") || arguments.read("--help")) {
		arguments.getApplicationUsage()->write(std::cout);
		return 1;
	}

	// any option left unread are converted into errors to write out later.
	arguments.reportRemainingOptionsAsUnrecognized();

	// report any errors if they have occured when parsing the program aguments.
	if (arguments.errors()) {
		arguments.writeErrorMessages(std::cout);
		return 1;
	}

	// add the handler for doing the picking
	m_Viewer->getEventHandlerList().push_front(new PickHandler(this));

	m_Viewer->setSceneData(m_Group.getGraph());

	// default to windowed
	Producer::CameraConfig* cfg = m_Viewer->getCameraConfig();
	for( unsigned int i = 0; i < cfg->getNumberOfCameras(); ++i ) {
		Producer::Camera *cam = cfg->getCamera(i);
		Producer::RenderSurface* rs = cam->getRenderSurface();
		if (rs->isFullScreen()) {
			#ifndef WIN32                    
			rs->useBorder(true);
			rs->setWindowRectangle(240,220,800,600);
			#else
			rs->fullScreen(false);
			rs->setWindowRectangle(240,220,800,600);
			#endif
		}
	}

	return 0;
}

View::~View() {
	if (m_Viewer) delete m_Viewer;
}

void View::run() {
	if (!m_Viewer) return;
	m_Quit = false;
	m_Viewer->realize();
	while (!m_Viewer->done() && !m_Quit) {
		m_Viewer->sync();
		m_Viewer->update();
		m_Viewer->frame();
		Py_BEGIN_ALLOW_THREADS;
		unlock();
		usleep(100);
		lock();
		Py_END_ALLOW_THREADS;
	}
	m_Viewer->sync();
	delete m_Viewer;
	m_Viewer = 0;
}

void View::quit() {
	m_Quit = true;
}

void View::setCamera(float x0, float y0, float z0, float x1, float y1, float z1) {
	Producer::Vec3 from(x0, y0, z0);
	Producer::Vec3 to(x1, y1, z1);
	Producer::Vec3 up = (to-from)^((to-from)^Producer::Vec3(0,0,1));
	m_Viewer->getCamera(0)->setViewByLookat(from, to, up);
}

int main(int argc, char **argv) {
	View app;
	app.init(argc, argv);
	app.run();
	return 0;
}
