// Combat Simulator Project
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
 * @file DamageModifier.cpp
 *
 **/

#include <csp/cspsim/DamageModifier.h>
#include <csp/csplib/data/ObjectInterface.h>

CSP_NAMESPACE

CSP_XML_BEGIN(DamageModifier)
	CSP_DEF("incediary", m_Incendiary, false)
	CSP_DEF("high_explosive", m_HighExplosive, false)
	CSP_DEF("penetrating", m_Penetrating, false)
	CSP_DEF("armor_piercing", m_ArmorPiercing, false)
	CSP_DEF("small_arms", m_SmallArms, false)
CSP_XML_END

CSP_NAMESPACE_END
