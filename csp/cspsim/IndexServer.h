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

#ifndef __CSPSIM_INDEXSERVER_H__
#define __CSPSIM_INDEXSERVER_H__

#include <csp/csplib/util/Ref.h>
#include <csp/cspsim/Export.h>

namespace csp {

class Server;
class GlobalBattlefield;


class CSPSIM_EXPORT IndexServer {
public:
	IndexServer();
	~IndexServer();
	void run();
	static int main();

private:
	void initialize();
	void initPrimaryInterface();

	Ref<Server> m_NetworkServer;
	Ref<GlobalBattlefield> m_Battlefield;

	IndexServer(const IndexServer&);
	IndexServer const &operator=(const IndexServer&);
};

} // namespace csp

#endif // __CSPSIM_INDEXSERVER_H__
