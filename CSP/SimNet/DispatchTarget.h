// Combat Simulator Project
// Copyright (C) 2004 The Combat Simulator Project
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
 * @file DispatchTarget.h
 *
 * @see DispatchManager.h
 */

#ifndef __SIMNET_DISPATCHTARGET_H__
#define __SIMNET_DISPATCHTARGET_H__


#include <SimNet/Callbacks.h>


/** Begin a message dispatch declaration.  This macro should be followed by
 *  zero or more SIMNET_DISPATCH macros, and finially a SIMNET_DISPATCH_END
 *  macro.
 *
 *  @param CLASS the current class.
 *  @param PARANT the base class that is a subclass of DispatchTarget.
 */
#define SIMNET_DECLARE_DISPATCH(CLASS, PARENT) \
	virtual bool dispatch(simnet::DispatchManager *helper) { \
		typedef CLASS Class; \
		typedef PARENT Parent; \
		switch (helper->id()) {

/** Declare a message handler.
 *
 *  @param MSG the message class
 *  @param HANDLER the method to handle this message.  The handler must
 *    return void and take a single argument of type (MSG::Ref const &).
 */
#define SIMNET_DISPATCH(MSG, HANDLER) \
			case MSG::_Id: \
				return helper->handle<Class, MSG>(this, &Class::HANDLER);

/** End a message dispatch declaration.
 */
#define SIMNET_DISPATCH_END \
			default: break; \
		} \
		return Parent::dispatch(helper); \
	}


namespace simnet {


/** An interface for objects receiving messages from a DispatchManager.
 *  In each subclass, use a SIMNET_DECLARE_DISPATCH block to bind handler
 *  methods to specific message types.
 */
class DispatchTarget {
	friend class DispatchManager;
protected:
	/** This method is called by DispatchManager::dispatch to pass a
	 *  message to the various internal dispatch functions created by
	 *  the SIMNET_DECLARE_DISPATCH blocks (which extend this method).
	 *  Do not call this method directly except from dispatchChildren.
	 *
	 *  @param manager the DispatchManager controlling the message dispatch.
	 *  @return true if the message was handled, false otherwise.
	 */
	virtual bool dispatch(DispatchManager *manager);

	/** This method is called when ordinary dispatch fails.  Extend this
	 *  method to pass messages on to child objects by calling their
	 *  dispatch methods directly (i.e. do not call manager->dispatch
	 *  for child objects).  If any child handles the message, return
	 *  true.  Otherwise return the result of a call the dispatchChildren
	 *  method of the immediate superclass.
	 *
	 *  @param manager the DispatchManager controlling the message dispatch.
	 *  @return true if the message was handled, false otherwise.
	 */
	virtual bool dispatchChildren(DispatchManager *manager);
};


}  // namespace simnet

#endif // __SIMNET_DISPATCHTARGET_H__


