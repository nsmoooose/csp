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
 * @file ScrollBar.h
 *
 **/

#ifndef __CSPSIM_WF_SCROLLBAR_H__
#define __CSPSIM_WF_SCROLLBAR_H__

#include <csp/cspsim/wf/Container.h>

CSP_NAMESPACE

namespace wf {

/** A button placed to the left in a horizontal scrollbar. 
 * Used to scroll content to the left. The purpose of this
 * class is to make the button styleable using the name of
 * this class.
 */  	
class CSPSIM_EXPORT ScrollLeftButton : public Control {
public:
	ScrollLeftButton() : Control("ScrollLeftButton") {}
};

/** A button placed to the right in a horizontal scrollbar.
 * Used to scroll content to the right. The purpose of this
 * class is to make the button styleable using the name of
 * this class.
 */
class CSPSIM_EXPORT ScrollRightButton : public Control {
public:
	ScrollRightButton() : Control("ScrollRightButton") {}
};

/** A button placed to the top in a vertical scrollbar.
 * Used to scroll content up. The purpose of this
 * class is to make the button styleable using the name of
 * this class.
 */
class CSPSIM_EXPORT ScrollUpButton : public Control {
public:
	ScrollUpButton() : Control("ScrollUpButton") {}
};

/** A button placed at the bottom of a vertical scrollbar.
 * Used to scroll content down. The purpose of this
 * class is to make the button styleable using the name of
 * this class.
 */
class CSPSIM_EXPORT ScrollDownButton : public Control {
public:
	ScrollDownButton() : Control("ScrollDownButton") {}
};

/** Base class for scrollbars with all shared functionality.
 */ 
class CSPSIM_EXPORT ScrollBar : public Container {
public:
	ScrollBar(std::string name);

	float getValue() const;
	void setValue(float value);
	
	float getMinimum() const;
	void setMinimum(float minimum);
	
	float getMaximum() const;
	void setMaximum(float maximum);
	
private:
	float m_Value;
	float m_Minimum;
	float m_Maximum;
};

/** A vertical scrollbar. */
class CSPSIM_EXPORT VerticalScrollBar : public ScrollBar {
public:
	VerticalScrollBar();
	
	virtual void layoutChildControls();
	virtual ControlVector getChildControls();
	
private:
	Ref<ScrollUpButton> m_ScrollUpButton;
	Ref<ScrollDownButton> m_ScrollDownButton;
};

/** A horizontal scrollbar. */
class CSPSIM_EXPORT HorizontalScrollBar : public ScrollBar {
public:
	HorizontalScrollBar();

	virtual void layoutChildControls();
	virtual ControlVector getChildControls();
	
private:
	Ref<ScrollLeftButton> m_ScrollLeftButton;
	Ref<ScrollRightButton> m_ScrollRightButton;
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_SCROLLBAR_H__


