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

#include <Animation.h>
#include <Station.h>
#include <ObjectModel.h>

#include <csp/lib/data/ObjectInterface.h>


SIMDATA_XML_BEGIN(Station)
	SIMDATA_DEF("name", m_Name, true)
	SIMDATA_DEF("detail_model", m_DetailModel, false)
	SIMDATA_DEF("mask_names", m_MaskNames, false)
	//SIMDATA_DEF("view_model", m_ViewModel, false)  // TODO
SIMDATA_XML_END


Station::Station(): m_Mask(0) {}
Station::~Station() {}

SceneModelChild *Station::createDetailModel() const {
	return !m_DetailModel ? 0 : new SceneModelChild(m_DetailModel);
}

