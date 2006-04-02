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
 * @file MultiFunctionDisplay.h
 *
 **/


#ifndef __CSPSIM_F16_MULTIFUNCTIONDISPLAY_H__
#define __CSPSIM_F16_MULTIFUNCTIONDISPLAY_H__

#include <csp/cspsim/hud/Display.h>
#include <csp/cspsim/CockpitInterface.h>
#include <csp/cspsim/System.h>
#include <csp/cspsim/f16/F16System.h>
#include <csp/cspsim/hud/DisplayTools.h>
#include <csp/csplib/data/Vector3.h>
#include <sigc++/trackable.h>

CSP_NAMESPACE

namespace display { class Element; }

class DisplayFont;
class DisplayFormat;
class DisplayInterface;
class FormatIndex;
class OptionSelectButton;

class MultiFunctionDisplayPage: public System {
	friend class MultiFunctionDisplay;

public:
	CSP_DECLARE_ABSTRACT_OBJECT(MultiFunctionDisplayPage)

protected:
	virtual bool onOSB(int) { return false; }
};


// A helper class for laying out text and symbols in a display.
class DisplayLayout: public Referenced {
public:
	DisplayLayout(double width, double height, DisplayFont *font);
	~DisplayLayout();
	double height() const { return m_Height; }
	double width() const { return m_Width; }
	double rowOffset(double row, bool center=false) const;
	double colOffset(double col, bool center=false) const;
	double osbRowOffset(int index, double rel=0.0) const;
	double osbColOffset(int index, double rel=0.0) const;
	void osbAlignment(int index, double &x, double &y, osgText::Text::AlignmentType &alignment) const;
	DisplayFont *font() { return m_Font.get(); }
	DisplayFont const *font() const { return m_Font.get(); }
private:
	double m_OsbSpacing;
	double m_Width;
	double m_Height;
	osg::ref_ptr<DisplayFont> m_Font;
};

// A multiline text widget that supports drawing the text in reverse video.
class DisplayText: public display::Element {
public:
	typedef osg::ref_ptr<DisplayText> RefT;
	DisplayText(DisplayFont *font, double x, double y, osgText::Text::AlignmentType alignment);
	void setPosition(double x, double y);
	void setAlignment(osgText::Text::AlignmentType alignment);
	void setText(std::string const &text, bool invert=false);
	void setInvert(bool invert=true);
private:
	void reformat();
	osg::ref_ptr<DisplayFont> m_Font;
	osg::Vec4 m_Color;
	osg::Vec4 m_Black;
	display::SymbolMaker m_Box;
	std::vector<osg::ref_ptr<display::ElementText> > m_Lines;
	double m_PosX;
	double m_PosY;
	osgText::Text::AlignmentType m_Alignment;
	bool m_Invert;
	double m_BoxWidth;
	double m_MinimumWidth;
};

class TextPlacement {
public:
	virtual void get(DisplayLayout *layout, double &x, double &y, osgText::Text::AlignmentType &alignment) const = 0;
	virtual ~TextPlacement();
};

class PlaceOSB: public TextPlacement {
	unsigned m_Index;
	double m_ColOffset;
	double m_RowOffset;
public:
	PlaceOSB(unsigned index, double col_offset=0.0, double row_offset=0.0);
	virtual void get(DisplayLayout *layout, double &x, double &y, osgText::Text::AlignmentType &alignment) const;
};

class PlaceRowCol: public TextPlacement {
	double m_Row;
	double m_Col;
	osgText::Text::AlignmentType m_Alignment;
public:
	PlaceRowCol(double row, double col, osgText::Text::AlignmentType = osgText::Text::LEFT_BASE_LINE);
	virtual void get(DisplayLayout *layout, double &x, double &y, osgText::Text::AlignmentType &alignment) const;
};

class PlaceAbs: public TextPlacement {
	double m_X;
	double m_Y;
	osgText::Text::AlignmentType m_Alignment;
public:
	PlaceAbs(double x, double y, osgText::Text::AlignmentType = osgText::Text::LEFT_BASE_LINE);
	virtual void get(DisplayLayout *layout, double &x, double &y, osgText::Text::AlignmentType &alignment) const;
};


class MultiFunctionDisplay: public System, public sigc::trackable {
friend class DisplayInterface;
public:
	CSP_DECLARE_OBJECT(MultiFunctionDisplay)

	MultiFunctionDisplay();
	virtual ~MultiFunctionDisplay();

	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus*);

private:
	class State;

	virtual double onUpdate(double dt);
	virtual void buildMultiFunctionDisplay(Bus*);
	virtual void onMasterModeChange(f16::MasterMode const &);
	virtual void resetConfiguration();

	DisplayFormat *getActiveFormat();
	DisplayFormat *getSelectedFormat();
	void setSelectedFormat(std::string const &id);
	void setOverrideFormat(std::string const &id);

	DisplayText *osb(unsigned idx);
	void swap();

	DisplayInterface *getInterface();

	void onOSB(int idx);
	bool hasOtherState() const;
	State *getState();
	State *getOtherState();
	void exchangeState(Ref<State>&);
	void link(MultiFunctionDisplay*);
	FormatIndex *getFormats();
	void initFormats();

	Vector3 m_Color;
	std::string m_ChannelName;
	std::string m_EventPrefix;

	Ref<DisplayInterface> m_Interface;

	Ref<State> m_State;
	Ref<DisplayFormat> m_ActiveFormat;
	MultiFunctionDisplay* m_OtherDisplay;

	osg::ref_ptr<DisplayText> m_OSB[20];
	osg::ref_ptr<DisplayFont> m_StandardFont;
	osg::ref_ptr<osg::Group> m_PageDisplay;
	osg::ref_ptr<osg::Group> m_OverlayDisplay;
	osg::ref_ptr<display::Element> m_BreakX;

	Ref<DisplayLayout> m_Layout;
	DataChannel<Display>::RefT b_Display;
	f16::MasterModeSelection::CRefT b_MasterMode;
	DataChannel<bool>::CRefT b_AdvanceAltitudeAdvisory;
	std::string m_DefaultConfiguration;

	double m_ElapsedTime;
};


// Provides a limited interface by which a DisplayFormat may interact
// with its parent MFD.  Note that DisplayFormats may be reparented,
// so DisplayInterface is passed in as a method argument rather than
// stored by each format.
class DisplayInterface: public Referenced {
public:
	DisplayInterface(MultiFunctionDisplay *mfd): m_Display(mfd) { assert(mfd); }
	void setSelectedFormat(std::string const &id) { mfd()->setSelectedFormat(id); }
	void setOverrideFormat(std::string const &id) { mfd()->setOverrideFormat(id); }
	DisplayFormat *getSelectedFormat() { return mfd()->getSelectedFormat(); }
	DisplayFormat *getActiveFormat() { return mfd()->getActiveFormat(); }
protected:
	~DisplayInterface() {}
private:
	MultiFunctionDisplay *mfd() { return m_Display; }
	MultiFunctionDisplay *m_Display;
};


// Base class for all MFD pages.  Each pages has its own osg::Group
// that contains the symbols and text on that page.  These groups are
// swapped in and out of the scene graph as new pages are selected.
class BasePage: public Referenced {
public:
	BasePage(DisplayLayout *layout);
	virtual ~BasePage();
	virtual void importChannels(Bus*) { }
	osg::Group *display() { return m_Group.get(); }
protected:
	DisplayText *addText(TextPlacement const &placement);
	DisplayText *osb(unsigned index);
	DisplayLayout *layout() { return m_Layout.get(); }
private:
	Ref<DisplayLayout> m_Layout;
	osg::ref_ptr<osg::Group> m_Group;
	osg::ref_ptr<DisplayText> m_OSB[20];
};


// Base class for top-level MFD pages, corresponding to labels that
// appear in the LIST page and also above OSB's 12, 13, and 14.  A
// DisplayFormat may have many subpages that are managed internally.
// See StoresManagementPage for an example.
//
// TODO The DisplayInterface is only needed by the LIST and RESET
// override pages.  Is there an easy way to specialize those pages
// and generalize the base class?
class DisplayFormat: public BasePage {
public:
	DisplayFormat(DisplayLayout *layout);
	virtual ~DisplayFormat();
	virtual void onSelect(DisplayInterface*) { }
	virtual std::string id() const = 0;
	virtual std::string label() const { return id(); }
	virtual bool onOSB(int, DisplayInterface*) { return false; }
	virtual double update(double, DisplayInterface*) { return -1; };
};



// TODO make this class more useful: multiline alignment, offset,
// subselection, etc.
class OptionSelectButton: public display::Element {
public:
	enum Mode { NORMAL, BOX, INVERSE, UNCHANGED };
	OptionSelectButton(DisplayFont *font, double x, double y, osgText::Text::AlignmentType alignment, double margin=0.001);
	display::ElementText *text() { return m_Text.get(); }
	void setText(std::string const &text, Mode mode=UNCHANGED);
	void setText(std::string const &text, bool invert);
	void resize();
	Mode getMode() const { return m_Mode; }
	void setMode(Mode mode);
	void setMode(bool invert);

private:
	Mode m_Mode;
	double m_Margin;
	osg::Vec4 m_Color;
	osg::Vec4 m_Black;
	display::SymbolMaker m_Box;
	osg::ref_ptr<display::ElementText> m_Text;
};


CSP_NAMESPACE_END

#endif // __CSPSIM_F16_MULTIFUNCTIONDISPLAY_H__

