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
#include "Animation.h"
#include "CSPSim.h"
#include "DynamicObject.h"
#include "ObjectModel.h"
#include "VirtualScene.h"

#include <iomanip>
#include <sstream>

#include <osg/Texture2D>
#include <osg/StateSet>
#include <osgText/Text>

#include <SimData/Timing.h>

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
// 7/25/04
// this has changed again :)


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
	m_FontSize(20),
	//m_CharacterSize(14),
	m_CharacterSize(11),
	m_Text(makeText(pos_x,pos_y - m_CharacterSize, text)) {
		addDrawable(m_Text);
		setName(name);
		// HACK to prevent text from disappearing when chunklod multitexture details
		// are turned on:
		osg::StateSet *ss = getOrCreateStateSet();
		ss->setTextureMode(1,GL_TEXTURE_2D,osg::StateAttribute::OFF);
		//ss->setTextureMode(2,GL_TEXTURE_2D,osg::StateAttribute::OFF);
		//ss->setTextureMode(3,GL_TEXTURE_2D,osg::StateAttribute::OFF);
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


Framerate::Framerate(int pos_x, int pos_y):
	ScreenInfo(pos_x, pos_y, "FRAMERATE"),
	m_MinFps(99.0f),
	m_MaxFps(10.0f),
	m_Cumul(0.0f),
	m_Date(makeText(pos_x, pos_y - 2*m_CharacterSize)) {
		m_Text->setUseDisplayList(false);
		m_Date->setUseDisplayList(false);
		addDrawable(m_Date);
		setUpdateCallback(new UpdateCallback);
}

void Framerate::update() {
	static unsigned long count = 0;
	float fps = CSPSim::theSim->getFrameRate();
		
	if ((++count)%1000 == 0) { // reset occasionally
		m_MinFps = 99.0f;
		m_MaxFps = 10.0f;
	}
	
	m_MinFps = min(m_MinFps,fps);
	m_MaxFps = max(m_MaxFps,fps);
	m_Cumul += fps;

	std::stringstream line;
	line.precision(0);
	line.setf(std::ios::fixed);
	line << setw(3) << setfill(' ') << fps
		 << " fps min: "  << setw(3) << m_MinFps
		 << " max: " << setw(3) << m_MaxFps
		 << " avg: "  << m_Cumul / count;
	m_Text->setText(line.str());

	line.str("");
	simdata::SimDate artificial_time = CSPSim::theSim->getCurrentTime();
	artificial_time.addTime(CSPSim::theSim->getScene()->getSpin());
	line << artificial_time.asString();
	m_Date->setText(line.str());
}


GeneralStats::GeneralStats(int pos_x,int pos_y):
	ScreenInfo(pos_x,pos_y,"GENERAL STATS"),
	m_Altitude(makeText(pos_x, pos_y - 10*m_CharacterSize)),
	m_GlobalPosition(makeText(pos_x, pos_y - 11*m_CharacterSize)),
	m_Velocity(makeText(pos_x, pos_y - 12*m_CharacterSize)) {

	m_Text->setUseDisplayList(false);
	m_Altitude->setUseDisplayList(false);
	addDrawable(m_Altitude);
	m_GlobalPosition->setUseDisplayList(false);
	addDrawable(m_GlobalPosition);
	m_Velocity->setUseDisplayList(false);
	addDrawable(m_Velocity);

	setUpdateCallback(new UpdateCallback);
}

void GeneralStats::update() {
	unsigned short const precision = 2;

  	std::ostringstream osstr;
	osstr << "Terrain Polygons: " << setw(7) << CSPSim::theSim->getScene()->getTerrainPolygonsRendered();
	m_Text->setText(osstr.str());

	
	simdata::Ref<DynamicObject const> const activeObject = CSPSim::theSim->getActiveObject();
	if (activeObject.valid()) {
		simdata::Vector3 pos = activeObject->getGlobalPosition();
		double altitude = activeObject->getAltitude();
		osstr.str("");
		osstr << "Altitude: " << setprecision(precision) << fixed << setw(8) << altitude;
		m_Altitude->setText(osstr.str());

		osstr.str("");
		osstr << "Global position:" << setprecision(precision) << fixed << std::showpos
		      << setw(8)  << setfill(' ')  << pos;
		m_GlobalPosition->setText(osstr.str());
	
		simdata::Vector3 vel = activeObject->getVelocity();
		osstr.str("");
		osstr << "Velocity: " << setw(6) << setprecision(2) << setfill(' ') << fixed
			  << vel << " magnitude: " << std::noshowpos << vel.length();
		m_Velocity->setText(osstr.str());
	}
}


ObjectStats::ObjectStats(int posx,int posy, simdata::Ref<DynamicObject> const& /*activeObject*/)
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
			for (int i = m; i < n; ++i) {
				removeDrawable(m_ObjectStats[i].get());
			}
			m_ObjectStats.resize(m);
		} else
		if (m > n) {
			m_ObjectStats.resize(m);
			for (int i = n; i < m; ++i) {
				m_ObjectStats[i] = makeText(m_PosX, m_PosY+i*m_Skip);
				addDrawable(m_ObjectStats[i].get());
			}
		}
		while (--m >= 0) {
			m_ObjectStats[m]->setText(stringStats[m]);
		}
	}
}

MessageBox::MessageBox(int posx, int posy, int lines, float delay)
	: ScreenInfo(posx, posy, "MESSAGE BOX"), m_Lines(lines), m_Delay(delay), m_Alpha(1.0), m_LastUpdate(0)
{
	if (m_Text) {
		removeDrawable(m_Text);
	}
	int stepy = static_cast<int>(m_CharacterSize);
	for (int i = 0; i < lines; ++i) {
		osg::ref_ptr<osgText::Text> line = makeText(posx, posy - i * stepy);
		m_Messages.push_back(line);
		addDrawable(line.get());
	}
	if (!getUpdateCallback()) {
		setUpdateCallback(new UpdateCallback);
	}
}

void MessageBox::addLine(std::string const &line) {
	m_LastUpdate = simdata::get_realtime();
	for (int i = m_Lines-1; i > 0; --i) {
		m_Messages[i]->setText(m_Messages[i-1]->getText());
	}
	if (m_Lines > 0) {
		m_Messages[0]->setText(line);
	}
}

void MessageBox::update() {
	double now = simdata::get_realtime();
	float dt = static_cast<float>(now - m_LastUpdate);
	float old_alpha = m_Alpha;
	if (dt > m_Delay) {
		m_Alpha = std::max<float>(0, 1.0 - (dt - m_Delay));
	} else {
		m_Alpha = 1.0;
	}
	if (m_Alpha != old_alpha) {
		for (int i = 0; i < m_Lines; ++i) {
			m_Messages[i]->setColor(osg::Vec4(1, 1, 1, m_Alpha));
			if (m_Alpha == 0.0) {
				m_Messages[i]->setText("");
			}
		}
	}
}
