// Combat Simulator Project - FlightSim Demo
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
 * @file Controller.h
 *
 **/


#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include <Message.h>
#include <System.h>


/** Interface for controlling dynamic objects.
 *
 *  Used for both AI control and remote (network) control.
 */
class Controller: public System {

public:
	Controller() {}
	virtual ~Controller() {}

	virtual void onMessage(const Message *) = 0;
	virtual void doControl(double dt) = 0;
	virtual void initialize() = 0;

};

#endif // __CONTROLLER_H__

