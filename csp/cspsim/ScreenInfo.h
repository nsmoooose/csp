#pragma once
// Combat Simulator Project
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
 * @file ScreenInfo.h
 *
 **/

#include <csp/csplib/util/Ref.h>

namespace osgText {
	class Text;
}

#include <osg/Geode>
#include <osg/Switch>

namespace csp {

class DynamicObject;

class ScreenInfo: public osg::Switch {
protected:
	std::string m_TTFPath;
	int m_FontSize;
	float m_CharacterSize;
	osg::ref_ptr<osg::Geode> m_InfoGeode;
	osg::ref_ptr<osgText::Text> m_Text;
	osgText::Text *makeText(float pos_x, float pos_y, std::string const &string_text = "");
public:
	ScreenInfo(float pos_x, float pos_y, std::string const &name, std::string const &text = "");
	virtual void update() { }
	void setStatus(bool const visible) { if (visible) setAllChildrenOn(); else setAllChildrenOff(); }
	bool getStatus() const { return getValue(0); }
protected:
	virtual ~ScreenInfo();
};


class Framerate: public ScreenInfo {
	float m_MinFps, m_MaxFps, m_Cumul;
	osg::ref_ptr<osgText::Text> m_Date;
public:
	Framerate(int posx, int posy);
	virtual void update();
protected:
	virtual ~Framerate();
};


class GeneralStats: public ScreenInfo {
	osg::ref_ptr<osgText::Text> m_Altitude;
	osg::ref_ptr<osgText::Text> m_GlobalPosition;
	osg::ref_ptr<osgText::Text> m_Velocity;
public:
	GeneralStats(int posx, int posy);
	virtual void update();
protected:
	virtual ~GeneralStats();
};


class ObjectStats: public ScreenInfo {
	// input device informations
	std::vector<osg::ref_ptr<osgText::Text> > m_ObjectStats;
	int m_PosX;
	int m_PosY;
	int m_Skip;
public:
	ObjectStats(int posx, int posy, Ref<DynamicObject> const& activeObject);
	virtual void update();
protected:
	virtual ~ObjectStats();
};


class MessageList: public ScreenInfo {
	std::vector<osg::ref_ptr<osgText::Text> > m_Messages;
	int m_Lines;
	float m_Delay;
	float m_Alpha;
	double m_LastUpdate;
public:
	MessageList(int posx, int posy, int lines, float delay);
	void addLine(std::string const &line);
	virtual void update();
protected:
	virtual ~MessageList();
};

} // namespace csp
