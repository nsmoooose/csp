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
 * @file DamageModifier.h
 *
 **/


#ifndef __DAMAGEMODIFIER_H__
#define __DAMAGEMODIFIER_H__


#include <SimData/Object.h>
#include <SimData/InterfaceRegistry.h>


/**
 * clss DamageModifier (STATIC)
 *
 * Damage modifiers reflect the resistance of an object to 
 * various types of weapons.
 *
 */
class DamageModifier: public simdata::Object { 
public:
	char m_Incendiary;
	char m_HighExplosive;
	char m_Penetrating;
	char m_ArmorPiercing;
	char m_SmallArms;
	// etc
	
	SIMDATA_OBJECT(DamageModifier, 0, 0)

	BEGIN_SIMDATA_XML_INTERFACE(DamageModifier)
		SIMDATA_XML("incediary", DamageModifier::m_Incendiary, false)
		SIMDATA_XML("high_explosive", DamageModifier::m_HighExplosive, false)
		SIMDATA_XML("penetrating", DamageModifier::m_Penetrating, false)
		SIMDATA_XML("armor_piercing", DamageModifier::m_ArmorPiercing, false)
		SIMDATA_XML("small_arms", DamageModifier::m_SmallArms, false)
	END_SIMDATA_XML_INTERFACE

	DamageModifier() {}
	virtual ~DamageModifier() {}

	virtual void serialize(simdata::Archive& archive) {
		Object::serialize(archive);
		archive(m_Incendiary);
		archive(m_HighExplosive);
		archive(m_Penetrating);
		archive(m_ArmorPiercing);
		archive(m_SmallArms);
	}

	virtual void postCreate() {}
};


#endif  // __DAMAGEMODIFIER_H__

