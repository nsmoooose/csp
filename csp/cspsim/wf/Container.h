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

#include <csp/cspsim/wf/Control.h>

CSP_NAMESPACE

namespace wf {

class Container : public Control {
public:
	Container(Theme* theme);
	virtual ~Container();
	
	virtual void layoutChildControls() = 0;
	
	virtual ControlVector getChildControls() = 0;
	
	template<class T>
	T* getById(const std::string& id) {
		return dynamic_cast<T*>(internalGetById(id));
	}
	
protected:

private:
	virtual Control* internalGetById(const std::string& id);
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_CONTAINER_H__


