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

#include "ScreenInfo.h"
#include "CSPSim.h"
#include "VirtualBattlefield.h"
#include "VirtualScene.h"

#include <iomanip>
#include <sstream>

using std::max;
using std::min;
using std::setprecision;
using std::setw;
using std::fixed;
using std::setfill;

/*
  NOTE: as of OSG 0.9.4, updating text is a costly operation since
  the text class redoes the complete layout from scratch, glyph by
  glyph.  in principle we don't need to update after every frame, or
  perhaps can implement our own simplified version of osgText::Text
  for single lines without kerning, and cache the portions that don't
  change.  Actually, the latter could be done within the current
  implementation by having both static text nodes for labels and 
  dynamic nodes for values that change frequently.
*/
class UpdateCallback : public osg::NodeCallback
{
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{ 
		ScreenInfo* aScreenInfo = dynamic_cast<ScreenInfo*>(node);
		if (aScreenInfo && aScreenInfo->getStatus()) {
			aScreenInfo->update();
		}
		traverse(node,nv);
	}
};

ScreenInfo::ScreenInfo(int posx,int posy, std::string const & name, std::string const & text):
	m_TTFPath("ltype.ttf"),m_CharacterSize(11),m_FontSize(15), m_Text(new osgText::Text)
{
	m_Text->setFont(m_TTFPath);
	m_Text->setFontSize(m_FontSize, m_FontSize);
	m_Text->setCharacterSize(m_CharacterSize, 1.0);
	m_Text->setPosition(osg::Vec3(posx, posy - m_CharacterSize, 0));
	setUpdateCallback(new UpdateCallback());
	m_Text->setText(text);
	addDrawable(m_Text);

	setName(name);
}

Framerate::Framerate(int posx, int posy): ScreenInfo(posx, posy, "FRAMERATE"), m_minFps(60), m_maxFps(25), m_cumul(0.0)
{	
	m_Date = new osgText::Text();
	m_Date->setFont(m_TTFPath);
	m_Date->setFontSize(m_FontSize, m_FontSize);
	m_Date->setCharacterSize(m_CharacterSize, 1.0);
	m_Date->setPosition(osg::Vec3(posx, posy - 2 * m_CharacterSize, 0));
	addDrawable(m_Date);
}

void Framerate::update()
{
	static unsigned long count = 0;
	float fps = CSPSim::theSim->getFrameRate();
	
	if ((count++)%1000 == 0) { // reset occasionally
		m_minFps = 100.0;
		m_maxFps = 0.0;
	}
	m_minFps = min(m_minFps,fps);
	m_maxFps = max(m_maxFps,fps);
	m_cumul += fps;
	std::ostringstream osstr;
	osstr << setprecision(1) << setw(5) << fixed << fps << " FPS min: " << m_minFps << " max: " << m_maxFps << " av:"
		  << m_cumul / count;
	m_Text->setText(osstr.str());

	simdata::SimDate artificial_time = CSPSim::theSim->getCurrentTime();
	artificial_time.addTime(CSPSim::theSim->getScene()->getSpin());
	m_Date->setText("Date: " + artificial_time.asString());
}

GeneralStats::GeneralStats(int posx,int posy):ScreenInfo(posx,posy,"GENERAL STATS")
{
	float yOffset = 50.0 - m_CharacterSize;
	m_LocalPosition = new osgText::Text();
	m_LocalPosition->setFont(m_TTFPath);
	m_LocalPosition->setFontSize(m_FontSize, m_FontSize);
	m_LocalPosition->setCharacterSize(m_CharacterSize, 1.0);
	m_LocalPosition->setPosition(osg::Vec3(m_CharacterSize, yOffset, 0));
	addDrawable(m_LocalPosition);
	
	yOffset -= m_CharacterSize;
	m_GlobalPosition = new osgText::Text();
	m_GlobalPosition->setFont(m_TTFPath);
	m_GlobalPosition->setFontSize(m_FontSize, m_FontSize);
	m_GlobalPosition->setCharacterSize(m_CharacterSize, 1.0);
	m_GlobalPosition->setPosition(osg::Vec3(m_CharacterSize, yOffset, 0));
	addDrawable(m_GlobalPosition);
	
	yOffset -= m_CharacterSize;
	m_Velocity = new osgText::Text();
	m_Velocity->setFont(m_TTFPath);
	m_Velocity->setFontSize(m_FontSize, m_FontSize);
	m_Velocity->setCharacterSize(m_CharacterSize, 1.0);
	m_Velocity->setPosition(osg::Vec3(m_CharacterSize, yOffset, 0));
	addDrawable(m_Velocity);
}

void GeneralStats::update()
{
	unsigned short const precision = 2;

  	std::ostringstream osstr;
	osstr << "Terrain Polygons: " << CSPSim::theSim->getScene()->getTerrainPolygonsRendered();
	m_Text->setText(osstr.str());

	simdata::Ref<DynamicObject const> const activeObject = CSPSim::theSim->getActiveObject();
	if (activeObject.valid()) {
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

ObjectStats::ObjectStats(int posx,int posy, simdata::Ref<DynamicObject> const& activeObject):
ScreenInfo(posx,posy,"OBJECT STATS")
{
	if (activeObject.valid()) {
		std::vector<std::string> stringStats;
		activeObject->getStats(stringStats);
		short n = stringStats.size();
		for (;n-->0;) {
			posy -= m_CharacterSize;
			osgText::Text* aStat = new osgText::Text();
			aStat->setFont(m_TTFPath);
			aStat->setCharacterSize(m_CharacterSize, 1.0);
			aStat->setPosition(osg::Vec3(posx, posy, 0));
			m_ObjectStats.push_back(aStat);
			addDrawable(aStat);
		}
		if (m_Text) 
			removeDrawable(m_Text);	
	}
}

void ObjectStats::update() 
{
	simdata::Ref<DynamicObject> activeObject = CSPSim::theSim->getActiveObject();
	if (activeObject.valid()) {
		std::vector<std::string> stringStats;
		activeObject->getStats(stringStats);
		short n = m_ObjectStats.size();
		short m = stringStats.size();
		if (m < n) n = m;
		for (;--n>=0;)
			m_ObjectStats[n]->setText(stringStats[n]);
	}
}
