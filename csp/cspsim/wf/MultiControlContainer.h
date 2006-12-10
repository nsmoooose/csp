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
 * @file MultiControlContainer.h
 *
 **/

#ifndef __CSPSIM_WF_MULTICONTROLCONTAINER_H__
#define __CSPSIM_WF_MULTICONTROLCONTAINER_H__

#include <csp/cspsim/wf/Container.h>

CSP_NAMESPACE

namespace wf {

/** Class that has several child controls.
 *
 *  This class provides functionality to be able to handle several 
 *  childcontrols. Every child control must be positioned manually
 *  with code.
 *
 *  The purpose is to have a control that handles several child 
 *  controls and have absolute control over positioning.
 */
class MultiControlContainer : public Container {
public:
	MultiControlContainer(Theme* theme);
	virtual ~MultiControlContainer();
	
	virtual ControlVector getChildControls();

	virtual void buildGeometry();

	//! Does nothing in this implementation.
	virtual void layoutChildControls();

	//! Retreives a vector with all child controls for this control.
	virtual ControlVector getControls();
	
	//! Adds a new child control to this container.
	virtual void addControl(Control* control);
	
	//! Removes the specified control from the container.
	virtual void removeControl(Control* control);

	template<class Archive>
	void serialize(Archive & ar) {
		Container::serialize(ar);
		ar & make_nvp("Controls", m_Controls);
	}	
		
protected:

private:
	ControlVector m_Controls;
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_MULTICONTROLCONTAINER_H__
