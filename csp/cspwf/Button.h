#pragma once
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

#include <csp/cspwf/SingleControlContainer.h>

namespace csp {
namespace wf {

/** A class that represents a button.
 *
 *  A button is a widget that can be pressed by the user with the mouse.
 *
 */
class CSPWF_EXPORT Button : public SingleControlContainer {
public:
	Button();
	virtual ~Button();

	virtual void performLayout();

	/** Sets the text of the Button control. */
	virtual const std::string getText() const;

	/** Returns the text of the Button control. */
	virtual void setText(const std::string& text);

	template<class Archive>
	void serialize(Archive & ar)	{
		SingleControlContainer::serialize(ar);
		ar & make_nvp("@Text", m_text);
	}

private:

	std::string m_text;
};

} // namespace wf
} // namespace csp
