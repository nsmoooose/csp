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
 * @file Button.h
 *
 **/

#ifndef __CSPSIM_WF_BUTTON_H__
#define __CSPSIM_WF_BUTTON_H__

#include <csp/csplib/util/Signal.h>
#include <csp/cspsim/wf/SingleControlContainer.h>

CSP_NAMESPACE

namespace wf {

/** A class that represents a button.
 *
 *  A button is a widget that can be pressed by the user with the mouse.
 *  
 */
class Button : public SingleControlContainer {
public:
	Button();
	Button(const std::string text);
	virtual ~Button();

	virtual std::string getName() const;

	virtual void buildGeometry();
	
	virtual const std::string getText() const;
	virtual void setText(const std::string& text);
	
	virtual void addButtonClickedHandler(const sigc::slot<void> &handler);

	template<class Archive>
	void serialize(Archive & ar)	{
		SingleControlContainer::serialize(ar);
		ar & make_nvp("@Text", m_text);
	}
	
private:

protected:
	typedef sigc::signal<void> ButtonClickedSignal;	
	ButtonClickedSignal m_ButtonClicked;

	class ButtonClickedCallback;
	
	std::string m_text;
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_BUTTON_H__


