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


%{
#include "CSPSim.h"
#include <SimData/Math.h>
%}


class CSPSim
{
public:
	static CSPSim *theSim;
    
	CSPSim();
	virtual ~CSPSim();
	virtual void init();
	virtual void run();
	virtual void quit();
	virtual void cleanup();

	void changeScreen(BaseScreen * newScreen);
	SDL_Surface * getSDLScreen() {return m_SDLScreen;};
	simdata::SimDate & getCurrentTime() { return m_CurrentTime; }
	simdata::SimTime const & getFrameRate() const{ return m_FrameRate; }
	void setActiveObject(simdata::Pointer<DynamicObject> object);
	simdata::Pointer<DynamicObject> const getActiveObject() const;
	VirtualBattlefield * const getBattlefield() const;
	void setShell(PyObject *shell) { m_Shell.bind(shell); }
	EventMapIndex *getInterfaceMaps() { return m_InterfaceMaps; }
	void togglePause();
	void runConsole(PyConsole *console);
	void endConsole();
	bool isPaused() { return m_Paused; }
	simdata::DataManager & getDataManager() { return m_DataManager; }
};


%extend CSPSim {
	void createVehicle(const char *path, simdata::Vector3 position, 
	                   simdata::Vector3 velocity, simdata::Vector3 attitude) {
		simdata::Pointer<DynamicObject> obj = self->getDataManager().getObject(path);
		obj->setGlobalPosition(position);
		obj->setVelocity(velocity);
		simdata::Quaternion q_attitude;
		q_attitude = simdata::Quaternion::MakeQFromEulerAngles(attitude.x, attitude.y, attitude.z);
		obj->setAttitude(q_attitude);
		self->getBattlefield()->addObject(obj);
	}
}
