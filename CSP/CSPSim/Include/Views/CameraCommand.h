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

class CameraCommand {
public:
	virtual void execute(CameraKinematics* ck) = 0;
	virtual ~CameraCommand() {}
};


class CameraResetCommand: public CameraCommand {
public:
	virtual void execute(CameraKinematics* ck) { ck->reset(); }
};


class PanLeftCommand: public CameraCommand {
public:
	virtual void execute(CameraKinematics* ck) { ck->panLeft(); }
};


class PanRightCommand: public CameraCommand {
public:
	virtual void execute(CameraKinematics* ck) { ck->panRight(); }
};


class PanLeftRightStopCommand: public CameraCommand {
public:
	virtual void execute(CameraKinematics* ck) { ck->panLeftRightStop(); }
};


class PanUpCommand: public CameraCommand {
public:
	virtual void execute(CameraKinematics* ck) { ck->panUp(); }
};

class PanDownCommand: public CameraCommand {
public:
	virtual void execute(CameraKinematics* ck) { ck->panDown(); }
};


class PanUpDownStopCommand: public CameraCommand {
public:
	virtual void execute(CameraKinematics* ck) { ck->panUpDownStop(); }
};


class ZoomInCommand: public CameraCommand {
public:
	virtual void execute(CameraKinematics* ck) { ck->zoomIn(); }
};


class ZoomOutCommand: public CameraCommand {
public:
	virtual void execute(CameraKinematics* ck) { ck->zoomOut(); }
};


class ZoomStopCommand: public CameraCommand {
public:
	virtual void execute(CameraKinematics* ck) { ck->zoomStop(); }
};

class ZoomStepInCommand: public CameraCommand {
public:
	virtual void execute(CameraKinematics* ck) { ck->zoomStepIn(); }
};


class ZoomStepOutCommand: public CameraCommand {
public:
	virtual void execute(CameraKinematics* ck) { ck->zoomStepOut(); }
};

class MouseCommand: public CameraCommand {
	int m_x, m_y, m_dx, m_dy;
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
	void set(int x, int y, int dx, int dy) {
		m_x = x;
		m_y = y;
		m_dx = dx;
		m_dy = dy;
	}
	virtual void execute(CameraKinematics* ck) {
		ck->displacement(m_x, m_y, m_dx, m_dy);
		reset();
	}
};

struct CameraCommands {
	MouseCommand Mouse;
	PanUpCommand PanUp;
	PanDownCommand PanDown;
	PanLeftCommand PanLeft;
	PanRightCommand PanRight;
	PanLeftRightStopCommand PanLeftRightStop;
	PanUpDownStopCommand PanUpDownStop;
	ZoomInCommand ZoomIn;
	ZoomOutCommand ZoomOut;
	ZoomStopCommand ZoomStop;
	ZoomStepInCommand ZoomStepIn;
	ZoomStepOutCommand ZoomStepOut;
	CameraResetCommand CameraReset;
};

#endif //__CAMERACOMMAND_H__

