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

#include <osg/Texture2D>

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

// 6/18/03:
// 1) update is done every x frame - see void GameScreen::onUpdate(double)
// 2) Framerate & GeneralStats classes implement these static texts


class UpdateCallback : public osg::NodeCallback {
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) { 
		ScreenInfo* aScreenInfo = dynamic_cast<ScreenInfo*>(node);
		if (aScreenInfo && aScreenInfo->getStatus()) {
			aScreenInfo->update();
		}
		traverse(node,nv);
	}
};


ScreenInfo::ScreenInfo(float pos_x, float pos_y, std::string const &name, std::string const &text):
	m_TTFPath("screeninfo.ttf"),
	//m_TTFPath("ltype.ttf"),
	m_FontSize(20), 
	//m_CharacterSize(14),
	m_CharacterSize(11),
	m_Text(0) {
	m_Text = makeText(pos_x,pos_y - m_CharacterSize, text);
	addDrawable(m_Text);
	setName(name);
	// HACK to prevent text from disappearing when chunklod multitexture details
	// are turned on:
	osg::StateSet *ss = getOrCreateStateSet();
	ss->setTextureAttributeAndModes(1, new osg::Texture2D, osg::StateAttribute::OFF);
	//ss->setTextureAttributeAndModes(2, new osg::Texture2D, osg::StateAttribute::OFF);
	//ss->setTextureAttributeAndModes(3, new osg::Texture2D, osg::StateAttribute::OFF);
}

osgText::Text *ScreenInfo::makeText(float pos_x, float pos_y, std::string const &string_text) {
	osgText::Text *text = new osgText::Text;
	text->setFont(m_TTFPath);
	text->setFontSize(m_FontSize, m_FontSize);
	text->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	text->setCharacterSize(m_CharacterSize, 1.0);
	text->setPosition(osg::Vec3(pos_x, pos_y, 0));
	text->setText(string_text);
	return text;
}


Framerate::Framerate(int posx, int posy): ScreenInfo(posx, posy, "FRAMERATE"), m_minFps(60), m_maxFps(25), m_cumul(0.0) {

	m_Text->setUseDisplayList(false);

	// FIXME
	// this is clearly not the right thing to do, but just happens
	// to be too expedient to resist.  need to look more deeply into
	// the osg::Text implementation to figure out how to detect mono
	// spaced fonts and get the character width of a string prior to
	// and update/draw passes.
	float width = m_CharacterSize * 0.7;
	float spacing = 6 * width;

	float l = spacing;


	osgText::Text *s1_0 = makeText(posx + l, posy - m_CharacterSize, "fps min:");
	addDrawable(s1_0);
	
	l = s1_0->getBound().xMax() + width;

	m_MinFps = makeText(l, posy - m_CharacterSize);
	m_MinFps->setUseDisplayList(false);
	addDrawable(m_MinFps);

	l += spacing; 

	osgText::Text *s1_1 = makeText(l, posy - m_CharacterSize, "max:");
	addDrawable(s1_1);

	l = s1_1->getBound().xMax() + width;

	m_MaxFps = makeText(l, posy - m_CharacterSize);
	m_MaxFps->setUseDisplayList(false);
	addDrawable(m_MaxFps);

	l += spacing;

	osgText::Text *s1_2 = makeText(l, posy - m_CharacterSize, "avg:");
	addDrawable(s1_2);

	l = s1_2->getBound().xMax() + width;

	m_Av = makeText(l, posy - m_CharacterSize);
	m_Av->setUseDisplayList(false);
	addDrawable(m_Av);

	osgText::Text *s2 = makeText(posx, posy - 2 * m_CharacterSize, "Date:"); 
	addDrawable(s2);

	m_Date = makeText(s2->getBound().xMax() + width, posy - 2 * m_CharacterSize);
	m_Date->setUseDisplayList(false);
	addDrawable(m_Date);

	setUpdateCallback(new UpdateCallback);
}

void Framerate::update() {
	static unsigned long count = 0;
	float fps = CSPSim::theSim->getFrameRate();
		
	if ((++count)%1000 == 0) { // reset occasionally
		m_minFps = 100.0;
		m_maxFps = 0.0;
	}
	
	m_minFps = min(m_minFps,fps);
	m_maxFps = max(m_maxFps,fps);
	m_cumul += fps;

	std::ostringstream osstr;
	osstr << setprecision(1) << setw(5) << fixed << fps;
	m_Text->setText(osstr.str());

	osstr.str("");
	osstr << setprecision(1) << setw(5) << fixed << m_minFps;
	m_MinFps->setText(osstr.str());

	osstr.str("");
	osstr << setprecision(1) << setw(5) << fixed << m_maxFps;
	m_MaxFps->setText(osstr.str());
	
	osstr.str("");
	osstr << setprecision(1) << setw(5) << fixed << m_cumul / count;
	m_Av->setText(osstr.str());

	simdata::SimDate artificial_time = CSPSim::theSim->getCurrentTime();
	artificial_time.addTime(CSPSim::theSim->getScene()->getSpin());
	m_Date->setText(artificial_time.asString());
}


GeneralStats::GeneralStats(int posx,int posy):ScreenInfo(posx,posy,"GENERAL STATS") {
	osgText::Text *s1 = makeText(m_CharacterSize, posy,"Terrain Polygons:"); 
	addDrawable(s1);

	m_Text->setPosition(osg::Vec3(posx + 14.0 * m_CharacterSize, posy, 0.0));
	m_Text->setUseDisplayList(false);

	float yOffset = 50.0 - m_CharacterSize;

	osgText::Text *s2 = makeText(m_CharacterSize,yOffset,"Altitude:"); 
	addDrawable(s2);

	m_Altitude = makeText(8 * m_CharacterSize, yOffset);
	m_Altitude->setUseDisplayList(false);
	addDrawable(m_Altitude);

	yOffset -= m_CharacterSize;

	osgText::Text *s3 = makeText(m_CharacterSize,yOffset,"GlobalPosition:"); 
	addDrawable(s3);

	m_GlobalPosition = makeText(12 * m_CharacterSize, yOffset);
	m_GlobalPosition->setUseDisplayList(false);
	addDrawable(m_GlobalPosition);
	
	yOffset -= m_CharacterSize;

	osgText::Text *s4 = makeText(m_CharacterSize,yOffset,"Velocity:"); 
	addDrawable(s4);

	m_Velocity = makeText(8*m_CharacterSize, yOffset);
	m_Velocity->setUseDisplayList(false);
	addDrawable(m_Velocity);

	osgText::Text *s5 = makeText(m_CharacterSize+24*m_CharacterSize,yOffset,"magnitude:"); 
	addDrawable(s5);

	m_Magnitude = makeText(33*m_CharacterSize, yOffset);
	m_Magnitude->setUseDisplayList(false);
	addDrawable(m_Magnitude);

	setUpdateCallback(new UpdateCallback);
}

void GeneralStats::update() {
	unsigned short const precision = 2;

  	std::ostringstream osstr;
	osstr << CSPSim::theSim->getScene()->getTerrainPolygonsRendered();
	m_Text->setText(osstr.str());

	simdata::Ref<DynamicObject const> const activeObject = CSPSim::theSim->getActiveObject();
	if (activeObject.valid()) {
		simdata::Vector3 pos = activeObject->getGlobalPosition();
		osstr.str("");
		osstr << setprecision(precision) << fixed 
		      << setw(7 + precision) << setfill('0') << pos.z() - CSPSim::theSim->getBattlefield()->getGroundElevation(pos.x(),pos.y());
		m_Altitude->setText(osstr.str());

		osstr.str("");
		osstr << "(" << setprecision(precision) << fixed 
		      << setw(7 + precision) << setfill('0')  << pos.x() << ",";
		osstr << setw(7 + precision) << setfill('0') << pos.y() << "," << pos.z() << ")";
		m_GlobalPosition->setText(osstr.str());
	
		simdata::Vector3 vel = activeObject->getVelocity();
		osstr.str("");
		osstr << setprecision(2) << fixed << "(" << vel.x() << "," << vel.y() << "," << vel.z() << ")";
		m_Velocity->setText(osstr.str());

		osstr.str("");
		osstr << setprecision(2) << fixed << vel.length();
		m_Magnitude->setText(osstr.str());
	}
}


ObjectStats::ObjectStats(int posx,int posy, simdata::Ref<DynamicObject> const& activeObject)
	: ScreenInfo(posx,posy,"OBJECT STATS"), m_PosX(posx), m_PosY(posy) 
{
	m_Skip = static_cast<int>(m_CharacterSize);
	if (m_Text) {
		removeDrawable(m_Text);	
	}
	if (!getUpdateCallback()) {
		setUpdateCallback(new UpdateCallback);
	}
}

void ObjectStats::update() {
	simdata::Ref<DynamicObject> activeObject = CSPSim::theSim->getActiveObject();
	if (activeObject.valid()) {
		std::vector<std::string> stringStats;
		activeObject->getInfo(stringStats);
		short n = m_ObjectStats.size();
		short m = stringStats.size();
		if (m < n) {
			for (int i = m; i < n; i++) {
				removeDrawable(m_ObjectStats[i].get());
			}
			m_ObjectStats.resize(m);
		} else
		if (m > n) {
			m_ObjectStats.resize(m);
			for (int i = n; i < m; i++) {
				m_ObjectStats[i] = makeText(m_PosX, m_PosY+i*m_Skip);
				addDrawable(m_ObjectStats[i].get());
			}
		}
		while (--m >= 0) {
			m_ObjectStats[m]->setText(stringStats[m]);
		}
	}
}

