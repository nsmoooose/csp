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
 * @file UpFrontControls.h
 *
 **/


#ifndef __CSPSIM_F16_UP_FRONT_CONTROLS_H__
#define __CSPSIM_F16_UP_FRONT_CONTROLS_H__

#include <System.h>
#include "DataEntryPage.h"


// At the one extreme, each system defines its own DataEntryPage and exports registers
// it with the UFC.  The UFC simply forwards ICP events on the active page, which holds
// a pointer to the relevant system and modifies it directly.  The custom pages are
// tightly bound (linked) to the corresponding system classes, and the bus is only
// used to register pages.
//
// At the other extreme, each system exports channels for every externally visible
// parameter.  DED pages are defined centrally as System subclasses, and bind to the
// appropriate channels.  The interfaces are not well defined, so DED pages may only
// partially work depending on the system implementations.


class UpFrontControls: public System, public sigc::trackable {
public:
	CSP_DECLARE_OBJECT(UpFrontControls)

	DECLARE_INPUT_INTERFACE(UpFrontControls, System)
		// the various icp button events are declared dynamically in registerChannels
	END_INPUT_INTERFACE  // protected:

public:
	UpFrontControls();
	~UpFrontControls();

	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus*);

protected:
	// event handlers
	virtual void ICP_0();
	virtual void ICP_1();
	virtual void ICP_2();
	virtual void ICP_3();
	virtual void ICP_4();
	virtual void ICP_5();
	virtual void ICP_6();
	virtual void ICP_7();
	virtual void ICP_8();
	virtual void ICP_9();
	virtual void ICP_COM1();
	virtual void ICP_COM2();
	virtual void ICP_IFF();
	virtual void ICP_LIST();
	virtual void ICP_AA();
	virtual void ICP_AG();
	virtual void ICP_RCL();
	virtual void ICP_ENTR();
	virtual void ICP_RTN();
	virtual void ICP_SEQ();
	virtual void ICP_UP();
	virtual void ICP_DN();
	virtual void ICP_INC();
	virtual void ICP_DEC();

	virtual void getInfo(InfoList &info) const;
	virtual double onUpdate(double dt);
	virtual void transition(std::string const &);
	virtual void override(std::string const &);
	virtual void updateActivePage();
	DataEntryPage::Ref getActivePage();

	void addPages(Bus *bus);

private:
	DataEntryPage::Ref m_ActivePage;
	DataEntryPage::Ref m_Override;

	DataEntryPage::Ref m_CNI;
	DataEntryPage::Ref m_Com1;
	DataEntryPage::Ref m_Com2;
	DataEntryPage::Ref m_List;
	DataEntryPage::Ref m_IFF;
	bool m_AA;
	bool m_AG;
	
	DataChannel<AlphaNumericDisplay::Ref>::Ref m_Display;

	typedef std::map<std::string, DataEntryPage::Ref> PageMap;
	PageMap m_PageMap;

	double m_ElapsedTime;
	double m_UpdateTime;
};

#endif // __CSPSIM_F16_UP_FRONT_CONTROLS_H__

