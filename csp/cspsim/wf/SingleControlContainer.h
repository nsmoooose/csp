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
 * @file SingleControlContainer.h
 *
 **/

#ifndef __CSPSIM_WF_SINGLECONTROLCONTAINER_H__
#define __CSPSIM_WF_SINGLECONTROLCONTAINER_H__

#include <csp/cspsim/wf/Container.h>

CSP_NAMESPACE

namespace wf {

/** The base class for all controls that has a single child control.
 *
 *  This class provides functionality to be able to handle a single 
 *  childcontrol that occupies the entire surface of this control.
 *
 *  The purpose is to have a control that automaticly resizes the child
 *  if the parent is resizing. When the layoutChildControls()
 *  method is called the child control will be resized to fit the
 *  entire surface.
 */
class CSPSIM_EXPORT SingleControlContainer : public Container {
public:
	SingleControlContainer();
	SingleControlContainer(std::string name);
	virtual ~SingleControlContainer();

	virtual ControlVector getChildControls();

	virtual void buildGeometry();
	
	virtual void setControl(Control* childControl);
	virtual Control* getControl();

	//! Resizes the child control to fit the entire surface of this control.
	virtual void layoutChildControls();

	template<class Archive>
	void serialize(Archive & ar)	{
		Container::serialize(ar);
		ar & make_nvp("Control", m_Control);
	}	
	
protected:

private:
	Ref<Control> m_Control;
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_SINGLECONTROLCONTAINER_H__


