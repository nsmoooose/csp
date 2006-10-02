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


#ifndef __CSPSIM_AI_TASK_INL__
#define __CSPSIM_AI_TASK_INL__

#include <csp/cspsim/ai/Task.h>
#include <csp/csplib/util/Signal.h>

#include <sigc++/functors/mem_fun.h>

CSP_NAMESPACE

template <class TASK>
void Task::addHandler(int state, void (TASK::*method)(), std::string const &name) {
	addHandler(state, sigc::mem_fun(static_cast<TASK*>(this), method), name);
}

template <class CLASS>
void Task::override(Task *task, void (CLASS::*method)(Status)) {
	override(task);
	setOverrideHandler(sigc::mem_fun(static_cast<CLASS*>(this), method));
}

CSP_NAMESPACE_END

#endif // __CSPSIM_AI_TASK_INL__

