// Combat Simulator Project - CSPSim
// Copyright (C) 2002-2004 The Combat Simulator Project
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
 * @file LogoScreen.cpp
 *
 **/


# if defined(_MSC_VER) && (_MSC_VER <= 1200)
#pragma warning(disable : 4786)
# endif

#include "LogoScreen.h"

#include <ctime>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/LightSource>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgUtil/SceneView>

#include <SimData/FileUtility.h>

#include "Config.h"
#include "Exception.h"

extern OpenThreads::Barrier bar;

LogoScreen::LogoScreen(int width, int height) {
	m_width = width;
	m_height = height;
}

LogoScreen::~LogoScreen() {
	//cancel();
}

class ImageUpdateCallback: public osg::NodeCallback {
	typedef std::vector<osg::ref_ptr<osg::Image> > ImageList;
	osg::ref_ptr<osg::Texture2D> m_Texture;
	double m_Delay;
	ImageList m_ImageList;
	ImageList::size_type m_CurrPos;
	double m_PrevTime;
	void setValue() {
		m_Texture->setImage(m_ImageList[m_CurrPos].get());
	}
public:
	ImageUpdateCallback(osg::Texture2D *texture, double delay):
	m_Texture(texture),
	m_Delay(delay),
	m_PrevTime(0.0) {
		// serialized?
		typedef std::vector<std::string> StrVec;
		StrVec file_name;
		file_name.push_back("CSPLogo.bmp");
		file_name.push_back("moonrise1.png");
		file_name.push_back("crescent.png");
		file_name.push_back("sunlight2.png");
		file_name.push_back("sunset4.png");
		file_name.push_back("console2.png");
		file_name.push_back("TLabMultitex.jpeg");
		file_name.push_back("landing-2.png");
		file_name.push_back("vista.png");
		file_name.push_back("ground-fog.png");
		file_name.push_back("fx2.jpeg");

		std::string image_path = getDataPath("ImagePath");
		StrVec::const_iterator iEnd = file_name.end();
		for (StrVec::const_iterator i = file_name.begin();i!=iEnd;++i) {
			std::string path = simdata::ospath::join(image_path, *i);
			osg::ref_ptr<osg::Image> image = osgDB::readImageFile(path);
			if (!image.valid()) {
				std::string err = "Unable to load bitmap " + path;
				std::cerr << err << std::endl;
				throw csp::DataError(err);
			}
			else
				m_ImageList.push_back(image);
		}
		if (!file_name.empty()) {
			srand(static_cast<unsigned int>(time(0)));
			m_CurrPos = rand() % file_name.size();
			setValue();
		}
	}
	virtual void operator()(osg::Node*, osg::NodeVisitor *nv) {
		if (nv->getFrameStamp()) {
			double currTime = nv->getFrameStamp()->getReferenceTime();
			//if (currTime-m_PrevTime>m_Delay) {
			// record time
			m_PrevTime = currTime;
			// advance the current positon, wrap round if required.
			++m_CurrPos %= m_ImageList.size();
			setValue();
			//}
		}
	}
};

void LogoScreen::onInit() {
	//bar.block(2);
	
	m_LogoView = new osgUtil::SceneView();
	m_LogoView->setDefaults();
	m_LogoView->setViewport(0,0,m_width,m_height);
	float scale = 0.5f; 
	float w = scale * m_width, h = scale * m_height;
	m_LogoView->setProjectionMatrixAsOrtho2D(-w,w,-h,h);
	osg::FrameStamp *fs = new osg::FrameStamp();
	m_LogoView-> setFrameStamp(fs);

	// create geometry
	osg::Geometry* geom = new osg::Geometry;

	// disable display list so our modified tex show up
	geom->setUseDisplayList(false);

	scale = 0.5f * 1.0f;
	w = scale * m_width;
	h = scale * m_height;
	osg::Vec3Array* vertices = new osg::Vec3Array(4);
	(*vertices)[0] = osg::Vec3(w,h,0.0f);//top right
	(*vertices)[1] = osg::Vec3(w,-h,0.0f);//bottom right
	(*vertices)[2] = osg::Vec3(-w,-h,0.0f);//bottom left
	(*vertices)[3] = osg::Vec3(-w,h,0.0f);//top left

	//(*vertices)[0] = osg::Vec3(w,0.0f,h);//top right
	//(*vertices)[1] = osg::Vec3(w,0.0f,-h);//bottom right
	//(*vertices)[2] = osg::Vec3(-w,0.0f,-h);//bottom left
	//(*vertices)[3] = osg::Vec3(-w,0.0f,h);//top left
	geom->setVertexArray(vertices);
    
	osg::Vec2Array* texcoords = new osg::Vec2Array(4);
	(*texcoords)[3].set(0.0f, 1.0f);
	(*texcoords)[2].set(0.0f, 0.0f);
	(*texcoords)[1].set(1.0f, 0.0f);
	(*texcoords)[0].set(1.0f, 1.0f); 
	geom->setTexCoordArray(0,texcoords);

	osg::Vec4Array* colors = new osg::Vec4Array(1);
	(*colors)[0].set(1.0f,1.0f,1.0f,1.0f);
	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));

	m_Texture = new osg::Texture2D;

	// setup state
	osg::StateSet* stateset = geom->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(0, m_Texture.get(), osg::StateAttribute::ON);
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	geom->setStateSet(stateset);

	//getCamera(0)->setSyncBarrier(new Producer::RefBarrier);

	osg::Geode* geode = new osg::Geode;
	geode->addDrawable(geom);
	geode->setUpdateCallback(new ImageUpdateCallback(m_Texture.get(),0.5));

	m_LogoView->setSceneData(geode);

	//setUpViewer(osgProducer::Viewer::ViewerOptions::TRACKBALL_MANIPULATOR);
	//setUpViewer(osgProducer::Viewer::ViewerOptions::NO_EVENT_HANDLERS);
	
	//osg::ref_ptr<osg::Group> group = new osg::Group;
	//group->addChild(geode);
	//osg::MatrixTransform* mt = new osg::MatrixTransform();
	//const double a = 1;
	//mt->setMatrix(osg::Matrix::scale(a,a,a));
	//mt->addChild(osgDB::readNodeFile("cessna.osg"));
	//group->addChild(mt);

	//setSceneData(group.get());

	//osg::StateSet* ss = getGlobalStateSet();
	//osg::StateAttribute* sa = ss->getAttribute(osg::StateAttribute::LIGHT);
	//osg::Light* light = dynamic_cast<osg::Light*>(sa);

	//osg::LightSource* ls = dynamic_cast<osg::LightSource*>(getSceneDecorator());
	//osg::Light* light = static_cast<osg::Light*>(ls->getLight());

	//osg::Vec4 p = osg::Vec4(1,0,0,0);
	//light->setPosition(p);
	//light->setDirection(-osg::Vec3(p.x(),p.y(),p.z()));

	//const double b = 1500;
	//Producer::Matrix lookat;
	//lookat.makeLookAt(Producer::Vec3(0,-b,0),Producer::Vec3(0,0,0),Producer::Vec3(0,0,1));
	//setViewByMatrix(lookat);

	//Producer::RenderSurface* rs = getCamera(0)->getRenderSurface();
	//rs->setWindowName("CSPSim");
	//rs->setWindowRectangle(100,100,m_width,m_height);

	// create the windows and run the threads.
	//realize(ThreadPerCamera);
	//realize();
	//bar.block(2);
}

void LogoScreen::onExit() {
}	

void LogoScreen::onRender() {
	m_LogoView->cull();
	m_LogoView->draw();
}

void LogoScreen::onUpdate(double dt) {
	m_LogoView->update();
}

void LogoScreen::run() {
	/*static int i = 0;
	setDone(false);
    while (!done()) {
		//bar.block(2);
        // wait for all cull and draw threads to complete.
        sync();
        // update the scene by traversing it with the the update visitor which will
        // call all node update callbacks and animations.
		if (++i == 10) setDone(true);
        update();
        // fire off the cull and draw traversals of the scene.
        frame();
		//bar.block(2);
    }
    // wait for all cull and draw threads to complete before exit.
    sync();*/
}

void LogoScreen::stop() {
	//setDone(true);
}

































/*
#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <GL/gl.h>			// Header File For The OpenGL32 Library
#include <GL/glu.h>			// Header File For The GLu32 Library

#include <SDL/SDL.h>


LogoScreen::LogoScreen(int width, int height) 
{
	m_width = width;
	m_height = height;
}

LogoScreen::~LogoScreen()
{
}

void LogoScreen::onInit()
{
	std::string image_path = getDataPath("ImagePath");
	std::string path = simdata::ospath::join(image_path, "CSPLogo.bmp");
	m_image = SDL_LoadBMP(path.c_str());
	if (m_image == NULL) {
		throw csp::DataError("Unable to load bitmap " + path);
	}
}

void LogoScreen::onExit()
{
	if (m_image) {
		SDL_FreeSurface(m_image);
	}
}

void LogoScreen::onRender()
{
	if (!m_image) return;

	SDL_Rect src, dest;

	src.x = 0;
	src.y = 0;
	src.w = m_image->w;
	src.h = m_image->h;

	dest.x = 0;
	dest.y = 0;
	dest.w = m_image->w;
	dest.h = m_image->h;

	glViewport(0, 0, m_width, m_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, m_width, 0.0, m_height, -1.0, 1.0);

	// this centers the bitmap in case its a different size then the screen.
	glRasterPos2i( ((m_width - m_image->w) >> 1),
		     m_height - ((m_height - m_image->h) >> 1) );
	glPixelZoom(1.0,-1.0);

	glDrawPixels(m_image->w, m_image->h, GL_RGB, GL_UNSIGNED_BYTE, m_image->pixels);
}
*/

