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
#include <csp/cspsim/CSPSim.h>
#include <csp/cspsim/Theater.h>
#include <csp/cspsim/Projection.h>
#include <csp/cspsim/DynamicObject.h>
#include <csp/cspsim/battlefield/LocalBattlefield.h>
#include <csp/csplib/data/DataManager.h>
#include <csp/csplib/util/Math.h>
#include <iostream>

void _createVehicleHelper(csp::CSPSim *self, const char *path, csp::Vector3 position,
                          csp::Vector3 velocity, csp::Vector3 attitude, bool human) {
	csp::Ref<csp::DynamicObject> obj = self->getDataManager().getObject(path);
	if (!obj) {
		std::cout << "WARNING: Failed to create object '" << path << "'\n";
		return;
	}
	obj->setGlobalPosition(position);
	obj->setVelocity(velocity);
	csp::Quat q_attitude;
	attitude *= 3.1416 / 180.0;
	q_attitude.makeRotate(attitude.x(), attitude.y(), -attitude.z());
	obj->setAttitude(q_attitude);
	self->getBattlefield()->__test__addLocalHumanUnit(obj, human);
	if (!self->getActiveObject()) self->setActiveObject(obj);
}

%}

namespace csp {

// %newobject CSPSim::getConfiguration();

class CSPSim {
public:
	static csp::CSPSim *theSim;

	CSPSim();
	virtual ~CSPSim();
	virtual void init();
	virtual void loadSimulation();
	virtual void unloadSimulation();
	virtual void displayLogoScreen();
	virtual void displayMenuScreen();
	virtual void run();
	virtual void quit();
	virtual void cleanup();

	virtual wf::WindowManager* getWindowManager();

	void changeScreen(csp::BaseScreen * newScreen);
	csp::BaseScreen* getCurrentScreen();
	
	virtual config::Configuration* getConfiguration();
	virtual void setConfiguration(config::Configuration* config); 
	virtual wf::Signal* getConfigurationChangedSignal();


	csp::SimDate & getCurrentTime();
	void setCurrentTime(csp::SimDate& date);
	csp::SimTime const & getFrameRate() const;
	void setActiveObject(csp::Ref<csp::DynamicObject> object);
	csp::Ref<csp::DynamicObject> const getActiveObject() const;
	csp::Battlefield * const getBattlefield() const;
	void togglePause();
	bool isPaused();
	csp::DataManager & getDataManager();
};

%exception CSPSim::createVehicle {
	try {
		$action
	} catch (csp::Exception &e) {
	std::string msg = e.getType() + ": " + e.getMessage();
		PyErr_SetString(PyExc_RuntimeError, msg.c_str());
		SWIG_fail;
	} catch (...) {
		PyErr_SetString(PyExc_RuntimeError, "Caught an (unknown) exception in CSPSim::createVehicle");
		SWIG_fail;
	}
}

%extend CSPSim {
	void createVehicle(const char *path, csp::Vector3 position,
	                   csp::Vector3 velocity, csp::Vector3 attitude, bool human) {
		_createVehicleHelper(self, path, position, velocity, attitude, human);
	}
	void createVehicle(const char *path, csp::LLA lla,
	                   csp::Vector3 velocity, csp::Vector3 attitude, bool human) {
		csp::Ref<const csp::Projection> map = csp::CSPSim::theSim->getTheater()->getTerrain()->getProjection();
		csp::Vector3 position = map->convert(lla);
		_createVehicleHelper(self, path, position, velocity, attitude, human);
	}
	std::string const &getTerrainName() { return self->getTheater()->getTerrain()->getName(); }
	int getTerrainVersion() { return self->getTheater()->getTerrain()->getVersion(); }
}

} // namespace csp


%exception;

