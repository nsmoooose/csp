// Combat Simulator Project - FlightSim Demo
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
 * @file CameraCommand.h
 *
 **/

#ifndef __CAMERACOMMAND_H__
#define __CAMERACOMMAND_H__

#include "Views/CameraKinematics.h"

class Command {
public:
	virtual void execute() = 0;
	virtual ~Command(){}
};


class CameraCommand: public Command {
protected:
	CameraKinematics* m_CameraKinematics;
public:
	CameraCommand(): m_CameraKinematics(0){}
	void setCameraKinematics(CameraKinematics* cm) {
		m_CameraKinematics = cm;
	}
	virtual ~CameraCommand(){}
};


class CameraReset: public CameraCommand {
public:
	virtual void execute() {
		m_CameraKinematics->reset();
	}
	virtual ~CameraReset(){}
};


class PanLeft: public CameraCommand {
public:
	virtual void execute() {
		m_CameraKinematics->panLeft();
	}
	virtual ~PanLeft(){}
};


class PanRight: public CameraCommand {
public:
	virtual void execute() {
		m_CameraKinematics->panRight();
	}
	virtual ~PanRight(){}
};


class PanLeftRightStop: public CameraCommand {
public:
	virtual void execute() {
		m_CameraKinematics->panLeftRightStop();
	}
	virtual ~PanLeftRightStop(){}
};


class PanUp: public CameraCommand {
public:
	virtual void execute() {
		m_CameraKinematics->panUp();
	}
	virtual ~PanUp(){}
};

class PanDown: public CameraCommand {
public:
	virtual void execute() {
		m_CameraKinematics->panDown();
	}
	virtual ~PanDown(){}
};


class PanUpDownStop: public CameraCommand {
public:
	virtual void execute() {
		m_CameraKinematics->panUpDownStop();
	}
	virtual ~PanUpDownStop(){}
};


class ZoomIn: public CameraCommand {
public:
	virtual void execute() {
		m_CameraKinematics->zoomIn();
	}
	virtual ~ZoomIn(){}
};


class ZoomOut: public CameraCommand {
public:
	virtual void execute() {
		m_CameraKinematics->zoomOut();
	}
	virtual ~ZoomOut(){}
};


class ZoomStop: public CameraCommand {
public:
	virtual void execute() {
		m_CameraKinematics->zoomStop();
	}
	virtual ~ZoomStop(){}
};

class MouseCommand: public CameraCommand {
	int m_x,m_y,m_dx,m_dy;
	void reset() {
		m_x = 0;
		m_y = 0;
		m_dx = 0;
		m_dy = 0;
	}
public:
	MouseCommand() {
		  reset();
	 }
	void set(int x,int y, int dx, int dy) {
		m_x = x;
		m_y = y;
		m_dx = dx;
		m_dy = dy;
	}
	virtual void execute() {
		m_CameraKinematics->displacement(m_x,m_y,m_dx,m_dy);
		reset();
	}
	virtual ~MouseCommand(){}
};

#endif //__CAMERACOMMAND_H__

