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
 * @file Container.h
 *
 **/

#ifndef __CSPSIM_WF_CONTAINER_H__
#define __CSPSIM_WF_CONTAINER_H__

#include <csp/cspwf/Control.h>

namespace csp {
namespace wf {

/** Base class for all controls that can have new child controls.
 * By inheriting from this class you can extend normal controls to have
 * child controls. By implementing layoutChildControls you make it possible
 * to control the positioning behaviour of your children.
 */
class CSPWF_EXPORT Container : public Control {
public:
	Container(std::string name);
	virtual ~Container();
	
	virtual void layoutChildControls() = 0;
	virtual ControlVector getChildControls() = 0;
	
	virtual Rectangle getClientRect() const;
	
	virtual Control* getById(const std::string& id);
	template<class T>
	T* getById(const std::string& id) {
		return dynamic_cast<T*>(getById(id));
	}
	
protected:

private:
};

} // namespace wf
} // namespace csp

#endif // __CSPSIM_WF_CONTAINER_H__


