// Combat Simulator Project
// Copyright (C) 2004 The Combat Simulator Project
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


/**
 * @file IndexServer.h
 *
 **/

#ifndef __INDEXSERVER_H__
#define __INDEXSERVER_H__

#include <SimData/Ref.h>

namespace simnet {
	class Server;
}

class GlobalBattlefield;


class IndexServer {
public:
	IndexServer();
	~IndexServer();
	void run();

private:
	void initialize();
	void initPrimaryInterface();

	simdata::Ref<simnet::Server> m_NetworkServer;
	simdata::Ref<GlobalBattlefield> m_Battlefield;
};


#endif // __INDEXSERVER_H__
