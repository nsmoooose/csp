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


#ifndef __CSPSIM_AI_TASK_H__
#define __CSPSIM_AI_TASK_H__

#include <csp/csplib/util/Ref.h>
#include <csp/csplib/util/ScopedPointer.h>
#include <csp/csplib/util/SignalFwd.h>

CSP_NAMESPACE

class StateMachine;

class Task: public Referenced {
public:
	typedef enum {
		RUNNING,
		SUCCESS,
		FAILURE,
		CANCEL
	} Status;

	/// Update the task for the current time interval (frame).
	virtual void update(double dt);

	/// Returns true if the task is complete.  Check status() or one
	/// of succeeded, cancelled, or failed to determine the exit
	/// status.  Most tasks can continue to be updated for at least a
	/// short time after they are done, but in general a new task
	/// should be scheduled in their place as soon as possible.
	bool done() const { return m_Status != RUNNING; }

	/// True if the task completed successfully.  False indicates
	/// that the task is either still running, or has finished
	/// unsucessfully.  See done().
	bool succeded() const { return m_Status == SUCCESS; }

	/// True if the task has been cancelled.  False indicates
	/// that the task is either still running, or has finished
	/// in some other way.  See done().
	bool cancelled() const { return m_Status == CANCEL; }

	/// True if the task failed.  False indicates that the task is
	/// either still running, or has finished in some other way.  See
	/// done().
	bool failed() const { return m_Status == FAILURE; }

	/// Return the current status of the task.
	Status status() const { return m_Status; }

	/// If the task is running, ends the task and sets its status to
	/// CANCEL.
	virtual void cancel() { if (m_Status == RUNNING) m_Status = CANCEL; }

	/// Gets the name of the task and the name of the active state.
	virtual std::string stateName() const;

protected:
	Task(const char *name);
	virtual ~Task();

	template <class TASK>
	void addHandler(int state, void (TASK::*method)(), std::string const &name);

	double dt() const { return m_dt; }
	double elapsed() const { return m_ElapsedTime; }
	void resetElapsedTime() { m_ElapsedTime = 0.0; }
	bool initial() const { return m_NewState; }

	void next(int state) { m_NextState = state; }

	void fail() { if (m_Status == RUNNING) m_Status = FAILURE; }
	void succeed() { if (m_Status == RUNNING) m_Status = SUCCESS; }

	/// Hook that is called immediately before the update handler.
	virtual void preupdate() {}

	/// Hook that is called immediately after the update handler.
	virtual void postupdate() {}

	/// Hook that is called the first time the task is updated.
	virtual void init() {}

	/// A flag that is set true for one update every second.  Can be
	/// used to trigger periodic diagnostic output.
	bool debug() const { return m_DebugFlag; }

	template <class CLASS>
	void override(Task *task, void (CLASS::*method)(Status));

	virtual void override(Task *task);

private:
	bool advance();
	void addHandler(int state, const slot<void> &handler, std::string const &name);
	void setOverrideHandler(const slot<void, Status> &handler);

	const char *m_Name;

	Status m_Status;
	ScopedPointer<StateMachine> m_StateMachine;

	Ref<Task> m_OverrideTask;

	typedef slot<void, Status> OverrideDoneHandler;
	ScopedPointer<OverrideDoneHandler> m_OverrideDoneHandler;

	double m_dt;
	double m_ElapsedTime;

	int m_NextState;
	bool m_NewState;
	bool m_FirstUpdate;

	bool m_DebugFlag;
	double m_DebugTime;
};

CSP_NAMESPACE_END

#endif // __CSPSIM_AI_TASK_H__

