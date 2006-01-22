// Combat Simulator Project
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file DataEntryDisplay.cpp
 *
 **/


#include <csp/cspsim/f16/DataEntryDisplay.h>
#include <csp/cspsim/f16/AlphaNumericDisplay.h>
#include <csp/cspsim/f16/SpecialFonts.h>
#include <csp/cspsim/hud/DisplayTools.h>
#include <csp/cspsim/hud/Text.h>

#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/osg.h>
#include <csp/csplib/util/StringTools.h>
#include <csp/csplib/data/ObjectInterface.h>

#include <osg/Geode>
#include <osgText/Text>


CSP_NAMESPACE

CSP_XML_BEGIN(DataEntryDisplay)
	CSP_DEF("color", m_Color, false)
	CSP_DEF("text_height", m_TextHeight, true)
	CSP_DEF("line_spacing", m_LineSpacing, true)
CSP_XML_END

using display::Element;
using display::ElementText;
using display::SymbolMaker;

class DEDStandardFont: public DisplayFont {
	float m_Height;
	osg::Vec4 m_Color;
	osg::ref_ptr<osgText::Font> m_Font;
public:
	DEDStandardFont(float height, osg::Vec4 const &color): m_Height(height), m_Color(color), m_Font(NULL) {
		m_Font = ReverseAltFont::load("hud.ttf");
	}
	virtual double getHeight() const { return m_Height; }
	virtual osgText::Font const *font() const { return m_Font.get(); }
	virtual void apply(osgText::Text *text) {
		text->setFont(m_Font.get());
		text->setFontResolution(30, 30);
		text->setColor(m_Color);
		text->setCharacterSize(m_Height);
		text->setKerningType(osgText::KERNING_NONE);
		text->setAxisAlignment(osgText::Text::XZ_PLANE);
	}
};

DataEntryDisplay::DataEntryDisplay(): m_Color(1, 1, 1), m_TextHeight(0.003), m_LineSpacing(0.0035), m_LineCount(0), m_Lines(0), m_DirtyCount(0), m_Display(new osg::Group) {
}

DataEntryDisplay::~DataEntryDisplay() {
	delete[] m_Lines;
}

void DataEntryDisplay::postCreate() {
	System::postCreate();
	m_StandardFont = new DEDStandardFont(m_TextHeight, osg::Vec4(m_Color.x(), m_Color.y(), m_Color.z(), 1.0));
}

void DataEntryDisplay::registerChannels(Bus* bus) {
	b_Display = bus->registerSharedDataChannel<Display>("DataEntryDisplay", Display());
	b_Display->value().model()->addChild(m_Display.get());
}

void DataEntryDisplay::importChannels(Bus* bus) {
	b_DEDReadout = bus->getChannel("DED", false);
	if (b_DEDReadout.valid() && b_DEDReadout->value().valid()) initDisplay();
}

void DataEntryDisplay::initDisplay() {
	assert(m_LineCount == 0);
	m_LineCount = b_DEDReadout->value()->height();
	CSPLOG(INFO, ALL) << "DataEntryDisplay::initDisplaye " << m_LineCount;
	m_Lines = new osg::ref_ptr<osgText::Text>[m_LineCount];
	for (unsigned i = 0; i < m_LineCount; ++i) {
		display::LabelElement *line = new display::LabelElement;
		m_StandardFont->apply(line->text());
		m_Lines[i] = line->text();
		m_Lines[i]->setPosition(osg::Vec3(0, 0, -m_LineSpacing * i));
		m_Display->addChild(line->geode());
	}
}

double DataEntryDisplay::onUpdate(double) {
	assert(!m_LineCount || m_LineCount == b_DEDReadout->value()->height());
	int count = b_DEDReadout->value()->getDirtyCount();
	if (m_DirtyCount != count) {
		m_DirtyCount = count;
		for (unsigned i = 0; i < m_LineCount; ++i) {
			m_Lines[i]->setText(b_DEDReadout->value()->getLine(i));
		}
	}
	return (m_LineCount > 0) ? 0.1 : -1;
}

CSP_NAMESPACE_END

