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


#ifndef __CSPSIM_AI_STATEMACHINE_H__
#define __CSPSIM_AI_STATEMACHINE_H__

#include <csp/csplib/util/Properties.h>
#include <csp/csplib/util/SignalFwd.h>

#include <string>

CSP_NAMESPACE

class StateMachine: NonCopyable {
public:
	StateMachine();
	~StateMachine();

	void addHandler(int state, const slot<void> &handler, std::string const &name);

	void setState(int state);
	bool update();
	int state() const;
	std::string const &stateName() const;

private:
	struct Handler;
	struct Impl;
	Impl *m_Impl;
};

CSP_NAMESPACE_END

#endif // __CSPSIM_AI_STATEMACHINE_H__

