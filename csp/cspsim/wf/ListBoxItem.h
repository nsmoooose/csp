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
 * @file ListBoxItem.h
 *
 **/

#ifndef __CSPSIM_WF_LISTBOXITEM_H__
#define __CSPSIM_WF_LISTBOXITEM_H__

#include <csp/cspsim/wf/SingleControlContainer.h>

CSP_NAMESPACE

namespace wf {

/** A class that represents a button.
 *
 *  A button is a widget that can be pressed by the user with the mouse.
 *  
 */
class ListBoxItem : public SingleControlContainer {
public:
	ListBoxItem(Theme* theme);
	ListBoxItem(Theme* theme, const std::string text);
	virtual ~ListBoxItem();

	virtual void buildGeometry();
	
	virtual const std::string getText() const;
	virtual void setText(const std::string& text);
	
	virtual Control* getControl();
	virtual void setControl(Control* control);
	
private:

protected:
	Ref<Control> m_ChildControl;
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_LISTBOXITEM_H__
