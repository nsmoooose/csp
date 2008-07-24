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
 * @file Missile.h
 *
 **/

#ifndef __CSPSIM_STORES_MISSILE_H__
#define __CSPSIM_STORES_MISSILE_H__

#include <csp/cspsim/stores/Stores.h>

namespace csp {

/** Data for a missiles.
 */
class MissileData: public StoreData {
public:
	CSP_DECLARE_STATIC_OBJECT(MissileData)

	MissileData(): StoreData(MISSILE) { }

	/** Create a Missile instance, returning a new pointer.
	 */
	virtual Store *createStore() const;
};


class Missile: public Store {
friend class MissileData;
public:
	virtual Missile const *asMissile() const { return this; }
	virtual Missile *asMissile() { return this; }

	/** Get the static data for this missile.
	 */
	virtual StoreData const *data() const { return m_Data.get(); }

private:
	explicit Missile(MissileData const *data): m_Data(data) { assert(data); }
	Ref<const MissileData> m_Data;
};

} // namespace csp

#endif // __CSPSIM_STORES_MISSILE_H__
