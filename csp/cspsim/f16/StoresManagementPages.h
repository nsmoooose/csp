#pragma once
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
 * @file StoresManagementPages.h
 *
 **/

#include <csp/cspsim/f16/MultiFunctionDisplay.h>
#include <map>

namespace csp {

namespace f16 {

class StoresInventory;
class InventoryDisplay;
class NestedPage;
struct BombProfile;

class StoresManagementPage: public DisplayFormat {
public:
	StoresManagementPage(DisplayLayout *layout);
	virtual ~StoresManagementPage();

	virtual void importChannels(Bus*);

	virtual std::string id() const { return "SMS"; }

	virtual void onSelect(DisplayInterface*);
	virtual bool onOSB(int idx, DisplayInterface*);
	virtual double update(double, DisplayInterface*);

	InventoryDisplay *getInventoryDisplay() { return m_InventoryDisplay.get(); }

	BombProfile &getBombProfile();

protected:
	MasterModeSelection::RefT b_MasterMode;

	Ref<StoresInventory> m_Inventory;
	Ref<InventoryDisplay> m_InventoryDisplay;
	Ref<InventoryDisplay> m_SelectiveJettisonDisplay;
	Ref<InventoryDisplay> m_EmergencyJettisonDisplay;
	Ref<InventoryDisplay> m_ActiveDisplay;

	typedef std::map<MasterMode, Ref<NestedPage> > ModePages;
	ModePages m_ModePages;
	std::vector<Ref<NestedPage> > m_ChildPages;
	Ref<NestedPage> m_ActivePage;
	MasterMode m_LastMode;
	Ref<BombProfile> m_BombProfile;  /** @TODO temporary; should be pulled from the bus */
};

} // namespace f16

} // namespace csp
