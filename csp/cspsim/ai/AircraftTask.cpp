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


#include <csp/cspsim/ai/AircraftTask.h>
#include <csp/cspsim/ai/AircraftControl.h>
#include <csp/cspsim/ai/DiveRecovery.h>
#include <csp/cspsim/ai/Runway.h>
#include <csp/cspsim/ai/Task.inl>

CSP_NAMESPACE

AircraftTask::AircraftTask(const char *name): Task(name) {
}

AircraftTask::~AircraftTask() {
}

void AircraftTask::bind(AircraftControl *ai) {
	m_AircraftControl = ai;
}

void AircraftTask::setRunway(Runway const *runway) {
	m_Runway = runway;
}

void AircraftTask::init() {
	assert(ai());
}

bool AircraftTask::checkDive() {
	if (ai()->pullupAdvisory()) {
		AircraftTask *task = new DiveRecovery;
		task->bind(ai());
		override(task);
		return true;
	}
	return false;
}

CSP_NAMESPACE_END

