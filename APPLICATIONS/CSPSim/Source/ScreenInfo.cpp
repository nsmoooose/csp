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
 * @file ScreenInfo.cpp
 *
 **/
#include <iomanip>
#include <sstream>

#include "CSPSim.h"
#include "ScreenInfo.h"
#include "VirtualBattlefield.h"
#include "VirtualScene.h"

using std::max;
using std::min;
using std::setprecision;
using std::setw;
using std::fixed;
using std::setfill;

class UpdateCallback : public osg::NodeCallback
{
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{ 
		ScreenInfo* aScreenInfo = dynamic_cast<ScreenInfo*>(node);
		if (aScreenInfo && aScreenInfo->getStatus()) 
			aScreenInfo->update();
		traverse(node,nv);
	}
};

ScreenInfo::ScreenInfo(int posx,int posy, std::string const & name, std::string const & text):
	m_TTFPath("ltype.ttf"),m_FontSize(11),
	m_BitmapFont(new osgText::BitmapFont(m_TTFPath,m_FontSize)),
	m_Text(new osgText::Text(m_BitmapFont))
{
	int heightFont = m_FontSize;
	m_Text->setPosition(osg::Vec3(posx,posy - heightFont,0));
	setUpdateCallback(new UpdateCallback());
	m_Text->setText(text);
	addDrawable(m_Text);

	setName(name);
}

Framerate::Framerate(int posx, int posy):ScreenInfo(posx,posy,"FRAMERATE"),m_minFps(60),m_maxFps(25) 
{	
	m_Date = new  osgText::Text(m_BitmapFont);
	m_Date->setPosition(osg::Vec3(posx,posy - 2 * m_FontSize,0));
	addDrawable(m_Date);
}

void Framerate::update()
{
	float fps = CSPSim::theSim->getFrameRate();
	static int i = 0;
	if (i++ >= 1000) { // reset occasionally
		m_minFps = 100.0;
		m_maxFps = 0.0;
		i = 0;
	}
	m_minFps = min(m_minFps,fps);
	m_maxFps = max(m_maxFps,fps);
	std::ostringstream osstr;
	osstr << setprecision(1) << setw(5) << fixed << fps << " FPS min: " << m_minFps << " max: " << m_maxFps;
	m_Text->setText(osstr.str());

	simdata::SimDate artificial_time = CSPSim::theSim->getCurrentTime();
	artificial_time.addTime(CSPSim::theSim->getScene()->getSpin());
	m_Date->setText("Date: " + artificial_time.asString());
}

GeneralStats::GeneralStats(int posx,int posy):ScreenInfo(posx,posy,"GENERAL STATS")
{
	unsigned int yOffset;

	yOffset = 50 - m_FontSize;
	m_LocalPosition = new  osgText::Text(m_BitmapFont);
	m_LocalPosition->setPosition(osg::Vec3(posx,yOffset,0));
	addDrawable( m_LocalPosition );
	
	yOffset -= m_FontSize;
	m_GlobalPosition = new  osgText::Text(m_BitmapFont);
	m_GlobalPosition->setPosition(osg::Vec3(posx,yOffset,0));
	addDrawable( m_GlobalPosition );
	
	yOffset -= m_FontSize;
	m_Velocity = new  osgText::Text(m_BitmapFont);
	m_Velocity->setPosition(osg::Vec3(posx,yOffset,0));
	addDrawable( m_Velocity );
}

void GeneralStats::update()
{
	unsigned short const precision = 2;

  	std::ostringstream osstr;
	osstr << "Terrain Polygons: " << CSPSim::theSim->getScene()->getTerrainPolygonsRendered();
	m_Text->setText(osstr.str());

	simdata::Pointer<DynamicObject const> const activeObject = CSPSim::theSim->getActiveObject();
	if (!activeObject.isNull()) {
		simdata::Vector3 pos = activeObject->getLocalPosition();
		osstr.str("");
		osstr << "LocalPosition: (" << setprecision(precision) << fixed 
		      << setw(6 + precision) << setfill('0')  << pos.x << ", ";
		osstr << setw(6 + precision) << setfill('0') << pos.y << ", ";
		osstr << setw(6 + precision) << setfill('0') << pos.z << ")";
		m_LocalPosition->setText(osstr.str());
		
		pos = activeObject->getGlobalPosition();
		osstr.str("");
		osstr << "GlobalPosition: (" << setprecision(precision) << fixed 
		      << setw(7 + precision) << setfill('0')  << pos.x << ", ";
		osstr << setw(7 + precision) << setfill('0') << pos.y << ", ";
		osstr << setw(7 + precision) << setfill('0') << pos.z << ")";
		m_GlobalPosition->setText(osstr.str());
	
		simdata::Vector3 vel = activeObject->getVelocity();
		osstr.str("");
		osstr << "Velocity: (" << setprecision(2) << fixed << vel.x << ", " << vel.y << ", " << vel.z << ")";
		m_Velocity->setText(osstr.str());
	}
}

ObjectStats::ObjectStats(int posx,int posy, simdata::Pointer<DynamicObject> const& activeObject):
ScreenInfo(posx,posy,"OBJECT STATS")
{
	if (!activeObject.isNull()) {
		std::vector<std::string> stringStats;
		activeObject->getStats(stringStats);
		short n = stringStats.size();
		for (;n-->0;) {
			posy -= m_FontSize;
			osgText::Text* aStat = new  osgText::Text(m_BitmapFont);
			aStat->setPosition(osg::Vec3(posx,posy,0));
			m_ObjectStats.push_back(aStat);
			addDrawable( aStat );
		}
		if (m_Text) 
			removeDrawable(m_Text);	
	}
}

void ObjectStats::update() 
{
	simdata::Pointer<DynamicObject> activeObject = CSPSim::theSim->getActiveObject();
	if (!activeObject.isNull()) {
		std::vector<std::string> stringStats;
		activeObject->getStats(stringStats);
		short n = m_ObjectStats.size();
		short m = stringStats.size();
		if (m < n) n = m;
		for (;--n>=0;) {
			m_ObjectStats[n]->setText(stringStats[n]);
			//			std::cout << stringStats[n] << std::endl;
		}
		//		std::cout << stringStats.size() << std::endl;
	}
}
