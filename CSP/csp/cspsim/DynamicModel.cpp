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
 * @file DynamicModel.cpp
 *
 * Provides an interface for vehicle subsystems to interact with the 3D world.
 **/


#include <csp/cspsim/DynamicModel.h>
#include <csp/cspsim/SceneModel.h>

CSP_NAMESPACE

void DynamicModel::signalCreateSceneModel(SceneModel *model) {
	assert(model);
	m_CreateSceneModelSignal.emit(model->getDynamicGroup());
}

CSP_NAMESPACE_END

