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


/**
 * @file NetworkAddress.cpp:
 * @author Scott Flicker (Wolverine)
 *
 * This class encapsulates a node in the simulation network.
 */
#include "Networking.h"

NetworkAddress::NetworkAddress()
{
    m_name = "hostname";
    m_IPAddress = "0.0.0.0";
}

NetworkAddress::NetworkAddress(const simdata::String name)
{

}

NetworkAddress::NetworkAddress(NetworkAddress & addr)
{

}

simdata::String NetworkAddress::getNetworkName()
{
  //FIXME this needs to be not hardcoded.
    return m_name;
}

simdata::String NetworkAddress::getNetworkIP()
{
    return m_IPAddress;
}

void NetworkAddress::setByNetworkName(const simdata::String & name)
{

}

void NetworkAddress::setByNetworkIP(const simdata::String & address)
{

}

