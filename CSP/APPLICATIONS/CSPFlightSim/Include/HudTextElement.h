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
 * @file HudTextElement.h
 *
 **/

#ifndef HUD_TEXT_ELEMENT_H
#define HUD_TEXT_ELEMENT_H

#include <fstream>

#include <osg/Vec4>
#include <osgText/Text>

#include "HudElement.h"

struct DrawableAppCallback;

// hud text element

/**
 * class HudTextElement - Describe me!
 *
 * @author unknown
 */
class HudTextElement : public HudElement, public osgText::Text
{
	typedef void (HudTextElement::*TypeSetValue)();
	typedef std::map< std::string, TypeSetValue > FuncMap;
	
	std::string m_sstaticString;
	char m_cstaticStringPosition;
	std::string m_fontPath;
	unsigned short m_fontSize;
	float m_fvalue;
	unsigned short m_usiformat;
	
	bool static m_initializedSetValue;      // initialization flag for SetValue functions map 
	FuncMap static m_SetValueSet;           // Set of setter functions
	
	virtual void SetValueHEAD();
	void SetValueAOA();
	void SetValueG();
	void SetValueKT();
	void SetValueMACH();
	void SetValueALTFT();
    void SetValueALTR();
    void SetValueALTS();
    void SetValueNWP();
    void SetValueWPH();
    void SetValueWPD();
    void SetValuePALT1();
    void SetValuePALT2();
    void SetValueRPM();
	void SetValue();
	void InitFuncMap();
    virtual void OnUpdate();
public:
	HudTextElement(){;};
	HudTextElement(std::string const & p_selementName, osg::Vec3 const & p_position, 
		std::string const & p_sstaticString, char const p_staticStringPosition = 'p', 
		std::string const & p_fontName = "ltypeb", unsigned short const p_fontSize = 16, 
		unsigned short const p_usiformat = 0);
	virtual ~HudTextElement();
	void OnUpdate(float p_fvalue);
	void OnUpdate(float p_fvalue, osg::Vec3 const & p_position);
	void OnUpdate(float p_fvalue, osg::Vec3 const & p_position, float p_fangle);

	void SetValue(float p_fvalue);
	void SetValue(std::string const & p_svalue);
	float GetValue() const;
	friend struct DrawableAppCallback;
};

#endif // __HUDTEXTELEMENT_H__


