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
 * @file CheckBox.h
 *
 **/

#ifndef __CSPSIM_WF_CHECKBOX_H__
#define __CSPSIM_WF_CHECKBOX_H__

#include <csp/csplib/util/Signal.h>
#include <csp/cspwf/SingleControlContainer.h>

namespace csp {
namespace wf {

class Label;

/** A class that represents a CheckBox.
 *
 *  A CheckBox is a widget that can be pressed by the user with the mouse.
 *  The checked or unchecked state is displayed.
 */
class CSPWF_EXPORT CheckBox : public SingleControlContainer {
public:
	CheckBox();
	virtual ~CheckBox();

	virtual void performLayout();
	virtual void layoutChildControls();

	virtual const std::string getText() const;
	virtual void setText(const std::string& text);

	virtual bool getChecked() const;
	virtual void setChecked(bool checked);

	virtual Signal* getCheckedChangedSignal();
	virtual void onClick(ClickEventArgs& event);

	template<class Archive>
	void serialize(Archive & ar) {
		SingleControlContainer::serialize(ar);
		ar & make_nvp("@Checked", m_Checked);
		ar & make_nvp("@Text", m_Text);
	}

private:

protected:
	bool m_Checked;
	std::string m_Text;

	Ref<Signal> m_CheckedChanged;
};

} // namespace wf
} // namespace csp

#endif // __CSPSIM_WF_CHECKBOX_H__
