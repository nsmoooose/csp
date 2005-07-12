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
 * @file ScreenInfo.h
 *
 **/


#ifndef __SCREENINFO_H__
#define __SCREENINFO_H__

namespace osgText {
	class Text;
}

#include <osg/Geode>
#include <osg/Switch>

#include <SimData/Ref.h>

class DynamicObject;

class ScreenInfo: public osg::Switch
//class ScreenInfo: public osg::Geode
{
protected:
	std::string m_TTFPath;
	int m_FontSize;
	float m_CharacterSize;
	osg::ref_ptr<osg::Geode> m_InfoGeode;
	osg::ref_ptr<osgText::Text> m_Text;
	osgText::Text *makeText(float pos_x, float pos_y,std::string const &string_text = "");
	virtual ~ScreenInfo() {}
public:
	ScreenInfo(float pos_x, float pos_y, std::string const &name, std::string const &text = "");
	virtual void update(){}
	void setStatus(bool const visible) {if (visible) setAllChildrenOn(); else setAllChildrenOff();}
	bool getStatus() const {return getValue(0);}
};


class Framerate: public ScreenInfo
{
	float m_MinFps, m_MaxFps, m_Cumul;
	osgText::Text* m_Date;
protected:
	~Framerate(){}
public:
	Framerate(int posx,int posy);
	virtual void update();
};


class GeneralStats: public ScreenInfo
{
	osgText::Text* m_Altitude;
	osgText::Text* m_GlobalPosition;
	osgText::Text* m_Velocity;
protected:
	~GeneralStats(){}
public:
	GeneralStats(int posx,int posy);
	virtual void update();
};


class ObjectStats: public ScreenInfo
{
	// input device informations
	std::vector<osg::ref_ptr<osgText::Text> > m_ObjectStats;
	int m_PosX;
	int m_PosY;
	int m_Skip;
protected:
	~ObjectStats(){}
public:
	ObjectStats(int posx,int posy, simdata::Ref<DynamicObject> const& activeObject);
	virtual void update();
};


class MessageList: public ScreenInfo
{
	std::vector<osg::ref_ptr<osgText::Text> > m_Messages;
	int m_Lines;
	float m_Delay;
	float m_Alpha;
	double m_LastUpdate;
protected:
	~MessageList(){}
public:
	MessageList(int posx, int posy, int lines, float delay);
	void addLine(std::string const &line);
	virtual void update();
};

#endif  // __SCREENINFO_H__

