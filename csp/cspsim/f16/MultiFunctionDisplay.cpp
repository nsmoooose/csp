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
 * @file MultiFunctionDisplay.cpp
 *
 **/


#include <csp/cspsim/f16/MultiFunctionDisplay.h>
#include <csp/cspsim/f16/SpecialFonts.h>
#include <csp/cspsim/f16/StoresManagementPages.h>
#include <csp/cspsim/hud/DisplayTools.h>
#include <csp/cspsim/hud/Text.h>
#include <csp/cspsim/InputEventChannel.h>

#include <csp/csplib/util/osg.h>
#include <csp/csplib/util/StringTools.h>
#include <csp/csplib/data/ObjectInterface.h>

#include <osg/Geode>
#include <osg/Material>
#include <osg/StateSet>
#include <osgText/Text>
#include <osgText/Font>


CSP_NAMESPACE

CSP_XML_BEGIN(MultiFunctionDisplay)
	CSP_DEF("channel", m_ChannelName, true)
	CSP_DEF("event_prefix", m_EventPrefix, true)
	CSP_DEF("color", m_Color, false)
	CSP_DEF("default_configuration", m_DefaultConfiguration, false)
CSP_XML_END

using hud::Display;
using hud::DisplayFont;
using hud::display::Element;
using hud::display::ElementText;
using hud::display::SymbolMaker;


DisplayLayout::DisplayLayout(double width, double height, DisplayFont *font): m_Width(width), m_Height(height), m_Font(font) {
	CSP_VERIFY(font);
	m_OsbSpacing = 0.154;
}

DisplayLayout::~DisplayLayout() {
}

double DisplayLayout::rowOffset(double row, bool center) const {
	return (center ? 0.0 : (row < 0 ? -0.5 : 0.5) * m_Height) - row * m_Font->getHeight();
}

double DisplayLayout::colOffset(double col, bool center) const {
	return (center ? 0.0 : (col >= 0 ? -0.5 : 0.5) * m_Width) + col * m_Font->getWidth();
}

double DisplayLayout::osbRowOffset(int index, double rel) const {
	double offset = 0.0;
	if (index >= 0 && index < 5) {
		offset = (2 - (index % 5)) * m_OsbSpacing * m_Height - rel * m_Font->getHeight();
	}
	return offset;
}

double DisplayLayout::osbColOffset(int index, double rel) const {
	double offset = 0.0;
	if (index >= 0 && index < 5) {
		offset = ((index % 5) - 2) * m_OsbSpacing * m_Width + rel * m_Font->getWidth();
	}
	return offset;
}

void DisplayLayout::osbAlignment(int index, double &x, double &y, osgText::Text::AlignmentType &alignment) const {
	x = 0;
	y = 0;
	alignment = osgText::Text::CENTER_CENTER;
	double offset = ((index % 5) - 2) * m_OsbSpacing;
	int edge = (index / 5);
	switch (edge) {
		case 0:  // top
			x = offset * m_Width;
			y = 0.5 * m_Height;
			alignment = osgText::Text::CENTER_TOP;
			break;
		case 1:  // right
			x = 0.5 * m_Width;
			y = -offset * m_Height;
			alignment = osgText::Text::RIGHT_CENTER;
			break;
		case 2:  // bottom
			x = -offset * m_Width;
			y = -0.5 * m_Height;
			alignment = osgText::Text::CENTER_BOTTOM;
			break;
		case 3:  // left
			x = -0.5 * m_Width;
			y = offset * m_Height;
			alignment = osgText::Text::LEFT_CENTER;
			break;
	}
}


PlaceOSB::PlaceOSB(unsigned index, double col_offset, double row_offset) {
	m_Index = index;
	m_ColOffset = col_offset;
	m_RowOffset = row_offset;
}

void PlaceOSB::get(DisplayLayout *layout, double &x, double &y, osgText::Text::AlignmentType &alignment) const {
	CSP_VERIFY(layout);
	layout->osbAlignment(m_Index, x, y, alignment);
	x += layout->font()->getWidth() * m_ColOffset;
	y -= layout->font()->getHeight() * m_RowOffset;
}

PlaceRowCol::PlaceRowCol(double row, double col, osgText::Text::AlignmentType alignment) {
	m_Row = row;
	m_Col = col;
	m_Alignment = alignment;
}

void PlaceRowCol::get(DisplayLayout *layout, double &x, double &y, osgText::Text::AlignmentType &alignment) const {
	CSP_VERIFY(layout);
	x = layout->colOffset(m_Col);
	y = layout->rowOffset(m_Row);
	alignment = m_Alignment;
}

PlaceAbs::PlaceAbs(double x, double y, osgText::Text::AlignmentType alignment) {
	m_X = x;
	m_Y = y;
	m_Alignment = alignment;
}

void PlaceAbs::get(DisplayLayout *layout, double &x, double &y, osgText::Text::AlignmentType &alignment) const {
	CSP_VERIFY(layout);
	x = m_X;
	y = m_Y;
	alignment = m_Alignment;
}


namespace {

class MFDStandardFont: public DisplayFont {
	double m_Height;
	double m_Width;
	osg::Vec4 m_Color;
	osg::ref_ptr<osgText::Font> m_Font;
public:
	MFDStandardFont(double height, osg::Vec4 const &color): m_Height(height), m_Color(color), m_Font(NULL) {
		m_Font = osgText::readFontFile("hud.ttf");
		osg::ref_ptr<osgText::Text> M = new osgText::Text;
		M->setText("MMMMMMMMMM");
		apply(M.get());
		M->update();
		osg::BoundingBox bbox = M->getBound();
		m_Width = (bbox.xMax() - bbox.xMin()) * 0.1;
		CSPLOG(INFO, ALL) << "font width: " << m_Width << " " << m_Height << " " << (bbox.zMax() - bbox.zMin());
	}
	virtual double getHeight() const { return m_Height; }
	virtual double getWidth() const { return m_Width; }
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

} // namespace


DisplayText::DisplayText(DisplayFont *font, double x, double y, osgText::Text::AlignmentType alignment) {
	m_Font = font;
	m_PosX = x;
	m_PosY = y;
	m_Alignment = alignment;
	m_Black.set(0, 0, 0, 1);
	m_Invert = false;
	m_BoxWidth = 0;
	m_MinimumWidth = 0;
	addSymbol(m_Box);
}

void DisplayText::setPosition(double x, double y) {
	m_PosX = x;
	m_PosY = y;
	reformat();
}

void DisplayText::setAlignment(osgText::Text::AlignmentType alignment) {
	m_Alignment = alignment;
	reformat();
}

void DisplayText::setInvert(bool invert) {
	m_Invert = invert;
	reformat();
}

void DisplayText::setText(std::string const &text, bool invert) {
	TokenQueue tokens(text, "\n");
	m_Lines.reserve(tokens.size());
	size_t widest = 0;
	for (unsigned i = 0; i < tokens.size(); ++i) {
		widest = std::max(tokens[i].size(), widest);
		ElementText *text = (i < m_Lines.size()) ? m_Lines[i].get() : 0;
		if (!text) {
			text = new ElementText(this);
			m_Font->apply(text);
			m_Color = text->getColor();
			m_Lines.push_back(text);
		}
		text->setText(tokens[i]);
	}
	m_BoxWidth = std::max(widest * 1.0, m_MinimumWidth) * m_Font->getWidth();
	for (unsigned i = tokens.size(); i < m_Lines.size(); ++i) {
		geode()->removeDrawable(m_Lines[i].get());
	}
	m_Lines.resize(tokens.size());
	m_Invert = invert;
	reformat();
}

void DisplayText::reformat() {
	unsigned lines = m_Lines.size();
	osgText::Text::AlignmentType alignment = m_Alignment;
	double stride = m_Font->getHeight();
	double offset = 0.0;
	// special case osb alignments to use the baseline and an offset.  (todo: the
	// same for other non-baseline alignments.)
	switch (alignment) {
		case osgText::Text::CENTER_CENTER:
			alignment = osgText::Text::CENTER_BASE_LINE;
			offset = -0.3 + 0.5 * (lines-1);
			break;
		case osgText::Text::LEFT_CENTER:
			alignment = osgText::Text::LEFT_BASE_LINE;
			offset = -0.3 + 0.5 * (lines-1);
			break;
		case osgText::Text::RIGHT_CENTER:
			alignment = osgText::Text::RIGHT_BASE_LINE;
			offset = -0.3 + 0.5 * (lines-1);
			break;
		case osgText::Text::CENTER_TOP:
			alignment = osgText::Text::CENTER_BASE_LINE;
			offset = -0.8;
			break;
		case osgText::Text::CENTER_BOTTOM:
			alignment = osgText::Text::CENTER_BASE_LINE;
			offset = 0.2 + (lines-1);
			break;
		default:
			break;
	}
	for (unsigned i = 0; i < lines; ++i) {
		m_Lines[i]->setAlignment(alignment);
		m_Lines[i]->setPosition(m_PosX, m_PosY + (offset - i) * stride);
		m_Lines[i]->setColor(m_Invert ? m_Black : m_Color);
	}
	m_Box.erase();
	if (m_Invert && lines > 0) {
		double top = m_PosY + (offset + 0.8) * stride;
		double bottom = top - lines * stride;
		osg::BoundingBox bbox = m_Lines[0]->getBound();
		double left = 0.5 * (bbox.xMin() + bbox.xMax()) - 0.5 * m_BoxWidth;
		double right = 0.5 * (bbox.xMin() + bbox.xMax()) + 0.5 * m_BoxWidth;
		m_Box.addRectangle(left, bottom, right, top, true, m_Color);
	}
}


OptionSelectButton::OptionSelectButton(DisplayFont *font, double x, double y, osgText::Text::AlignmentType alignment, double margin): m_Margin(margin) {
	addSymbol(m_Box);
	m_Text = new ElementText(this);
	if (font) font->apply(m_Text);
	m_Text->setPosition(x, y);
	m_Text->setAlignment(alignment);
	m_Color = m_Text->getColor();
	m_Black.set(0, 0, 0, 1);
}

void OptionSelectButton::setText(std::string const &text, Mode mode) {
	m_Text->setText(text);
	if (text.empty()) {
		setMode((mode == UNCHANGED) ? m_Mode : mode);
	} else if (mode != UNCHANGED) {
		setMode(mode);
	}
}

void OptionSelectButton::setText(std::string const &text, bool invert) {
	m_Text->setText(text);
	setMode(invert);
}

void OptionSelectButton::setMode(bool invert) {
	setMode(invert ? INVERSE : NORMAL);
}

void OptionSelectButton::resize() {
	setMode(m_Mode);
}

void OptionSelectButton::setMode(Mode mode) {
	m_Mode = mode;
	// the bounding box for an empty string is a tiny box near the origin.
	// in general we don't want to draw it.
	bool empty = m_Text->getText().empty();
	m_Box.erase();
	osg::BoundingBox bbox = m_Text->getBound();
	bbox.xMin() -= m_Margin;
	bbox.zMin() -= m_Margin;
	bbox.xMax() += m_Margin;
	bbox.zMax() += m_Margin;
	switch (mode) {
		case NORMAL:
			// some of the mlu manual diagrams suggest that the normal text blocks out other overlapping
			// symbols, but this does not look good when the display surface is externally illuminated
			// (giving a black box on a gray background).
			//if (!empty) m_Box.addRectangle(bbox.xMin(), bbox.zMin(), bbox.xMax(), bbox.zMax(), true, m_Black);
			m_Text->setColor(m_Color);
			break;
		case BOX:
			if (!empty) m_Box.addRectangle(bbox.xMin(), bbox.zMin(), bbox.xMax(), bbox.zMax(), false, m_Color);
			m_Text->setColor(m_Color);
			break;
		case INVERSE:
			if (!empty) m_Box.addRectangle(bbox.xMin(), bbox.zMin(), bbox.xMax(), bbox.zMax(), true, m_Color);
			m_Text->setColor(osg::Vec4(0, 0, 0, 1));
			break;
		default: break;
	}
}


TextPlacement::~TextPlacement() {
}

BasePage::BasePage(DisplayLayout *layout): m_Layout(layout), m_Group(new osg::Group) {
	CSP_VERIFY(layout);
}

BasePage::~BasePage() {
}

DisplayText *BasePage::addText(TextPlacement const &placement) {
	double x = 0.0;
	double y = 0.0;
	osgText::Text::AlignmentType alignment = osgText::Text::LEFT_BASE_LINE;
	placement.get(layout(), x, y, alignment);
	DisplayText *text = new DisplayText(layout()->font(), x, y, alignment);
	m_Group->addChild(text->node());
	return text;
}

DisplayText *BasePage::osb(unsigned index) {
	CSP_VERIFY(index < 20);
	if (!m_OSB[index]) {
		m_OSB[index] = addText(PlaceOSB(index));
		m_Group->addChild(m_OSB[index]->node());
	}
	return m_OSB[index].get();
}

DisplayFormat::DisplayFormat(DisplayLayout *layout): BasePage(layout) {
}

DisplayFormat::~DisplayFormat() {
}


class DummyFormat: public DisplayFormat {
public:
	DummyFormat(DisplayLayout *layout, std::string const &id): DisplayFormat(layout), m_Id(id) { }
private:
	virtual std::string id() const { return m_Id; }
	virtual std::string label() const { return m_Id; }
	std::string m_Id;
};

class Blank: public DisplayFormat {
public:
	Blank(DisplayLayout *layout): DisplayFormat(layout) { }
private:
	virtual std::string id() const { return "BLANK"; }
	virtual std::string label() const { return "   "; }
};

class FormatSelectionMasterMenu: public DisplayFormat {
	static const char* const cLabels[];

public:
	FormatSelectionMasterMenu(DisplayLayout *layout): DisplayFormat(layout) { }
	virtual std::string id() const { return "SELECT"; }
	virtual std::string label() const { return ""; }
	virtual bool onOSB(int idx, DisplayInterface *di) {
		if (idx == 4) {
			di->setOverrideFormat("RESET_MENU");
			return true;
		}
		if (idx >= 10 && idx <= 14) return false;
		if (idx >= 0 && idx < 20 && *(cLabels[idx])) {
			di->setOverrideFormat("");
			di->setSelectedFormat(cLabels[idx]);
			return true;
		}
		return false;
	}
	virtual void onSelect(DisplayInterface *di) {
		for (unsigned i = 0; i < 20; ++i) {
			if (i >= 10 && i <= 14) continue;
			if (!cLabels[i][0]) continue;
			bool active = (cLabels[i] == di->getSelectedFormat()->id());
			osb(i)->setText(cLabels[i], active);
		}
	}
};

const char* const FormatSelectionMasterMenu::cLabels[] = {
	"BLANK", "", "", "RCCE", "RESET\nMENU",
	"SMS", "HSD", "DTE", "TEST", "FLCS",
	"", "", "", "", "",
	"FLIR", "TFR", "WPN", "TGP", "FCR"
};

class ResetMenu: public DisplayFormat {
	static const char* const cLabels[];

public:
	ResetMenu(DisplayLayout *layout): DisplayFormat(layout) { }
	virtual std::string id() const { return "RESET_MENU"; }
	virtual std::string label() const { return ""; }
	virtual bool onOSB(int idx, DisplayInterface *di) {
		if (idx == 4) {
			di->setOverrideFormat("SELECT");
			return true;
		}
		if (idx >= 10 && idx <= 14) return false;
		if (idx >= 0 && idx < 20 && *(cLabels[idx])) {
			di->setOverrideFormat("");
			di->setSelectedFormat(cLabels[idx]);
			return true;
		}
		return false;
	}
	virtual void onSelect(DisplayInterface *di) {
		for (unsigned i = 0; i < 20; ++i) {
			if (i >= 10 && i <= 14) continue;
			if (!cLabels[i][0]) continue;
			bool active = (cLabels[i] == di->getSelectedFormat()->id()) || (i == 4);
			osb(i)->setText(cLabels[i], active);
		}
	}
};

const char* const ResetMenu::cLabels[] = {
	"BLANK", "", "", "", "RESET\nMENU",
	"SBC DAY\n  RESET", "SBC NIGHT\n    RESET", "SBC DFLT\n   RESET", "SBC DAY\n    SET", "SBC NIGHT\n      SET",
	"", "", "", "", "",
	"", "", "NVIS\nOVRD", "PROG DCLT\nRESET", "MSMD\nRESET"
};


class FormatIndex: public Referenced {
public:
	void importChannels(Bus *bus) {
		for (FormatMap::iterator iter = m_FormatMap.begin(); iter != m_FormatMap.end(); ++iter) {
			iter->second->importChannels(bus);
		}
	}

	void addFormat(Ref<DisplayFormat> format) {
		assert(format.valid());
		CSPLOG(INFO, OBJECT) << "Adding MFD format id " << format->id();
		if (!m_FormatMap.insert(std::make_pair(format->id(), format)).second) {
			CSPLOG(ERROR, OBJECT) << "Duplicate MFD format id " << format->id();
		}
	}

	DisplayFormat *getFormat(std::string const &id) {
		FormatMap::iterator iter = m_FormatMap.find(id);
		if (iter == m_FormatMap.end()) {
			CSPLOG(ERROR, OBJECT) << "Could not find MFD format page " << id;
			return 0;
		}
		return iter->second.get();
	}

	DisplayFormat *getFormatOrBlank(std::string const &id) {
		DisplayFormat *format = getFormat(id);
		return format ? format : getBlank();
	}

	DisplayFormat *getBlank() {
		DisplayFormat *format = getFormat("BLANK");
		CSP_VERIFY(format);
		return format;
	}

private:
	typedef std::map<std::string, Ref<DisplayFormat> > FormatMap;
	FormatMap m_FormatMap;
};


// A representation of the internal state of an MFD, comprising the minimal
// amount of information needed to swap displays.  Individual display formats
// may contain additional state, but these are shared between the MFD's.
class MultiFunctionDisplay::State: public Referenced {
public:
	State(): m_Initialized(false), m_Dirty(true) {
	}

	void bindFormats(FormatIndex *index) {
		CSP_VERIFY(!m_FormatIndex);
		m_FormatIndex = index;
	}

	FormatIndex *getFormats() {
		return m_FormatIndex.get();
	}

	void resetConfiguration(std::string const &format0, std::string const &format1, std::string const &format2, unsigned index) {
		for (ModeMap::iterator iter = m_ModeMap.begin(); iter != m_ModeMap.end(); ++iter) {
			iter->second->setFormat(0, m_FormatIndex->getFormatOrBlank(format0));
			iter->second->setFormat(1, m_FormatIndex->getFormatOrBlank(format1));
			iter->second->setFormat(2, m_FormatIndex->getFormatOrBlank(format2));
			iter->second->setIndex(index);
		}
	}

	void setMode(f16::MasterMode const &mode) {
		// is this the correct way to handle SJ and EJ?  we essentially stay in the
		// previous master mode, but the SMS page updates accordingly.
		if (mode != f16::SJ && mode != f16::EJ) {
			ModeMap::iterator iter = m_ModeMap.find(mode);
			CSP_VERIFY(iter != m_ModeMap.end());
			m_ModeState = iter->second;
			m_OverrideFormat = 0;
			m_Initialized = true;
			m_Dirty = true;
		}
	}

	void initializeMode(f16::MasterMode const &mode) {
		CSP_VERIFY(!m_Initialized);
		CSP_VERIFY(m_FormatIndex.valid());
		ModeState *state = new ModeState;
		for (int i = 0; i < 3; ++i) {
			state->setFormat(i, m_FormatIndex->getBlank());
		}
		m_ModeMap[mode] = state;
	}

	bool isDirty() const {
		return m_Dirty;
	}

	void clearDirty() {
		m_Dirty = false;
	}

	void setDirty() {
		m_Dirty = true;
	}

	bool isOverridden() const {
		return m_OverrideFormat.valid();
	}

	void clearOverride() {
		m_OverrideFormat = 0;
		m_Dirty = true;
	}

	void removeDuplicateFormat(std::string const &id) {
		CSP_VERIFY(m_Initialized);
		for (unsigned idx = 0; idx < 3; ++idx) {
			if (m_ModeState->getFormat(idx)->id() == id) {
				m_ModeState->setFormat(idx, m_FormatIndex->getBlank());
				m_Dirty = true;
			}
		}
	}

	void setSelectedFormat(std::string const &id) {
		CSP_VERIFY(m_Initialized);
		DisplayFormat *format = m_FormatIndex->getFormatOrBlank(id);
		removeDuplicateFormat(format->id());
		m_ModeState->setFormat(m_ModeState->getIndex(), format);
		m_Dirty = true;
	}

	void setOverrideFormat(std::string const &id) {
		m_OverrideFormat = m_FormatIndex->getFormat(id);
	}

	DisplayFormat *getSelectedFormat() const {
		CSP_VERIFY(m_Initialized);
		return m_ModeState->getSelectedFormat();
	}

	DisplayFormat *getActiveFormat() const {
		CSP_VERIFY(m_Initialized);
		return (m_OverrideFormat.valid()) ? m_OverrideFormat.get() : getSelectedFormat();
	}

	void selectFormat(unsigned index) {
		CSP_VERIFY(m_Initialized);
		m_ModeState->setIndex(index);
		m_Dirty = true;
	}

	unsigned getSelectedFormatIndex() const {
		CSP_VERIFY(m_Initialized);
		return m_ModeState->getIndex();
	}

	DisplayFormat *getFormat(unsigned idx) const {
		CSP_VERIFY(m_Initialized);
		return m_ModeState->getFormat(idx);
	}

private:
	class ModeState: public Referenced {
	public:
		ModeState(): m_Index(0) { }
		unsigned getIndex() const { return m_Index; }
		void setIndex(unsigned index) {
			CSP_VERIFY(index < 3);
			m_Index = index;
		}
		DisplayFormat* getSelectedFormat() const {
			return m_Formats[m_Index].get();
		}
		DisplayFormat* getFormat(unsigned index) const {
			CSP_VERIFY(index < 3);
			return m_Formats[index].get();
		}
		void setFormat(unsigned index, DisplayFormat *format) {
			CSP_VERIFY(index < 3);
			m_Formats[index] = format;
		}
	private:
		unsigned m_Index;
		Ref<DisplayFormat> m_Formats[3];
	};

	typedef std::map<f16::MasterMode, Ref<ModeState> > ModeMap;
	ModeMap m_ModeMap;

	Ref<ModeState> m_ModeState;
	Ref<DisplayFormat> m_OverrideFormat;
	Ref<FormatIndex> m_FormatIndex;
	bool m_Initialized;
	bool m_Dirty;
};

void MultiFunctionDisplay::link(MultiFunctionDisplay* other) {
	CSP_VERIFY(other && other != this);
	m_OtherDisplay = other;
}

FormatIndex *MultiFunctionDisplay::getFormats() {
	return m_State->getFormats();
}

void MultiFunctionDisplay::exchangeState(Ref<State> &state) {
	Ref<State> temp = m_State;
	m_State = state;
	state = temp;
	m_State->setDirty();
	state->setDirty();
}

void MultiFunctionDisplay::swap() {
	if (m_OtherDisplay) {
		m_OtherDisplay->exchangeState(m_State);
	}
}

MultiFunctionDisplay::MultiFunctionDisplay(): m_Color(0.2, 1.0, 0.48), m_State(new State), m_OtherDisplay(0), m_PageDisplay(new osg::Group), m_OverlayDisplay(new osg::Group), m_ElapsedTime(0) {
	osg::Vec4 color(m_Color.x(), m_Color.y(), m_Color.z(), 1.0);
	m_Interface = new DisplayInterface(this);
	m_StandardFont = new MFDStandardFont(0.0055, color);
	m_Layout = new DisplayLayout(0.12, 0.12, m_StandardFont.get());
}

MultiFunctionDisplay::~MultiFunctionDisplay() {
}

// called only by the "master" mfd (i.e., the first one to register "mfd.link").
// all other linked mfds share the master copy rather than creating their own
// formats.
void MultiFunctionDisplay::initFormats() {
	DisplayLayout *layout = m_Layout.get();
	FormatIndex *index = new FormatIndex;
	index->addFormat(new Blank(layout));
	index->addFormat(new FormatSelectionMasterMenu(layout));
	index->addFormat(new ResetMenu(layout));
	index->addFormat(new f16::StoresManagementPage(layout));
	index->addFormat(new DummyFormat(layout, "FCR"));
	index->addFormat(new DummyFormat(layout, "DTE"));
	index->addFormat(new DummyFormat(layout, "TGP"));
	index->addFormat(new DummyFormat(layout, "WPN"));
	index->addFormat(new DummyFormat(layout, "TFR"));
	index->addFormat(new DummyFormat(layout, "FLIR"));
	index->addFormat(new DummyFormat(layout, "HSD"));
	index->addFormat(new DummyFormat(layout, "TEST"));
	index->addFormat(new DummyFormat(layout, "FLCS"));
	index->addFormat(new DummyFormat(layout, "RCCE"));
	m_State->bindFormats(index);
}

void MultiFunctionDisplay::registerChannels(Bus* bus) {
	// multiple-display discovery protocol.  currently assumes at most two displays
	// but could easily be extended by treating the first display to register the
	// link channel as the master.
	if (!bus->hasChannel("mfd.link")) {
		bus->registerSharedDataChannel<MultiFunctionDisplay*>("mfd.link", this);
		initFormats();
	}

	b_Display = bus->registerSharedDataChannel<Display>(m_ChannelName, Display());
	b_Display->value().model()->addChild(m_PageDisplay.get());
	b_Display->value().model()->addChild(m_OverlayDisplay.get());
	buildMultiFunctionDisplay(bus);
}

void MultiFunctionDisplay::resetConfiguration() {
	TokenQueue config(m_DefaultConfiguration);
	unsigned index = 0;
	for (unsigned i = 0; i < config.size(); ++i) {
		if (!config[i].empty() && config[i][0] == '*') {
			index = i;
			config[i].erase(0, 1);
		}
	}
	while (config.size() < 3) config.push_back("BLANK");
	m_State->resetConfiguration(config[0], config[1], config[2], index);
}

void MultiFunctionDisplay::importChannels(Bus* bus) {
	DataChannel<MultiFunctionDisplay*>::CRefT other = bus->getSharedChannel("mfd.link", false);
	if (other.valid() && other->value() != this) {
		m_OtherDisplay = other->value();
		CSP_VERIFY(m_OtherDisplay);
		m_OtherDisplay->link(this);
		m_State->bindFormats(m_OtherDisplay->getFormats());
	} else {
		m_State->getFormats()->importChannels(bus);
	}

	const std::vector<EnumLink> modes = f16::MasterModes.each();
	for (unsigned j = 0; j < modes.size(); ++j) {
		m_State->initializeMode(modes[j]);
	}

	resetConfiguration();

	b_MasterMode = bus->getSharedChannel("MasterMode", true);
	b_MasterMode->registerCallback(sigc::mem_fun(this, &MultiFunctionDisplay::onMasterModeChange));
	onMasterModeChange(b_MasterMode->mode());

	b_AdvanceAltitudeAdvisory = bus->getChannel("F16.GroundAvoidance.AdvanceAdvisory", false);
}

double MultiFunctionDisplay::onUpdate(double dt) {
	DisplayFormat *format = getActiveFormat();
	if (format != m_ActiveFormat.get()) {
		if (m_ActiveFormat.valid()) {
			m_PageDisplay->removeChild(m_ActiveFormat->display());
		}
		m_ActiveFormat = format;
		if (m_ActiveFormat.valid()) {
			m_PageDisplay->addChild(m_ActiveFormat->display());
			m_ActiveFormat->onSelect(getInterface());
		}
	}

	const bool advisory = b_AdvanceAltitudeAdvisory.valid() && b_AdvanceAltitudeAdvisory->value();
	m_BreakX->show(advisory && (static_cast<int>(m_ElapsedTime * 6.0) & 1));

	// TODO handle update interval
	m_ActiveFormat->update(dt, getInterface());
	if (m_State->isDirty()) {
		unsigned selected = m_State->getSelectedFormatIndex();
		for (unsigned idx = 0; idx < 3; ++idx) {
			bool is_active = (selected == idx);
			DisplayFormat *format = m_State->getFormat(idx);
			osb(13 - idx)->setText(format->label(), is_active);
		}
		m_State->clearDirty();
	}

	m_ElapsedTime += dt;
	return advisory ? 0.0 : 0.2;
}

DisplayInterface *MultiFunctionDisplay::getInterface() {
	assert(m_Interface.valid());
	return m_Interface.get();
}

void MultiFunctionDisplay::buildMultiFunctionDisplay(Bus *bus) {
	for (int i = 0; i < 20; ++i) {
		std::string label = stringprintf("%sOSB_%02d", m_EventPrefix, i);
		bus->registerChannel(new InputEventChannel(label.c_str(), this, sigc::bind(sigc::mem_fun(this, &MultiFunctionDisplay::onOSB), i)));
	}

	osb(14)->setText("SWAP");

	SymbolMaker pullup_cue;
	osg::Vec4 red(1, 0, 0, 1);
	pullup_cue.addLine(-0.045, -0.045, 0.045, 0.045, red);
	pullup_cue.addLine(0.045, -0.045, -0.045, 0.045, red);
	m_BreakX= new Element;
	m_BreakX->addSymbol(pullup_cue);
	m_BreakX->setLineWidth(2.5);
	m_BreakX->show(false);
	m_OverlayDisplay->addChild(m_BreakX->node());
}

void MultiFunctionDisplay::onOSB(int idx) {
	assert(idx >= 0 && idx < 20);
	if (idx == 14) swap();
	if (idx >= 11 && idx <= 13) {
		unsigned option = 13 - idx;
		if ((option == m_State->getSelectedFormatIndex()) || (m_State->getFormat(option)->id() == "BLANK")) {
			if (m_State->isOverridden()) {
				m_State->clearOverride();
			} else {
				if (hasOtherState()) getOtherState()->clearOverride();
				m_State->setOverrideFormat("SELECT");
			}
		} else {
			m_State->clearOverride();
		}
		m_State->selectFormat(option);
	}
	if (getActiveFormat()->onOSB(idx, getInterface())) return;
}

bool MultiFunctionDisplay::hasOtherState() const {
	return m_OtherDisplay != 0;
}

MultiFunctionDisplay::State *MultiFunctionDisplay::getState() {
	return m_State.get();
}

MultiFunctionDisplay::State *MultiFunctionDisplay::getOtherState() {
	return m_OtherDisplay ? m_OtherDisplay->getState() : 0;
}

DisplayFormat *MultiFunctionDisplay::getActiveFormat()  {
	return m_State->getActiveFormat();
}

DisplayFormat *MultiFunctionDisplay::getSelectedFormat()  {
	return m_State->getSelectedFormat();
}

void MultiFunctionDisplay::setSelectedFormat(std::string const &id) {
	m_State->setSelectedFormat(id);
	if (hasOtherState()) getOtherState()->removeDuplicateFormat(id);
}

void MultiFunctionDisplay::setOverrideFormat(std::string const &id) {
	m_State->setOverrideFormat(id);
	if (hasOtherState()) getOtherState()->clearOverride();
}

void MultiFunctionDisplay::onMasterModeChange(f16::MasterMode const &mode) {
	m_State->setMode(mode);
}

DisplayText *MultiFunctionDisplay::osb(unsigned index) {
	CSP_VERIFY(index < 20);
	if (!m_OSB[index]) {
		double x = 0.0;
		double y = 0.0;
		osgText::Text::AlignmentType alignment = osgText::Text::LEFT_BASE_LINE;
		PlaceOSB(index).get(m_Layout.get(), x, y, alignment);
		m_OSB[index] = new DisplayText(m_Layout->font(), x, y, alignment);
		//m_OSB[i] = new OptionSelectButton(m_StandardFont.get(), x, y, alignment);
		m_OverlayDisplay->addChild(m_OSB[index]->node());
	}
	return m_OSB[index].get();
}

CSP_NAMESPACE_END

