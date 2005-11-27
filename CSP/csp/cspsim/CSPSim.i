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


%{
#include <csp/cspsim/CSPSim.h>
#include <csp/cspsim/Theater.h>
#include <csp/cspsim/Projection.h>
#include <csp/cspsim/Shell.h>
#include <csp/cspsim/DynamicObject.h>
#include <csp/csplib/battlefield/LocalBattlefield.h>
#include <csp/csplib/data/DataManager.h>
#include <csp/csplib/data/Math.h>

void _createVehicleHelper(CSPSim *self, const char *path, simdata::Vector3 position,
                          simdata::Vector3 velocity, simdata::Vector3 attitude) {
	simdata::Ref<DynamicObject> obj = self->getDataManager().getObject(path);
	if (!obj) {
		std::cout << "WARNING: Failed to create object '" << path << "'\n";
		return;
	}
	obj->setGlobalPosition(position);
	obj->setVelocity(velocity);
	simdata::Quat q_attitude;
	attitude *= 3.1416 / 180.0;
	q_attitude.makeRotate(attitude.x(), attitude.y(), -attitude.z());
	obj->setAttitude(q_attitude);
	self->getBattlefield()->__test__addLocalHumanUnit(obj);
	if (!self->getActiveObject()) self->setActiveObject(obj);
}

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
	void setActiveObject(simdata::Ref<DynamicObject> object);
	simdata::Ref<DynamicObject> const getActiveObject() const;
	Battlefield * const getBattlefield() const;
	void togglePause();
	void runConsole(PyConsole *console);
	void endConsole();
	bool isPaused() { return m_Paused; }
	simdata::DataManager & getDataManager() { return m_DataManager; }
};


%exception CSPSim::createVehicle {
	try {
		$action
	} catch (simdata::Exception &e) {
	std::string msg = e.getType() + ": " + e.getMessage();
		PyErr_SetString(PyExc_RuntimeError, msg.c_str());
		SWIG_fail;
	} catch (...) {
		PyErr_SetString(PyExc_RuntimeError, "Caught an (unknown) exception in CSPSim::createVehicle");
		SWIG_fail;
	}
}

%extend CSPSim {
	void createVehicle(const char *path, simdata::Vector3 position,
	                   simdata::Vector3 velocity, simdata::Vector3 attitude) {
		_createVehicleHelper(self, path, position, velocity, attitude);
	}
	void createVehicle(const char *path, simdata::LLA lla,
	                   simdata::Vector3 velocity, simdata::Vector3 attitude) {
		simdata::Ref<const Projection> map = CSPSim::theSim->getTheater()->getTerrain()->getProjection();
		simdata::Vector3 position = map->convert(lla);
		_createVehicleHelper(self, path, position, velocity, attitude);
	}
	void setShell(PyObject *shell) { self->getShell()->bind(shell); }
	std::string const &getTerrainName() { return self->getTheater()->getTerrain()->getName(); }
	int getTerrainVersion() { return self->getTheater()->getTerrain()->getVersion(); }
}

%exception;

