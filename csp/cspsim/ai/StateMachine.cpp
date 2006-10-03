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


#include <csp/cspsim/ai/StateMachine.h>
#include <csp/csplib/util/Signal.h>

#include <cassert>
#include <map>

namespace {
const std::string EmtpyString;
typedef sigc::slot<void> Callback;
}


CSP_NAMESPACE

struct StateMachine::Handler {
	Handler(std::string const &name, Callback const &callback): name(name), callback(callback) {}
	std::string name;
	Callback callback;
};

struct StateMachine::Impl {
	typedef std::map<int, Handler*> HandlerMap;

	Impl(): state(-1), handler(0) {}

	int state;
	Handler *handler;
	HandlerMap handlers;
};


StateMachine::StateMachine(): m_Impl(new Impl) { }

StateMachine::~StateMachine() {
	for (Impl::HandlerMap::iterator iter = m_Impl->handlers.begin(); iter != m_Impl->handlers.end(); ++iter) {
		delete iter->second;
	}
	delete m_Impl;
}

void StateMachine::addHandler(int state, const slot<void> &handler, std::string const &name) {
	m_Impl->handlers[state] = new Handler(name, handler);
}

void StateMachine::setState(int state) {
	if (m_Impl->state == state) return;
	m_Impl->state = state;
	Impl::HandlerMap::iterator iter = m_Impl->handlers.find(state);
	assert(iter != m_Impl->handlers.end());
	m_Impl->handler = iter->second;
}

bool StateMachine::update() {
	if (m_Impl->handler) m_Impl->handler->callback();
	return m_Impl->handler != 0;
}

int StateMachine::state() const { return m_Impl->state; }

std::string const &StateMachine::stateName() const {
	return !m_Impl->handler ? EmtpyString : m_Impl->handler->name;
}


CSP_NAMESPACE_END


