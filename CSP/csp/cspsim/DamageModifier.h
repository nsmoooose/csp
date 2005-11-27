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


#include <csp/lib/data/Object.h>


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
	
	SIMDATA_DECLARE_STATIC_OBJECT(DamageModifier)

	DamageModifier() {}
	virtual ~DamageModifier() {}

	virtual void postCreate() {}
};


#endif  // __DAMAGEMODIFIER_H__

