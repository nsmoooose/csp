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


#include <csp/cspsim/ai/Task.h>
#include <csp/cspsim/ai/Task.inl>
#include <csp/cspsim/ai/StateMachine.h>
#include <sigc++/functors/slot.h>

namespace csp {
namespace ai {

Task::Task(const char *name):
		m_Name(name),
		m_Status(RUNNING),
		m_StateMachine(new StateMachine),
		m_OverrideDoneHandler(new slot<void, Status>),
		m_dt(0.0f),
		m_ElapsedTime(0.0f),
		m_NextState(0),
		m_NewState(false),
		m_FirstUpdate(true),
		m_DebugFlag(false),
		m_DebugTime(0.0f)
{
}

Task::~Task() {
}

void Task::update(double dt) {
	// To aid with debugging new tasks, m_DebugFlag is set to true
	// for one update every one second.  Task update handlers can
	// used the debug() accessor to periodically write diagnostic
	// data to stdout.
	m_DebugTime += dt;
	m_DebugFlag = false;
	if (m_DebugTime >= 1.0) {
		m_DebugFlag = true;
		m_DebugTime = 0.0;
	}

	// Provide a hook to allow task subclasses to implement custom
	// initialization code when the task is updated for the first
	// time.
	if (m_FirstUpdate) {
		m_FirstUpdate = false;
		init();
	}

	// If there is an override task, delegate the update to it.
	// Otherwise update our state machine (which typically calls
	// back into one of the subclass methods).
	if (!m_OverrideTask) {
		m_dt = dt;
		preupdate();
		m_StateMachine->update();
		postupdate();
		m_NewState = false;
		m_ElapsedTime += dt;
	} else {
		m_OverrideTask->update(dt);
		if (!m_OverrideTask->done()) {
			return;
		}
		if (*m_OverrideDoneHandler) {
			(*m_OverrideDoneHandler)(m_OverrideTask->status());
		}
		m_OverrideTask = 0;
		m_OverrideDoneHandler->disconnect();
	}

	// Transition to the next state, if any.
	advance();
}

std::string Task::stateName() const {
	if (!m_OverrideTask) return m_Name + std::string(".") + m_StateMachine->stateName();
	return m_OverrideTask->stateName();
}

void Task::addHandler(int state, const slot<void> &handler, std::string const &name) {
	m_StateMachine->addHandler(state, handler, name);
}

void Task::setOverrideHandler(const slot<void, Status> &handler) {
	*m_OverrideDoneHandler = handler;
}

bool Task::advance() {
	if (m_NextState == m_StateMachine->state()) return false;
	m_StateMachine->setState(m_NextState);
	// TODO use AGENT instead of OBJECT; can we get the object id too?
	CSPLOG(Prio_DEBUG, Cat_OBJECT) << "State: " << m_StateMachine->stateName();
	m_NewState = true;
	resetElapsedTime();
	return true;
}

void Task::override(Task *task) {
	assert(task != this);
	assert(!m_OverrideTask);
	m_OverrideTask = task;
}

} // end namespace ai
} // end namespace csp
