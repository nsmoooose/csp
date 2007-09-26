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

/** A Control that can display text. The text can be both single or multiline.
 * Remember to set width since this controls when rowbreaks are created. Important
 * properties to set on the Style object is fontFamily, fontSize, color, 
 * textHorizontalAlign, textVerticalAlign. If these properties are missing the 
 * text cannot be displayed.
 */
class CSPSIM_EXPORT Label : public Control {
public:
	Label();
	virtual ~Label();

	virtual void buildGeometry();
	
	/** Returns the text set on this control. */
	virtual const std::string& getText() const;
	
	/** Sets the new text to display. It is also possible to use ${resource_string}
	 * in order to use localized strings. Do not forget to include the localized string
	 * resource from the window xml file. 
	 */ 
	virtual void setText(const std::string& text);
	
	template<class Archive>
	void serialize(Archive & ar) {
		Control::serialize(ar);
		ar & make_nvp("@Text", m_Text);
		ar & make_nvp("Text", m_Text);
	}
	
private:

protected:
	std::string m_Text;	
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_LABEL_H__

