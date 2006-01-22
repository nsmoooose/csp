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
 * @file DataEntryPage.cpp
 *
 **/


#include <csp/cspsim/f16/DataEntryPage.h>

CSP_NAMESPACE

DataEntryForm::DataEntryForm(): m_EntryIndex(0), m_CycleIndex(0) { }

void DataEntryForm::addCycle(DataCycle::RefT cycle) {
	assert(cycle.valid());
	cycle->setVisible(m_Cycles.empty());
	m_Cycles.push_back(cycle);
	m_Widgets.push_back(cycle);
}

void DataEntryForm::addEntry(DataEntry::RefT entry) {
	assert(entry.valid());
	entry->setActive(m_Entries.empty());
	m_Entries.push_back(entry);
	m_Widgets.push_back(entry);
}

void DataEntryForm::addWidget(DataWidget::RefT widget) {
	assert(widget.valid());
	m_Widgets.push_back(widget);
}

std::string DataEntryForm::ICP_0() { return onNumber(0); }
std::string DataEntryForm::ICP_1() { return onNumber(1); }
std::string DataEntryForm::ICP_2() { return onNumber(2); }
std::string DataEntryForm::ICP_3() { return onNumber(3); }
std::string DataEntryForm::ICP_4() { return onNumber(4); }
std::string DataEntryForm::ICP_5() { return onNumber(5); }
std::string DataEntryForm::ICP_6() { return onNumber(6); }
std::string DataEntryForm::ICP_7() { return onNumber(7); }
std::string DataEntryForm::ICP_8() { return onNumber(8); }
std::string DataEntryForm::ICP_9() { return onNumber(9); }

std::string DataEntryForm::ICP_RCL() {
	if (!m_Entries.empty()) m_Entries[m_EntryIndex]->recall();
	return "";
}

std::string DataEntryForm::ICP_ENTR() {
	if (!m_Entries.empty()) {
		if (m_Entries[m_EntryIndex]->enter()) {
			nextEntry();
		}
	}
	return "";
}

std::string DataEntryForm::ICP_RTN() { return ""; }
std::string DataEntryForm::ICP_SEQ() { return ""; }
std::string DataEntryForm::ICP_UP() { prevEntry(); return ""; }
std::string DataEntryForm::ICP_DN() { nextEntry(); return ""; }

void DataEntryForm::nextCycle() {
	if (m_Cycles.size() > 1) {
		m_Cycles[m_CycleIndex]->setVisible(false);
		m_CycleIndex = (m_CycleIndex + 1) % m_Cycles.size();
		m_Cycles[m_CycleIndex]->setVisible(true);
	}
}

void DataEntryForm::prevCycle() {
	if (m_Cycles.size() > 1) {
		m_Cycles[m_CycleIndex]->setVisible(false);
		m_CycleIndex = (m_CycleIndex + m_Cycles.size() - 1) % m_Cycles.size();
		m_Cycles[m_CycleIndex]->setVisible(true);
	}
}

void DataEntryForm::nextEntry() {
	if (m_Entries.size() > 1) {
		m_Entries[m_EntryIndex]->setActive(false);
		m_EntryIndex = (m_EntryIndex + 1) % m_Entries.size();
		m_Entries[m_EntryIndex]->setActive(true);
	}
}

void DataEntryForm::prevEntry() {
	if (m_Entries.size() > 1) {
		m_Entries[m_EntryIndex]->setActive(false);
		m_EntryIndex = (m_EntryIndex + m_Entries.size() - 1) % m_Entries.size();
		m_Entries[m_EntryIndex]->setActive(true);
	}
}

std::string DataEntryForm::ICP_INC() {
	if (!m_Cycles.empty()) m_Cycles[m_CycleIndex]->increment();
	return "";
}

std::string DataEntryForm::ICP_DEC() {
	if (!m_Cycles.empty()) m_Cycles[m_CycleIndex]->decrement();
	return "";
}

std::string DataEntryForm::onNumber(int n) {
	if (!m_Entries.empty()) m_Entries[m_EntryIndex]->onNumber(n);
	return "";
}

double DataEntryForm::update() { return 1; }

void DataEntryForm::importChannels(Bus* bus) {
	for (unsigned i = 0; i < m_Widgets.size(); ++i) m_Widgets[i]->importChannels(bus);
}

void DataEntryForm::reset() { }

void DataEntryForm::render(AlphaNumericDisplay& ded) {
	for (unsigned i = 0; i < m_Widgets.size(); ++i) m_Widgets[i]->render(ded);
}


DataEntryGroup::DataEntryGroup() { }

std::string DataEntryGroup::ICP_0() { return m_ActivePage.valid() ? m_ActivePage->ICP_0() : ""; }
std::string DataEntryGroup::ICP_1() { return m_ActivePage.valid() ? m_ActivePage->ICP_1() : ""; }
std::string DataEntryGroup::ICP_2() { return m_ActivePage.valid() ? m_ActivePage->ICP_2() : ""; }
std::string DataEntryGroup::ICP_3() { return m_ActivePage.valid() ? m_ActivePage->ICP_3() : ""; }
std::string DataEntryGroup::ICP_4() { return m_ActivePage.valid() ? m_ActivePage->ICP_4() : ""; }
std::string DataEntryGroup::ICP_5() { return m_ActivePage.valid() ? m_ActivePage->ICP_5() : ""; }
std::string DataEntryGroup::ICP_6() { return m_ActivePage.valid() ? m_ActivePage->ICP_6() : ""; }
std::string DataEntryGroup::ICP_7() { return m_ActivePage.valid() ? m_ActivePage->ICP_7() : ""; }
std::string DataEntryGroup::ICP_8() { return m_ActivePage.valid() ? m_ActivePage->ICP_8() : ""; }
std::string DataEntryGroup::ICP_9() { return m_ActivePage.valid() ? m_ActivePage->ICP_9() : ""; }
std::string DataEntryGroup::ICP_RCL() { return m_ActivePage.valid() ? m_ActivePage->ICP_RCL() : ""; }
std::string DataEntryGroup::ICP_ENTR() { return m_ActivePage.valid() ? m_ActivePage->ICP_ENTR() : ""; }
std::string DataEntryGroup::ICP_RTN() { return m_ActivePage.valid() ? m_ActivePage->ICP_RTN() : ""; }
std::string DataEntryGroup::ICP_SEQ() { return m_ActivePage.valid() ? m_ActivePage->ICP_SEQ() : ""; }
std::string DataEntryGroup::ICP_UP() { return m_ActivePage.valid() ? m_ActivePage->ICP_UP() : ""; }
std::string DataEntryGroup::ICP_DN() { return m_ActivePage.valid() ? m_ActivePage->ICP_DN() : ""; }
std::string DataEntryGroup::ICP_INC() { return m_ActivePage.valid() ? m_ActivePage->ICP_INC() : ""; }
std::string DataEntryGroup::ICP_DEC() { return m_ActivePage.valid() ? m_ActivePage->ICP_DEC() : ""; }

double DataEntryGroup::update() { return 1; }
void DataEntryGroup::importChannels(Bus*) { }
void DataEntryGroup::reset() { m_ActivePage = 0; }
void DataEntryGroup::setActivePage(DataEntryPage::RefT page) { m_ActivePage = page; }
std::string DataEntryGroup::onNumber(int) { return ""; }

CSP_NAMESPACE_END

