#pragma once
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
 * @file DataEntryPage.h
 *
 **/

#include <csp/cspsim/f16/DataEntry.h>
#include <csp/csplib/util/Callback.h>
#include <csp/csplib/util/Ref.h>

#include <vector>

namespace csp {

class AlphaNumericDisplay;
class Bus;


class DataEntryPage: public Referenced {
public:
	typedef Ref<DataEntryPage> RefT;
	virtual ~DataEntryPage() { }

	virtual std::string ICP_0()=0;
	virtual std::string ICP_1()=0;
	virtual std::string ICP_2()=0;
	virtual std::string ICP_3()=0;
	virtual std::string ICP_4()=0;
	virtual std::string ICP_5()=0;
	virtual std::string ICP_6()=0;
	virtual std::string ICP_7()=0;
	virtual std::string ICP_8()=0;
	virtual std::string ICP_9()=0;
	virtual std::string ICP_RCL()=0;
	virtual std::string ICP_ENTR()=0;
	virtual std::string ICP_RTN()=0;
	virtual std::string ICP_SEQ()=0;
	virtual std::string ICP_UP()=0;
	virtual std::string ICP_DN()=0;
	virtual std::string ICP_INC()=0;
	virtual std::string ICP_DEC()=0;

	virtual double update()=0;
	virtual void importChannels(Bus*)=0;
	virtual void reset()=0;
	virtual void render(AlphaNumericDisplay&)=0;

protected:
	virtual std::string onNumber(int)=0;
};


class DataEntryForm: public DataEntryPage {
public:
	typedef Ref<DataEntryForm> RefT;

	DataEntryForm();

	virtual std::string ICP_0();
	virtual std::string ICP_1();
	virtual std::string ICP_2();
	virtual std::string ICP_3();
	virtual std::string ICP_4();
	virtual std::string ICP_5();
	virtual std::string ICP_6();
	virtual std::string ICP_7();
	virtual std::string ICP_8();
	virtual std::string ICP_9();
	virtual std::string ICP_RCL();
	virtual std::string ICP_ENTR();
	virtual std::string ICP_RTN();
	virtual std::string ICP_SEQ();
	virtual std::string ICP_UP();
	virtual std::string ICP_DN();
	virtual std::string ICP_INC();
	virtual std::string ICP_DEC();

	virtual double update();
	virtual void importChannels(Bus *bus);
	virtual void reset();
	virtual void render(AlphaNumericDisplay&);

protected:
	virtual std::string onNumber(int n);

	virtual void nextCycle();
	virtual void prevCycle();
	virtual void nextEntry();
	virtual void prevEntry();

	virtual void addCycle(DataCycle::RefT);
	virtual void addEntry(DataEntry::RefT);
	virtual void addWidget(DataWidget::RefT);

private:
	std::vector<DataWidget::RefT> m_Widgets;
	std::vector<DataEntry::RefT> m_Entries;
	std::vector<DataCycle::RefT> m_Cycles;
	int m_EntryIndex;
	int m_CycleIndex;
};


class DataEntryGroup: public DataEntryPage {
public:
	typedef Ref<DataEntryGroup> RefT;

	DataEntryGroup();

	virtual std::string ICP_0();
	virtual std::string ICP_1();
	virtual std::string ICP_2();
	virtual std::string ICP_3();
	virtual std::string ICP_4();
	virtual std::string ICP_5();
	virtual std::string ICP_6();
	virtual std::string ICP_7();
	virtual std::string ICP_8();
	virtual std::string ICP_9();
	virtual std::string ICP_RCL();
	virtual std::string ICP_ENTR();
	virtual std::string ICP_RTN();
	virtual std::string ICP_SEQ();
	virtual std::string ICP_UP();
	virtual std::string ICP_DN();
	virtual std::string ICP_INC();
	virtual std::string ICP_DEC();

	virtual double update();
	virtual void importChannels(Bus *bus);
	virtual void reset();

protected:
	virtual void setActivePage(DataEntryPage::RefT page);
	virtual std::string onNumber(int n);

private:
	DataEntryPage::RefT m_ActivePage;
};

} // namespace csp
