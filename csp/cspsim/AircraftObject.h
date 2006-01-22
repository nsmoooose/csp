// Combat Simulator Project
// Copyright (C) 2002-2005 The Combat Simulator Project
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
 * @file AircraftObject.h
 *
 **/


#ifndef __CSPSIM_AIRCRAFTOBJECT_H__
#define __CSPSIM_AIRCRAFTOBJECT_H__

#include <csp/cspsim/DynamicObject.h>
#include <csp/cspsim/HID.h>

CSP_NAMESPACE

class TimedSequence;

class AircraftObject: public DynamicObject
{
public:
	CSP_DECLARE_OBJECT(AircraftObject)

	DECLARE_INPUT_INTERFACE(AircraftObject, DynamicObject)
		BIND_ACTION("SMOKE_ON", SmokeOn);
		BIND_ACTION("SMOKE_OFF", SmokeOff);
		BIND_ACTION("SMOKE_TOGGLE", SmokeToggle);
		BIND_ACTION("MARKS_TOGGLE_DEPRECATED", MarkersToggle);
	END_INPUT_INTERFACE  // protected:

public:
	AircraftObject();
	virtual ~AircraftObject();

	// input event handlers
	void SmokeOn();
	void SmokeOff();
	void SmokeToggle();
	void MarkersToggle();

	virtual void dump();
	
	virtual void onRender();

	virtual void initialize();
	
	void setAttitude(double pitch, double roll, double heading);

	void getInfo(std::vector<std::string> &info) const;

	virtual void setDataRecorder(DataRecorder *recorder);

protected:

	virtual double onUpdate(double);
	void postUpdate(double dt);
	//void doPhysics(double dt);

	virtual void registerChannels(Bus* bus);
	virtual void bindChannels(Bus* bus);

	virtual void postCreate();
	virtual void convertXML();

	// dynamic properties
	DataChannel<double>::RefT b_Roll;
	DataChannel<double>::RefT b_Pitch;
	DataChannel<double>::RefT b_Heading;
};

CSP_NAMESPACE_END

#endif // __CSPSIM_AIRCRAFTOBJECT_H__

