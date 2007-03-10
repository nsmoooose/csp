// Combat Simulator Project
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
 * @file Tab.h
 *
 **/

#ifndef __CSPSIM_WF_TAB_H__
#define __CSPSIM_WF_TAB_H__

#include <csp/cspsim/wf/Container.h>
#include <csp/cspsim/wf/TabPage.h>

CSP_NAMESPACE

namespace wf {

/** A class that represents a tab control.
 *
 *  This class contains a vector with one or more tab pages.
 *  
 */
class Tab : public Container {
public:
	Tab();
	virtual ~Tab();

	virtual ControlVector getChildControls();

	virtual std::string getName() const;

	virtual void buildGeometry();
	virtual void layoutChildControls();
	
	virtual void addPage(TabPage* page);
	
	virtual TabPage* getCurrentPage() const;
	virtual TabPage* getCurrentPage();
	virtual void setCurrentPage(TabPage* page);

	template<class Archive>
	void serialize(Archive & ar) {
		Container::serialize(ar);
		ar & make_nvp("Pages", m_Pages);
	}	

private:
	TabPageVector m_Pages;
	
	TabPage* m_CurrentPage;

protected:
	class TabClickedCallback;
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_TAB_H__


