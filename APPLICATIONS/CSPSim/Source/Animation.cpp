// Combat Simulator Project - CSPSim
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
 * @file Animation.cpp
 *
 **/


#include "Animation.h"
#include "Log.h"
#include "Config.h"

SIMDATA_REGISTER_INTERFACE(DrivenRotation)


Animation::Animation():
	m_LOD(0),
	m_Default(0.0f)
{
}

Animation::~Animation()
{
}

void Animation::pack(simdata::Packer& p) const
{
	Object::pack(p);
	p.pack(m_ModelID);
	p.pack(m_ControlID);
	p.pack(m_LOD);
	p.pack(m_Default);
}

void Animation::unpack(simdata::UnPacker& p)
{
	Object::unpack(p);
	p.unpack(m_ModelID);
	p.unpack(m_ControlID);
	p.unpack(m_LOD);
	p.unpack(m_Default);
}

DrivenRotation::DrivenRotation(): 
	m_Axis(0.0f, 0.0f, 0.0f),
	m_Limit0(-G_PI), 
	m_Limit1(G_PI),
	m_Gain(1.0f),
	m_Offset(0.0f)
{
}


DrivenRotation::~DrivenRotation()
{
}

void DrivenRotation::pack(simdata::Packer& p) const
{
	Animation::pack(p);
	p.pack(m_Limit0);
	p.pack(m_Limit1);
	p.pack(m_Gain);
	p.pack(m_Offset);
	p.pack(m_Axis);
}

void DrivenRotation::unpack(simdata::UnPacker& p)
{
	Animation::unpack(p);
	p.unpack(m_Limit0);
	p.unpack(m_Limit1);
	p.unpack(m_Gain);
	p.unpack(m_Offset);
	p.unpack(m_Axis);
}

void DrivenRotation::postCreate() 
{
}
