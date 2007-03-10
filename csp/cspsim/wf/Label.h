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
 * @file Label.h
 *
 **/

#ifndef __CSPSIM_WF_LABEL_H__
#define __CSPSIM_WF_LABEL_H__

#include <csp/cspsim/wf/Control.h>

#include <osgText/Text>

CSP_NAMESPACE

namespace wf {

/** A class that represents a button.
 *
 *  A button is a widget that can be pressed by the user with the mouse.
 *  
 */
class Label : public Control {
public:
	Label();
	Label(const std::string text);
	virtual ~Label();

	virtual std::string getName() const;

	virtual void buildGeometry();
	
	virtual const std::string& getText() const;
	virtual void setText(const std::string& text);
	
	virtual osgText::Text::AlignmentType getAlignment() const;
	virtual void setAlignment(osgText::Text::AlignmentType align);
	
	template<class Archive>
	void serialize(Archive & ar) {
		Control::serialize(ar);
		ar & make_nvp("@Text", m_Text);
	}
	
private:

protected:
	std::string m_Text;	
	
	osgText::Text::AlignmentType m_Alignment;
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_LABEL_H__

