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

namespace csp {

CSP_XML_BEGIN(DamageModifier)
	CSP_DEF("incediary", m_Incendiary, false)
	CSP_DEF("high_explosive", m_HighExplosive, false)
	CSP_DEF("penetrating", m_Penetrating, false)
	CSP_DEF("armor_piercing", m_ArmorPiercing, false)
	CSP_DEF("small_arms", m_SmallArms, false)
	CSP_DEF("high_explosive_anti_tank", m_HighExplosiveAntiTank, false)
	CSP_DEF("sabot", m_Sabot, false)
	CSP_DEF("small_arms_soft_point", m_SmallArmsSoftPoint, false)
	CSP_DEF("small_arms_full_metal_jacket", m_SmallArmsFullMetalJacket, false)
	CSP_DEF("small_arms_armor_piercing", m_SmallArmsArmorPiercing, false)
	CSP_DEF("small_arms_explosive", m_SmallArmsExplosive, false)
	CSP_DEF("small_arms_hydroshock", m_SmallArmsHydroshock, false)
	CSP_DEF("less_than_lethal_blunt_force", m_LessThanLethalBluntForce, false)
	CSP_DEF("less_than_lethal_electricity", m_LessThanLethalElectricity, false)
	CSP_DEF("less_than_lethal_chemical", m_LessThanLethalChemical, false)
	CSP_DEF("less_than_lethal_sonic", m_LessThanLethalSonic, false)
	CSP_DEF("generic_lethal_chemical", m_GenericLethalChemical, false)
	CSP_DEF("radiological", m_Radiological, false)
	CSP_DEF("blunt_impact", m_BluntImpact, false)
	CSP_DEF("piercing_impact", m_PiercingImpact, false)
	CSP_DEF("shockwave", m_Shockwave, false)
	CSP_DEF("shrapnel", m_Shrapnel, false)
	CSP_DEF("biological", m_Biological, false)
	CSP_DEF("electromagnetic", m_Electromagnetic, false)
	CSP_DEF("mechanical_flooding", m_MechanicalFlooding, false)
	CSP_DEF("material_overstress", m_MaterialOverstress, false)
	CSP_DEF("asphyxiation", m_Asphyxiation, false)
	CSP_DEF("bleeding", m_Bleeding, false)
	CSP_DEF("drowning", m_Drowning, false)
	CSP_DEF("electric_shock", m_ElectricShock, false)
CSP_XML_END

} // namespace csp

