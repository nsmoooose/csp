// Combat Simulator Project
// Copyright (C) 2006 The Combat Simulator Project
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
 * @file StoresManagementPages.cpp
 *
 **/

#include <csp/cspsim/f16/StoresManagementPages.h>
#include <csp/cspsim/f16/Constants.h>
#include <csp/cspsim/hud/DisplayTools.h>
#include <csp/cspsim/hud/Text.h>
#include <csp/csplib/util/Math.h>
#include <csp/csplib/util/ScopedPointer.h>
#include <csp/csplib/util/StringTools.h>

#include <osg/Geode>
#include <osg/Switch>
#include <osgText/Text>

#include <sigc++/slot.h>

#include <iostream>  // XXX

// TODO RDY/SIM flag then master arm is added to the bus.

// From the MCM v5:
//
//   The FCC assumes the weapon(s) will fall in a certain trajectory until the AD or BA
//   conditions are met and the fuze functions to change the ballistics.  The values of
//   AD or BA in the SMS must be the same as the values set on the weapon fuze to ensure
//   accuracy.
//
// We need to decide how the SMS settings affect the weapon behavior, if at all.
// Allowing the SMS to modify the fuze before takeoff might be a reasonable compromise
// between realism and convenience.
//
// Another important point from the MCM:
//
//   If you are using a weapon such as CBU or Rockeye for instance, the SMS is
//   smart enough to know that the AD value you load is really what's on the
//   weapon. If you want the munition to function at a particular burst altitude
//   (BA) to optimize coverage, you need only load that altitude in the SMS and
//   the pull-up anticipation cue will function to let you know when you reach
//   that (minimum) altitude. In this sense, the pull-up anticipation cue is not
//   trying to satisfy a minimum fuze arm constraint, but is trying to indicate
//   the optimum release point to ensure the correct function altitude.
//
// The various CAT parameters and functions are detailed in Tables 5.1 and 5.2 on page 122.
//
// Some additional miscellaneous information from www.f-16.net:
//
//   "Gus did a good job explaining the arming options. You have three
//    solenoids, nose, center and tail. Nose and center work together always and
//    tail is by itself. So you only have three real options, NOSE, NSTL, TAIL."
//
//  "Tankrat is right, when the tanks are punched off the carts blow the attach bolt
//   which goes the whole way through the wing. The aft is held with a pivot fitting
//   which holds on to the tank long enough for the front to be moving down and away
//   then it comes free so they won't hit the stabs or ailerons. The two cannon plugs
//   have quick disconnects and pop right off.  The centerline is a different story,
//   you can just jett the tank."
//
//  "B: 600-Gallon Wing Tanks
//   The Advanced Block 50/52 variant is certified to carry the 600-gallon wing
//   fuel tanks. These tanks increase range or persistence up to thirty percent
//   over the standard 370-gallon wing tanks. The tanks are mounted on
//   non-jettisonable pylons that can also carry the more common 370-gallon tanks."

CSP_NAMESPACE

namespace f16 {

// TODO can this be made more generic?  what about making the map static?
class AirToGroundSubmodeLabels {
public:
	AirToGroundSubmodeLabels() {
		m_Labels[AirToGround::MANUAL] = "MAN";
		m_Labels[AirToGround::ULOFT] = "ULFT";
	}
	std::string get(AirToGround::Submode const &e) {
		std::map<AirToGround::Submode, std::string>::const_iterator iter = m_Labels.find(e);
		return (iter == m_Labels.end()) ? e.getToken() : iter->second;
	}
private:
	std::map<AirToGround::Submode, std::string> m_Labels;
};

struct BombProfile: public Referenced {
	BombProfile() { reset(); }
	void reset() {
		submode = AirToGround::CCRP;
		cat1_ad1 = 16.0;
		cat1_ad2 = 1.50;
		cat2_ad = 4.00;
		cat2_ba = 500;
		cat3_ad = 12.25;
		cat3_ba = 300;
		cat4_ad1 = 2.00;
		cat4_ad2 = 2.50;
		cat4_ba = 100;
		ladd_pr = 25000;
		ladd_tof = 28.00;
		ladd_mra = 1105;
		release_angle = 23;
		pair_release = false;
		impact_spacing = 175;
		release_pulses = 1;
		fuse_arming_option = NOSE;
		adba_selection = AD;
	}
	// todo fuse
	AirToGround::Submode submode;
	double cat1_ad1;
	double cat1_ad2;
	double cat2_ad;
	unsigned cat2_ba;
	double cat3_ad;
	unsigned cat3_ba;
	double cat4_ad1;
	double cat4_ad2;
	unsigned cat4_ba;
	unsigned ladd_pr;
	double ladd_tof;
	unsigned ladd_mra;
	unsigned release_angle;
	bool pair_release;
	unsigned impact_spacing;
	unsigned release_pulses;
	enum {NOSE, TAIL, NSTL} fuse_arming_option;
	enum {AD, BA} adba_selection;
};


// the sms page model is as follows.  StoresManagementPage is the active
// display format, but delegates rendering and osb handling to subpages
// (NestedPage subclasses).  StoresManagementPage creates subpage instances
// for each master mode, and those subpages in turn create (and own) additional
// subpages.  calls to onOSB can optionally return a subpage that should
// replace the active subpage for the current mastermode.

class NestedPage: public BasePage {
public:
	NestedPage(DisplayLayout* layout): BasePage(layout) { }
	virtual NestedPage *onOSB(unsigned index) = 0;
	virtual void activate() { }
	virtual double update(double) { return -1.0; }
};



// A display element for showing stores mounted on a hardpoint in the various
// inventory pages.  Supports highlighting selected stores.
class StationElement: public display::Element {
public:
	typedef osg::ref_ptr<StationElement> RefT;

	StationElement(unsigned lines, unsigned columns, double x, double y, DisplayFont *font): m_Selected(false), m_Black(0, 0, 0, 1) {
		CSP_VERIFY(font && lines > 0);
		m_Lines.resize(lines);
		for (unsigned i = 0; i < lines; ++i) {
			m_Lines[i] = new osgText::Text;
			font->apply(m_Lines[i].get());
			m_Lines[i]->setPosition(osg::Vec3(x, 0, y - i * m_Lines[i]->getCharacterHeight()));
			geode()->addDrawable(m_Lines[i].get());
		}
		double width = font->getWidth();
		double height = font->getHeight();
		double side_margin = 0.2;
		double top_offset = 0.8;
		double bottom_offset = 0.3;
		m_LineSpacing = height;
		m_SelectOffset = 0.3 * height;
		m_BoxBottom = y - (bottom_offset + lines - 1.0) * height;
		m_BoxTop = y + (top_offset) * height;
		m_BoxLeft = x - side_margin * width;
		m_BoxRight = x + (side_margin + columns) * width;
		m_Color = m_Lines[0]->getColor();
		m_LowSelection = 0;
		m_HighSelection = size();
		addSymbol(m_Box);
	}

	unsigned size() const { return m_Lines.size(); }

	void setLine(unsigned index, std::string const &text) {
		if (index < m_Lines.size()) m_Lines[index]->setText(text);
	}

	void select(bool on=true, unsigned line1 = 0, unsigned line2 = ~0) {
		m_LowSelection = clampTo(line1, 0U, size());
		m_HighSelection = clampTo(line2, m_LowSelection, size());
		m_Selected = on;
		m_Box.erase();
		if (on) {
			m_Box.addRectangle(m_BoxLeft, m_BoxTop - m_LowSelection * m_LineSpacing, m_BoxRight, m_BoxTop - m_SelectOffset - m_HighSelection * m_LineSpacing, true, m_Color);
			for (unsigned i = 0; i < m_Lines.size(); ++i) m_Lines[i]->setColor((i >= m_LowSelection && i < m_HighSelection) ? m_Black : m_Color);
		} else {
			for (unsigned i = 0; i < m_Lines.size(); ++i) m_Lines[i]->setColor(m_Color);
		}
	}

	bool isSelected() const { return m_Selected; }
	unsigned lowSelection() const { return m_LowSelection; }
	unsigned highSelection() const { return m_HighSelection; }

private:
	std::vector<osg::ref_ptr<osgText::Text> > m_Lines;
	bool m_Selected;
	display::SymbolMaker m_Box;
	osg::Vec4 m_Color;
	osg::Vec4 m_Black;
	double m_BoxLeft;
	double m_BoxTop;
	double m_BoxRight;
	double m_BoxBottom;
	double m_LineSpacing;
	double m_SelectOffset;
	unsigned m_LowSelection;
	unsigned m_HighSelection;
};


// The underlying stores data used by the SMS display.  This does not necessarily
// correspond to the actual loadout, and there is currently no way to synchronize
// it with the loadout other than manually via the MDDE pages (which aren't yet
// implemented either!).  The inventory data owned by the main SMS page and used
// when rendering the various INV and SJ pages.
class StoresInventory: public Referenced {
public:
	struct StoreData {
		StoreData(): count(0), can_jettison(false) { }
		StoreData(std::string const &label, int count, bool can_jettison): label(label), count(count), can_jettison(can_jettison) { }
		bool empty() const { return label.empty(); }
		std::string label;
		int count;
		bool can_jettison;
	};

	struct StationData {
		StoreData pylon;
		StoreData rack;
		StoreData store;
	};

	StationData const &getStation(unsigned idx) const {
		CSP_VERIFY(idx < 9);
		return m_StationData[idx];
	}

	void setStation(unsigned idx, StationData const &data) {
		CSP_VERIFY(idx < 9);
		m_StationData[idx] = data;
		m_DirtyCount++;
	}

	void setGunRounds(int rounds, std::string const &label = "") {
		m_GunRounds.count = rounds;
		if (!label.empty()) m_GunRounds.label = label;
		m_DirtyCount++;
	}

	StoreData const &getGunRounds() const {
		return m_GunRounds;
	}

	StoresInventory(): m_DirtyCount(0) {
		// test data
		m_StationData[0].pylon = StoreData("LNCH", 1, false);
		m_StationData[0].store = StoreData("A-9X", 1, false);
		m_StationData[1].pylon = StoreData("MAU", 1, false);
		m_StationData[1].store = StoreData("A120C", 1, false);
		m_StationData[2].pylon = StoreData("MAU", 1, false);
		m_StationData[2].store = StoreData("AG88", 1, false);
		m_StationData[3].pylon = StoreData("MAU", 1, false);
		m_StationData[3].rack = StoreData("TER", 1, true);
		m_StationData[3].store = StoreData("MK82", 3, true);
		m_StationData[4].pylon = StoreData("MAU", 1, false);
		m_StationData[4].store = StoreData("TK300", 1, true);
		m_StationData[5].pylon = StoreData("MAU", 1, false);
		m_StationData[5].rack = StoreData("TER", 1, true);
		m_StationData[5].store = StoreData("MK82", 3, true);
		m_StationData[6].pylon = StoreData("MAU", 1, false);
		m_StationData[6].store = StoreData("AG88", 1, false);
		m_StationData[7].pylon = StoreData("MAU", 1, false);
		m_StationData[7].store = StoreData("A120C", 1, false);
		m_StationData[8].pylon = StoreData("LNCH", 1, false);
		m_StationData[8].store = StoreData("A-9X", 1, false);
		m_GunRounds = StoreData("PGU28", 510, false);
	}

	int getDirtyCount() const {
		return m_DirtyCount;
	}

private:
	StationData m_StationData[9];
	StoreData m_GunRounds;
	int m_DirtyCount;
};


// A visual representation of the SMS inventory as displayed on the various INV and SJ
// pages.  Stations 1-9 are displayed from bottom-left to top-center to bottom-right,
// and for non-SJ displays the gun rounds are shown next to OSB 20.
class InventoryDisplay: public Referenced {
public:
	InventoryDisplay(StoresInventory *inventory, DisplayLayout *layout, bool jettison=false): m_Inventory(inventory), m_Group(new osg::Group), m_JettisonPage(jettison), m_DirtyCount(-1), m_SelectionCount(0) {
		initializeDisplay(layout);
	}

	osg::Group *group() {
		return m_Group.get();
	}

	void update() {
		if (m_DirtyCount < m_Inventory->getDirtyCount()) {
			updateDisplay();
		}
	}

	void toggleSelection(unsigned index) {
		CSP_VERIFY(index < 9);
		StationElement *station = m_Stations[index].get();
		const bool state = station->isSelected();
		if (isJettisonPage()) {
			const unsigned high = m_JettisonCount[index];
			const unsigned low = state ? station->lowSelection() : high;
			station->select(low > 0, low - 1, high);
		} else {
			station->select(!state);
			m_SelectionCount += (state ? -1 : 1);
		}
	}

	void clearSelections() {
		for (unsigned index = 0; index < 9; ++index) {
			m_Stations[index]->select(false);
		}
		m_SelectionCount = 0;
	}

	bool hasSelection() const {
		return m_SelectionCount > 0;
	}

	bool isJettisonPage() const {
		return m_JettisonPage;
	}

private:
	void initializeDisplay(DisplayLayout *layout) {
		CSP_VERIFY(layout && !m_Stations[0]);
		m_Stations[0] = new StationElement(2, 7, layout->colOffset(1), layout->osbRowOffset(4, -0.5), layout->font());
		m_Stations[1] = new StationElement(2, 7, layout->colOffset(1), layout->osbRowOffset(3, -0.5), layout->font());
		m_Stations[2] = new StationElement(3, 7, layout->colOffset(2), layout->osbRowOffset(2, -1.0), layout->font());
		m_Stations[3] = new StationElement(3, 7, layout->colOffset(3), layout->osbRowOffset(1, -1.0), layout->font());
		m_Stations[4] = new StationElement(3, 7, layout->colOffset(-3, true), layout->rowOffset(1.5), layout->font());
		m_Stations[5] = new StationElement(3, 7, layout->colOffset(-10), layout->osbRowOffset(1, -1.0), layout->font());
		m_Stations[6] = new StationElement(3, 7, layout->colOffset(-9), layout->osbRowOffset(2, -1.0), layout->font());
		m_Stations[7] = new StationElement(2, 7, layout->colOffset(-8), layout->osbRowOffset(3, -0.5), layout->font());
		m_Stations[8] = new StationElement(2, 7, layout->colOffset(-8), layout->osbRowOffset(4, -0.5), layout->font());
		for (unsigned i = 0; i < 9; ++i) {
			m_Group->addChild(m_Stations[i]->node());
			m_JettisonCount[i] = 0;
		}
		if (!m_JettisonPage) {
			m_Gun = new StationElement(2, 6, layout->colOffset(1), layout->osbRowOffset(0, -0.5), layout->font());
			m_Group->addChild(m_Gun->node());
		}
	}

	std::string format(StoresInventory::StoreData const &data) {
		if (data.empty()) {
			return "-------";
		} else {
			std::string display = stringprintf("%d %s", data.count, data.label);
			return display.substr(0, 7);
		}
	}

	void formatStation(unsigned idx, unsigned &line, unsigned &blanks, StoresInventory::StoreData const &data) {
		if (!data.empty()) {
			if (!m_JettisonPage || data.can_jettison) {
				m_Stations[idx]->setLine(line++, format(data));
			} else {
				++blanks;
			}
		}
	}

	void updateDisplay() {
		m_DirtyCount = m_Inventory->getDirtyCount();
		for (unsigned i = 0; i < 9; ++i) {
			StoresInventory::StationData const &station = m_Inventory->getStation(i);
			unsigned line = 0;
			unsigned blanks = 0;
			formatStation(i, line, blanks, station.pylon);
			formatStation(i, line, blanks, station.rack);
			formatStation(i, line, blanks, station.store);
			m_JettisonCount[i] = line;
			line += blanks;
			for (; line < m_Stations[i]->size(); ++line) m_Stations[i]->setLine(line, "-------");
		}
		if (!m_JettisonPage) {
			StoresInventory::StoreData const &gun = m_Inventory->getGunRounds();
			m_Gun->setLine(0, stringprintf("%02d GUN", gun.count / 10));
			m_Gun->setLine(1, gun.label);
		}
	}

	Ref<StoresInventory> m_Inventory;
	Ref<DisplayLayout> m_Layout;
	osg::ref_ptr<osg::Group> m_Group;
	StationElement::RefT m_Stations[9];
	StationElement::RefT m_Gun;
	bool m_JettisonPage;
	int m_DirtyCount;
	int m_SelectionCount;
	unsigned m_JettisonCount[9];
};


class MDDE: public NestedPage {
public:
	MDDE(DisplayLayout *layout, StoresManagementPage *sms, std::string const &label, NestedPage *rtn): NestedPage(layout), m_SMS(sms), m_Label(label), m_ReturnPage(rtn) {
		osb(0)->setText(m_Label);
		osb(2)->setText("RTN");
		osb(10)->setText("S-J");
		osb(17)->setText("LAU");
		osb(18)->setText("TER");
		osb(19)->setText("MAU");
	}

	virtual NestedPage *onOSB(unsigned index) {
		if (index == 2) return m_ReturnPage;
		return 0;
	}

private:
	StoresManagementPage *m_SMS;
	std::string m_Label;
	NestedPage *m_ReturnPage;
};


class InventoryPage: public NestedPage {
public:
	InventoryPage(DisplayLayout *layout, StoresManagementPage *sms, std::string const &label, NestedPage *rtn, InventoryDisplay *inv_display=0): NestedPage(layout), m_SMS(sms), m_Label(label), m_ReturnPage(rtn) {
		m_MDDE = new MDDE(layout, sms, label, this);
		osb(0)->setText(m_Label);
		if (m_ReturnPage) {
			osb(3)->setText("INV", true);
		}
		osb(4)->setText("CLR");
		osb(10)->setText("S-J");
		m_InventoryDisplay = (inv_display ? inv_display : m_SMS->getInventoryDisplay());
		CSP_VERIFY(m_InventoryDisplay.valid());
		display()->addChild(m_InventoryDisplay->group());
	}

	virtual NestedPage *onOSB(unsigned index) {
		if (index == 2) {
			m_InventoryDisplay->toggleSelection(4);
		} else if (index == 3) {
			return m_ReturnPage;
		} else if (index == 4) {
			m_InventoryDisplay->clearSelections();
		} else if (index == 5 && m_ShowChange) {
			return m_MDDE.get();
		} else if (index >= 6 && index <= 9) {
			m_InventoryDisplay->toggleSelection(index - 1);
		} else if (index >= 15 && index <= 18) {
			m_InventoryDisplay->toggleSelection(index - 15);
		}
		updateChange();
		return 0;
	}

	void updateChange() {
		if (!m_InventoryDisplay->isJettisonPage()) {
			if (m_ShowChange != m_InventoryDisplay->hasSelection()) {
				m_ShowChange = !m_ShowChange;
				osb(5)->setText(m_ShowChange ? "CHNG" : "");
			}
		}
	}

	virtual double update(double) {
		m_InventoryDisplay->update();  // XXX should autoupdate on change
		// need to update CHNG here in addition to onOSB since m_InventoryDisplay
		// selections can be modified in other master modes.
		updateChange();
		return 0.0;
	}

private:
	StoresManagementPage *m_SMS;
	std::string m_Label;
	NestedPage *m_ReturnPage;
	Ref<NestedPage> m_MDDE;
	Ref<InventoryDisplay> m_InventoryDisplay;
	bool m_ShowChange;
};

class NumericEntry: public NestedPage {
	struct Entry {
		Entry(std::string const &unit, double value, unsigned digits, unsigned decimal, unsigned low, unsigned high): unit(unit), value(value), digits(digits), decimal(decimal), low(low), high(high) { }
		std::string unit;
		double value;
		unsigned digits;
		unsigned decimal;
		unsigned low;
		unsigned high;
	};

public:
	NumericEntry(DisplayLayout *layout, StoresManagementPage *sms, NestedPage *rtn): NestedPage(layout), m_SMS(sms), m_ReturnPage(rtn) {
		osb(1)->setText("ENTR");
		osb(2)->setText("RTN");
		osb(3)->setText("RCL");
		for (unsigned i = 0; i < 5; ++i) {
			osb(i + 5)->setText(stringprintf("%d", (i + 6) % 10));
			osb(19 - i)->setText(stringprintf("%d", i + 1));
		}
		m_Prompt = addText(PlaceAbs(0.0, layout->rowOffset(-2.0, true), osgText::Text::CENTER_BOTTOM));
		for (unsigned i = 0; i < 4; ++i) {
			m_EntryBox[i] = addText(PlaceAbs(0.0, layout->rowOffset(i - 2.0, true), osgText::Text::CENTER_TOP));
			m_EntryBox[i]->show(false);
		}
		m_LeftStar = addText(PlaceAbs(0, 0, osgText::Text::CENTER_TOP));
		m_RightStar = addText(PlaceAbs(0, 0, osgText::Text::CENTER_TOP));
		m_LeftStar->setText("*", true);
		m_RightStar->setText("*", true);
		setCursor(0);
	}

	void reset(std::string const &prompt) {
		m_Callback.reset(0);
		m_Prompt->setText(prompt);
		m_Entries.clear();
		for (unsigned i = 0; i < 4; ++i) m_EntryBox[i]->show(false);
		m_Current = "";
		setCursor(0);
	}

	void setCallback(sigc::slot<void> const &callback) {
		m_Callback.reset(new sigc::slot<void>(callback));
	}

	void addEntry(std::string const &unit, double value, unsigned digits=4, unsigned decimal=0, unsigned low=0, unsigned high=9999) {
		CSP_VERIFY(m_Entries.size() < 4);
		m_Entries.push_back(Entry(unit, value, digits, decimal, low, high));
		formatOutput(m_Entries.size() - 1);
	}

	template <typename T>
	void get(unsigned index, T& value) const {
		value = static_cast<T>(getValue(index));
	}

	double getValue(unsigned index) const {
		CSP_VERIFY(index < m_Entries.size());
		return m_Entries[index].value;
	}

protected:
	void setCursor(unsigned index) {
		m_LeftStar->setPosition(layout()->colOffset(-6.5, true), layout()->rowOffset(index - 2.0, true));
		m_RightStar->setPosition(layout()->colOffset(6.5, true), layout()->rowOffset(index - 2.0, true));
		m_Index = index;
	}

	void formatOutput(unsigned index) {
		CSP_VERIFY(index < m_Entries.size());
		Entry entry = m_Entries[index];
		std::string text;
		bool invert = false;
		if (index == m_Index && !m_Current.empty()) {
			invert = true;
			if (entry.decimal > 0) {
				if (m_Current.size() <= entry.decimal) {
					text = stringprintf(".%*s", entry.decimal, m_Current);
				} else {
					size_t i = m_Current.size() - entry.decimal;
					text = stringprintf("%s.%s", m_Current.substr(0, i), m_Current.substr(i, entry.decimal));
				}
			} else {
				text = m_Current;
			}
		} else {
			std::string format = stringprintf("%%.%df", entry.decimal);
			text = stringprintf(format.c_str(), entry.value);
		}
		m_EntryBox[index]->setText(stringprintf("  %8s  ", text + entry.unit), invert);
		m_EntryBox[index]->show(true);
	}

	virtual NestedPage *onOSB(unsigned index) {
		if (index == 2) {
			if (m_Callback.valid()) (*m_Callback)();
			return m_ReturnPage;
		}
		if (!m_Entries.empty()) {
			Entry &entry = m_Entries[m_Index];
			if (index == 1) {
				if (!m_Current.empty()) {
					double value = std::min(entry.high, std::max<unsigned>(entry.low, atoi(m_Current.c_str())));
					entry.value = value / pow(10.0, (double)entry.decimal);
					m_Current = "";
					formatOutput(m_Index);
				}
				setCursor(m_Index + 1);
				if (m_Index >= m_Entries.size()) {
					if (m_Callback.valid()) (*m_Callback)();
					return m_ReturnPage;
				}
			}
			if (index == 3) {
				m_Current = "";
				formatOutput(m_Index);
			}
			char digit = 0;
			if (index >= 5 && index < 10) {
				digit = '0' + (index + 1) % 10;
			} else if (index >= 15 && index < 20) {
				digit = '0' + (20 - index);
			}
			if (digit && (m_Current.size() < entry.digits)) {
				m_Current += digit;
				formatOutput(m_Index);
			}
		}
		return 0;
	}

private:
	StoresManagementPage *m_SMS;
	NestedPage *m_ReturnPage;
	DisplayText::RefT m_Prompt;
	DisplayText::RefT m_LeftStar;
	DisplayText::RefT m_RightStar;
	DisplayText::RefT m_EntryBox[4];
	std::vector<Entry> m_Entries;
	std::string m_Current;
	unsigned m_Index;
	ScopedPointer<sigc::slot<void> > m_Callback;
};


class AirToGroundControlPage: public NestedPage {
public:
	AirToGroundControlPage(DisplayLayout *layout, StoresManagementPage *sms, NestedPage *rtn, NestedPage *inv): NestedPage(layout), m_SMS(sms), m_ReturnPage(rtn), m_InventoryPage(inv) {
		m_NumericEntry = new NumericEntry(layout, sms, this);
		osb(0)->setText("A-G");
		osb(3)->setText("INV");
		osb(4)->setText("CNTL", true);
		osb(5)->setText("L\nA\nD\nD");
		osb(10)->setText("S-J");
		osb(16)->setText("C\n4");
		osb(17)->setText("C\n3");
		osb(18)->setText("C\n2");
		osb(19)->setText("C\n1", true);
		m_C1 = addText(PlaceOSB(19, 2, 0));
		m_C2 = addText(PlaceOSB(18, 2, 0));
		m_C3 = addText(PlaceOSB(17, 2, 0));
		m_C4 = addText(PlaceOSB(16, 2, 0.5));
		m_LADD = addText(PlaceOSB(5, -14.0, -0.5));
		m_LADD->setAlignment(osgText::Text::LEFT_CENTER);
		m_Mode = 19;
		updateDisplay();
	}

	virtual void activate() {
		updateDisplay();
	}

	void updateDisplay() {
		AirToGroundSubmodeLabels labels; // XXX
		osb(1)->setText(labels.get(profile().submode));
		m_C1->setText(stringprintf("AD1%5.2fSEC\nAD2%5.2fSEC", profile().cat1_ad1, profile().cat1_ad2));
		m_C2->setText(stringprintf("AD%5.2fSEC\nBA%5dFT", profile().cat2_ad, profile().cat2_ba));
		m_C3->setText(stringprintf("AD%5.2fSEC\nBA%5dFT", profile().cat3_ad, profile().cat3_ba));
		m_C4->setText(stringprintf("AD1%5.2fSEC\nAD2%5.2fSEC\nBA%5dFT", profile().cat4_ad1, profile().cat4_ad2, profile().cat4_ba));
		m_LADD->setText(stringprintf("PR%6dFT  \nTOF%6.2fSEC\nMRA%5dFT  ", profile().ladd_pr, profile().ladd_tof, profile().ladd_mra));
		osb(9)->setText(stringprintf("REL ANG\n%d", profile().release_angle));
	}

	virtual NestedPage *onOSB(unsigned index) {
		if (index == 3) {
			return m_InventoryPage;
		}
		if (index == 4) {
			return m_ReturnPage;
		}
		const bool edit = (index == m_Mode);
		if (index == 19 && edit) { // C1
			m_NumericEntry->reset("ENTER\nCAT 1 AD1/AD2");
			m_NumericEntry->addEntry("SEC", profile().cat1_ad1, 4, 2, 0, 9999);
			m_NumericEntry->addEntry("SEC", profile().cat1_ad2, 4, 2, 0, 9999);
			m_NumericEntry->setCallback(sigc::mem_fun(this, &AirToGroundControlPage::setC1));
			return m_NumericEntry.get();
		}
		if (index == 18 && edit) { // C2
			m_NumericEntry->reset("ENTER\nCAT 2 AD/BA");
			m_NumericEntry->addEntry("SEC", profile().cat2_ad, 4, 2, 0, 9999);
			m_NumericEntry->addEntry("FT", profile().cat2_ba, 4, 0, 0, 9999);
			m_NumericEntry->setCallback(sigc::mem_fun(this, &AirToGroundControlPage::setC2));
			return m_NumericEntry.get();
		}
		if (index == 17 && edit) { // C3
			m_NumericEntry->reset("ENTER\nCAT 3 AD/BA");
			m_NumericEntry->addEntry("SEC", profile().cat3_ad, 4, 2, 0, 9999);
			m_NumericEntry->addEntry("FT", profile().cat3_ba, 4, 0, 0, 9999);
			m_NumericEntry->setCallback(sigc::mem_fun(this, &AirToGroundControlPage::setC3));
			return m_NumericEntry.get();
		}
		if (index == 16 && edit) { // C4
			m_NumericEntry->reset("ENTER\nCAT 4 AD1/AD2/BA");
			m_NumericEntry->addEntry("SEC", profile().cat4_ad1, 4, 2, 0, 9999);
			m_NumericEntry->addEntry("SEC", profile().cat4_ad2, 4, 2, 0, 9999);
			m_NumericEntry->addEntry("FT", profile().cat4_ba, 4, 0, 0, 9999);
			m_NumericEntry->setCallback(sigc::mem_fun(this, &AirToGroundControlPage::setC4));
			return m_NumericEntry.get();
		}
		if (index == 5 && edit) { // LADD
			m_NumericEntry->reset("ENTER\nLADD PR/TOF/MRA");
			m_NumericEntry->addEntry("FT", profile().ladd_pr, 5, 0, 0, 99999);
			m_NumericEntry->addEntry("SEC", profile().ladd_tof, 4, 2, 0, 9999);
			m_NumericEntry->addEntry("FT", profile().ladd_mra, 4, 0, 0, 9999);
			m_NumericEntry->setCallback(sigc::mem_fun(this, &AirToGroundControlPage::setLADD));
			return m_NumericEntry.get();
		}
		if (index == 9) {
			m_NumericEntry->reset("ENTER\nRELEASE ANGLE");
			m_NumericEntry->addEntry("DEG", profile().release_angle, 2, 0, 0, 45);
			m_NumericEntry->setCallback(sigc::mem_fun(this, &AirToGroundControlPage::setReleaseAngle));
			return m_NumericEntry.get();
		}
		if ((index >= 16 && index < 20) || index == 5) {
			osb(m_Mode)->setInvert(false);
			m_Mode = index;
			osb(m_Mode)->setInvert(true);
		}
		return 0;
	}

	void setC1() {
		m_NumericEntry->get(0, profile().cat1_ad1);
		m_NumericEntry->get(1, profile().cat1_ad2);
		updateDisplay();
	}

	void setC2() {
		m_NumericEntry->get(0, profile().cat2_ad);
		m_NumericEntry->get(1, profile().cat2_ba);
		updateDisplay();
	}

	void setC3() {
		m_NumericEntry->get(0, profile().cat3_ad);
		m_NumericEntry->get(1, profile().cat3_ba);
		updateDisplay();
	}

	void setC4() {
		m_NumericEntry->get(0, profile().cat4_ad1);
		m_NumericEntry->get(1, profile().cat4_ad2);
		m_NumericEntry->get(2, profile().cat4_ba);
		updateDisplay();
	}

	void setLADD() {
		m_NumericEntry->get(0, profile().ladd_pr);
		m_NumericEntry->get(1, profile().ladd_tof);
		m_NumericEntry->get(2, profile().ladd_mra);
		updateDisplay();
	}

	void setReleaseAngle() {
		m_NumericEntry->get(0, profile().release_angle);
		updateDisplay();
	}

	BombProfile &profile() {
		return m_SMS->getBombProfile();
	}

private:
	DisplayText::RefT m_C1;
	DisplayText::RefT m_C2;
	DisplayText::RefT m_C3;
	DisplayText::RefT m_C4;
	DisplayText::RefT m_LADD;
	StoresManagementPage *m_SMS;
	NestedPage *m_ReturnPage;
	NestedPage *m_InventoryPage;
	Ref<NumericEntry> m_NumericEntry;
	unsigned m_Mode;
};


class AirToGroundSubmodePage: public NestedPage {
public:
	AirToGroundSubmodePage(DisplayLayout *layout, StoresManagementPage *sms, NestedPage *rtn, NestedPage *inv): NestedPage(layout), m_SMS(sms), m_ReturnPage(rtn), m_InventoryPage(inv) {
		osb(0)->setText("A-G");
		osb(3)->setText("INV");
	}

	virtual void activate() {
		AirToGround::Submode submode = m_SMS->getBombProfile().submode;
		AirToGroundSubmodeLabels labels; // XXX
		osb(1)->setText(labels.get(submode));
		osb(5)->setText("ULFT", submode == AirToGround::ULOFT);
		osb(15)->setText("MAN", submode == AirToGround::MANUAL);
		osb(16)->setText("LADD", submode == AirToGround::LADD);
		osb(17)->setText("DTOS", submode == AirToGround::DTOS);
		osb(18)->setText("CCRP", submode == AirToGround::CCRP);
		osb(19)->setText("CCIP", submode == AirToGround::CCIP);
	}

	virtual NestedPage *onOSB(unsigned index) {
		AirToGround::Submode &submode = m_SMS->getBombProfile().submode;
		switch (index) {
			case 1:
				return m_ReturnPage;
			case 3:
				return m_InventoryPage;
			case 5:
				submode = AirToGround::ULOFT;
				return m_ReturnPage;
			case 15:
				submode = AirToGround::MANUAL;
				return m_ReturnPage;
			case 16:
				submode = AirToGround::LADD;
				return m_ReturnPage;
			case 17:
				submode = AirToGround::DTOS;
				return m_ReturnPage;
			case 18:
				submode = AirToGround::CCRP;
				return m_ReturnPage;
			case 19:
				submode = AirToGround::CCIP;
				return m_ReturnPage;
			default:
				break;
		}
		return 0;
	}

private:
	StoresManagementPage *m_SMS;
	NestedPage *m_ReturnPage;
	NestedPage *m_InventoryPage;
};


class AirToGroundBomb: public NestedPage {
public:
	AirToGroundBomb(DisplayLayout *layout, StoresManagementPage *sms): NestedPage(layout), m_SMS(sms) {
		m_NumericEntry = new NumericEntry(layout, sms, this);
		m_InventoryPage = new InventoryPage(layout, sms, "A-G", this);
		m_SubmodePage = new AirToGroundSubmodePage(layout, sms, this, m_InventoryPage.get());
		m_ControlPage = new AirToGroundControlPage(layout, sms, this, m_InventoryPage.get());
		m_Parameters = addText(PlaceAbs(layout->colOffset(-6.0, true), layout->rowOffset(-2.0, true), osgText::Text::LEFT_TOP));
		osb(0)->setText("A-G");
		osb(3)->setText("INV");
		osb(4)->setText("CNTL");
		osb(5)->setText("6 M82");
		osb(6)->setText("PROF2");
		osb(10)->setText("S-J");
		osb(15)->setText("1 2 3");
		osb(16)->setText("AD/BA");
		osb(19)->setText("RBS");
		updateDisplay();
	}

	virtual void activate() {
		updateDisplay();
	}

	virtual NestedPage *onOSB(unsigned index) {
		if (index == 1) {
			return m_SubmodePage.get();
		}
		if (index == 3) {
			return m_InventoryPage.get();
		}
		if (index == 4) {
			return m_ControlPage.get();
		}
		if (index == 17) {
			switch (profile().fuse_arming_option) {
				case BombProfile::NOSE: profile().fuse_arming_option = BombProfile::NSTL; break;
				case BombProfile::NSTL: profile().fuse_arming_option = BombProfile::TAIL; break;
				case BombProfile::TAIL: profile().fuse_arming_option = BombProfile::NOSE; break;
			}
			updateDisplay();
		}
		if (index == 16) {
			profile().adba_selection = (profile().adba_selection == BombProfile::BA) ? BombProfile::AD : BombProfile::BA;
			updateDisplay();
		}
		if (index == 8) {
			m_NumericEntry->reset("ENTER\nIMPACT SPACING");
			m_NumericEntry->addEntry("FT", profile().impact_spacing, 3, 0, 0, 999);
			m_NumericEntry->setCallback(sigc::mem_fun(this, &AirToGroundBomb::setImpactSpacing));
			return m_NumericEntry.get();
		}
		if (index == 9) {
			m_NumericEntry->reset("ENTER\nRELEASE PULSES");
			m_NumericEntry->addEntry("", profile().release_pulses, 2, 0, 1, 12);
			m_NumericEntry->setCallback(sigc::mem_fun(this, &AirToGroundBomb::setReleasePulses));
			return m_NumericEntry.get();
			//profile().release_pulses = (profile().release_pulses + 1) % 8;
			//updateDisplay();
		}
		if (index == 7) {
			profile().pair_release = !profile().pair_release;
			updateDisplay();
		}
		return 0;
	}

	void updateDisplay() {
		AirToGroundSubmodeLabels labels; // XXX
		osb(1)->setText(labels.get(profile().submode));
		osb(7)->setText(stringprintf("%d %s", profile().release_pulses, profile().pair_release ? "PAIR" : "SNGL"));
		osb(8)->setText(stringprintf("%dFT", profile().impact_spacing));
		osb(9)->setText(stringprintf("RP\n%d", profile().release_pulses));
		switch (profile().fuse_arming_option) {
			case BombProfile::NOSE: osb(17)->setText("NOSE"); break;
			case BombProfile::NSTL: osb(17)->setText("NSTL"); break;
			case BombProfile::TAIL: osb(17)->setText("TAIL"); break;
			default: osb(17)->setText("?");
		}
		// TODO customize for different fuse modes and ad/ba settings
		m_Parameters->setText(stringprintf("AD %5.2fSEC\n \nREL ANG %3d", profile().cat1_ad1, profile().release_angle));
	}

	void setImpactSpacing() {
		m_NumericEntry->get(0, profile().impact_spacing);
		updateDisplay();
	}

	void setReleasePulses() {
		m_NumericEntry->get(0, profile().release_pulses);
		updateDisplay();
	}

	BombProfile &profile() {
		return m_SMS->getBombProfile();
	}

private:
	StoresManagementPage *m_SMS;
	Ref<AirToGroundSubmodePage> m_SubmodePage;
	Ref<InventoryPage> m_InventoryPage;
	Ref<AirToGroundControlPage> m_ControlPage;
	Ref<NumericEntry> m_NumericEntry;
	DisplayText::RefT m_Parameters;
};


StoresManagementPage::StoresManagementPage(DisplayLayout *layout): DisplayFormat(layout), m_LastMode(NAV) {
	m_BombProfile = new BombProfile;
	m_Inventory = new StoresInventory;
	m_InventoryDisplay = new InventoryDisplay(m_Inventory.get(), layout);
	m_SelectiveJettisonDisplay = new InventoryDisplay(m_Inventory.get(), layout, true);
	m_EmergencyJettisonDisplay = new InventoryDisplay(m_Inventory.get(), layout, true);

	m_ModePages[NAV] = new InventoryPage(layout, this, "STBY", 0);
	m_ModePages[AA] = new InventoryPage(layout, this, "AAM", 0);
	m_ModePages[AG] = new AirToGroundBomb(layout, this);
	m_ModePages[SJ] = new InventoryPage(layout, this, "S-J", 0, m_SelectiveJettisonDisplay.get());
	m_ActivePage = m_ModePages[NAV];
	m_ActivePage->activate();
	display()->addChild(m_ActivePage->display());

	// m_ModePages get reassigned, so we need a secondary store of all immediate
	// subpages to ensure that they are not deleted prematurely.
	for (ModePages::iterator iter = m_ModePages.begin(); iter != m_ModePages.end(); ++iter) {
		m_ChildPages.push_back(iter->second);
	}
}

StoresManagementPage::~StoresManagementPage() {
}


void StoresManagementPage::importChannels(Bus* bus) {
	b_MasterMode = bus->getSharedChannel("MasterMode", true);
	for (ModePages::iterator iter = m_ModePages.begin(); iter != m_ModePages.end(); ++iter) {
		iter->second->importChannels(bus);
	}
}

void StoresManagementPage::onSelect(DisplayInterface*) {
}

bool StoresManagementPage::onOSB(int idx, DisplayInterface*) {
	if (idx == 10) {
		if (b_MasterMode->mode() == SJ) {
			b_MasterMode->setSelectiveJettison(false);
		} else {
			b_MasterMode->setSelectiveJettison(true);
		}
		return true;
	}
	NestedPage *page = m_ActivePage->onOSB(idx);
	if (page && (page != m_ActivePage.get())) {
		m_ModePages[m_LastMode] = page;
		display()->removeChild(m_ActivePage->display());
		m_ActivePage = page;
		m_ActivePage->activate();
		display()->addChild(m_ActivePage->display());
	}
	return true;
}

double StoresManagementPage::update(double dt, DisplayInterface*) {
	MasterMode mode = b_MasterMode->mode();
	const bool mode_change = (mode != m_LastMode);
	if (mode_change) {
		display()->removeChild(m_ActivePage->display());
		m_LastMode = mode;
		m_ActivePage = m_ModePages[mode];
		CSP_VERIFY(m_ActivePage.valid());
		m_ActivePage->activate();
		display()->addChild(m_ActivePage->display());
	}
	return m_ActivePage->update(dt);
}

BombProfile &StoresManagementPage::getBombProfile() {
	return *m_BombProfile;
}

} // namespace f16

CSP_NAMESPACE_END

