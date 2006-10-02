// Combat Simulator Project
// Copyright (C) 2006 The Combat Simulator Project
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


#ifndef __CSPSIM_AI_AIRCRAFTTASK_H__
#define __CSPSIM_AI_AIRCRAFTTASK_H__

#include <csp/cspsim/ai/Task.h>

CSP_NAMESPACE

class AircraftControl;
class Runway;

class AircraftTask: public Task {
public:
	void bind(AircraftControl *ai);
	void setRunway(Runway const *runway);
protected:
	AircraftTask(const char *name);
	virtual ~AircraftTask();

	AircraftControl *ai() { return m_AircraftControl.get(); }
	Runway const *runway() { return m_Runway.get(); }

	bool checkDive();
	virtual void init();

private:
	Ref<AircraftControl> m_AircraftControl;
	Ref<const Runway> m_Runway;
};


CSP_NAMESPACE_END

#endif // __CSPSIM_AI_AIRCRAFTTASK_H__

