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
 * @file DamageModifier.h
 *
 **/


#ifndef __CSPSIM_DAMAGEMODIFIER_H__
#define __CSPSIM_DAMAGEMODIFIER_H__

#include <csp/csplib/data/Object.h>

namespace csp {

/**
 * @class DamageModifier DamageModifier.h "csp/cspsim/DamageModifier.h"
 * DamageModifier (STATIC)
 * Damage modifiers reflect the resistance of an object to
 * various types of weapons.
 *
 * @todo add more modifiers if needed
 * @todo depreciate m_SmallArms in favor of more specific small arms options.
 */
class DamageModifier: public Object {
public:
	char m_Incendiary;
	char m_HighExplosive;
	char m_Penetrating;
	char m_ArmorPiercing;
	char m_SmallArms;
	char m_HighExplosiveAntiTank;
	char m_Sabot;
	char m_SmallArmsSoftPoint;
	char m_SmallArmsFullMetalJacket;
	char m_SmallArmsArmorPiercing;
	char m_SmallArmsExplosive;;
	char m_SmallArmsHydroshock;
	char m_GenericSmallArms;
	char m_LessThanLethalBluntForce;
	char m_LessThanLethalElectricity;
	char m_LessThanLethalChemical;
	char m_LessThanLethalSonic;
	char m_GenericLethalChemical;
	char m_Radiological;
	char m_BluntImpact;
	char m_PiercingImpact;
	char m_Shockwave;
	char m_Shrapnel;
	char m_Biological;
	char m_Electromagnetic;
	char m_MechanicalFlooding;
	char m_MaterialOverstress;
	char m_Asphyxiation;
	char m_Bleeding;
	char m_Drowning;
	char m_ElectricShock;
	// etc
	
	CSP_DECLARE_STATIC_OBJECT(DamageModifier)

	DamageModifier() {}
	virtual ~DamageModifier() {}

	virtual void postCreate() {}
};

} // namespace csp

#endif  // __DAMAGEMODIFIER_H__

