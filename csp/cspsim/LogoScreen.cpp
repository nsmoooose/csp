// Combat Simulator Project
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


#include <csp/cspsim/LogoScreen.h>
#include <csp/cspsim/Config.h>
#include <csp/cspsim/CSPSim.h>

#include <csp/csplib/util/FileUtility.h>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgDB/Registry>

namespace csp {

LogoScreen::LogoScreen() {
}

LogoScreen::~LogoScreen() {
}

class ImageUpdateCallback: public osg::NodeCallback {
	typedef std::vector<osg::ref_ptr<osg::Image> > ImageList;
	osg::ref_ptr<osg::Texture2D> m_Texture;
	double m_Delay;
	ImageList m_ImageList;
	ImageList::size_type m_CurrPos;
	double m_PrevTime;
	void setValue() {
		if (!m_ImageList.empty()) {
			m_Texture->setImage(m_ImageList[m_CurrPos].get());
		}
	}
public:
	ImageUpdateCallback(osg::Texture2D *texture, double delay):
		m_Texture(texture),
		m_Delay(delay),
		m_PrevTime(0.0)
	{
		std::string image_path = getDataPath("ImagePath");
		std::string logo_path = ospath::join(image_path, "logo");
		ospath::DirectoryContents dir = ospath::getDirectoryContents(logo_path);
		for (unsigned i = 0; i < dir.size(); ++i) {
			std::string ext = ospath::getFileExtension(dir[i]);
			if (osgDB::Registry::instance()->getReaderWriterForExtension(ext)) {
				std::string path = ospath::join("logo", dir[i]);
				osg::ref_ptr<osg::Image> image = osgDB::readImageFile(path);
				if (image.valid()) m_ImageList.push_back(image);
			}
		}
		if (!m_ImageList.empty()) {
			srand(static_cast<unsigned int>(time(0)));
			m_CurrPos = rand() % m_ImageList.size();
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
			if (!m_ImageList.empty()) {
				++m_CurrPos %= m_ImageList.size();
				setValue();
			}
			//}
		}
	}
};

void LogoScreen::onInit() {
	m_Camera = new osg::Camera;
	CSPSim::theSim->getSceneData()->addChild(m_Camera.get());

	m_Camera->setProjectionMatrixAsOrtho2D(-1.0, 1.0, -1.0, 1.0);
	m_Camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	m_Camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	m_Camera->setRenderOrder(osg::Camera::POST_RENDER);
	m_Camera->setAllowEventFocus(false);

	// create geometry
	osg::Geometry* geom = new osg::Geometry;

	// disable display list so our modified tex show up
	geom->setUseDisplayList(false);

	osg::Vec3Array* vertices = new osg::Vec3Array(4);
	(*vertices)[0] = osg::Vec3(1.0f, 1.0f , 0.0f);//top right
	(*vertices)[1] = osg::Vec3(1.0f, -1.0f, 0.0f);//bottom right
	(*vertices)[2] = osg::Vec3(-1.0f, -1.0f, 0.0f);//bottom left
	(*vertices)[3] = osg::Vec3(-1.0f, 1.0f, 0.0f);//top left

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

	osg::Geode* geode = new osg::Geode;
	geode->addDrawable(geom);
	geode->setUpdateCallback(new ImageUpdateCallback(m_Texture.get(),0.5));

	m_Camera->addChild(geode);
}

void LogoScreen::onExit() {
	CSPSim::theSim->getSceneData()->removeChild(m_Camera.get());
	m_Camera = 0;
	m_Texture = 0;
}

void LogoScreen::onUpdate(double /*dt*/) {
}


} // namespace csp

