// Combat Simulator Project
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file Station.cpp
 *
 **/

#include <csp/cspsim/Station.h>
#include <csp/cspsim/Animation.h>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspsim/SceneModel.h>

#include <csp/csplib/data/ObjectInterface.h>

namespace csp {

CSP_XML_BEGIN(Station)
	CSP_DEF("name", m_Name, true)
	CSP_DEF("detail_model", m_DetailModel, false)
	CSP_DEF("mask_names", m_MaskNames, false)
	/** @TODO m_ViewModel variable to determine if the station model is visible or not */
	//CSP_DEF("view_model", m_ViewModel, false) // TODO
CSP_XML_END


Station::Station(): m_Mask(0) {}
Station::~Station() {}

SceneModelChild *Station::createDetailModel() const {
	return !m_DetailModel ? 0 : new SceneModelChild(m_DetailModel);
}

} // namespace csp

