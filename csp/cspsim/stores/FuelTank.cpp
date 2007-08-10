// Combat Simulator Project
// Copyright (C) 2007 The Combat Simulator Project
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
 * @file FuelTank.cpp
 *
 **/

#include <csp/cspsim/CSPSim.h>
#include <csp/cspsim/DynamicObject.h>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspsim/stores/FuelTank.h>
#include <csp/cspsim/stores/Stores.h>
#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/data/DataManager.h>


CSP_NAMESPACE

CSP_XML_BEGIN(FuelTankData)
	CSP_DEF("capacity", m_Capacity, true)
CSP_XML_END

Store *FuelTankData::createStore() const {
	return new FuelTank(this);
}

CSP_NAMESPACE_END

