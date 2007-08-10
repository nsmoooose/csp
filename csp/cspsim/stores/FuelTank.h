
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
 * @file FuelTanl.h
 *
 **/

// there might be some unneeded includes

#ifndef __CSPSIM_STORES_FUELTANK_H__
#define __CSPSIM_STORES_FUELTANK_H__

#include <csp/cspsim/stores/Stores.h>
#include <csp/cspsim/stores/StoresDynamics.h>

CSP_NAMESPACE

/** Data for a fuel tank (internal or external).
 */
class FuelTankData: public StoreData {
	// Fuel density:
	//  JP-5:  827 kg/m^3 = 0.827 kg/l at STP.
	//  JP-8:  800 kg/m^3 = 0.800 kg/l at STP.
public:
	CSP_DECLARE_STATIC_OBJECT(FuelTankData)

	FuelTankData(): StoreData(FUELTANK), m_Capacity(0.0) { }

	/** Create a fuel tank instance, returning a new pointer.
	 */
	virtual Store *createStore() const;

	/** Get the capacity in liters.
	 */
	double capacity() const { return m_Capacity; }

	/** Get the fuel density in kg/liter.
	 */
	double fuelDensity() const { return 0.800 /* kg/l */; }  // assume jp-8

private:
	double m_Capacity;  // l
};


/** Data for a fuel tank store (internal or external).
 */
class FuelTank: public Store {
friend class FuelTankData;
public:
	virtual FuelTank const *asFuelTank() const { return this; }
	virtual FuelTank *asFuelTank() { return this; }

	/** Get the mass of the tank, including fuel.
	 */
	virtual double mass() const { return m_Data->mass() + quantity() * m_Data->fuelDensity(); }

	/** Get the fuel density in kg/liter.
	 */
	double fuelDensity() const { return m_Data->fuelDensity(); }

	/** Get the capacity of the tank, in liters.
	 */
	double capacity() const { return m_Data->capacity(); }

	/** Get the quantity of fuel in the tank, in liters.
	 */
	double quantity() const { return m_Quantity; }

	/** Set the quantity of fuel in the tank, in liters.  The specified value is
	 *  clamped at the rated capacity of the tank.
	 */
	void setQuantity(double quantity) {
		m_Quantity = clampTo(quantity, 0.0, capacity());
	}

	/** Get the static data for this fuel tank.
	 */
	virtual StoreData const *data() const { return m_Data.get(); }

	/** Add dynamic properties of the fuel tank (mass, inertia, drag) to a StoresDynamics instance.
	 */
	virtual void sumDynamics(StoresDynamics &dynamics, Vector3 const &offset, Quat const &attitude, bool no_drag) const {
		m_Data->sumDynamics(dynamics, offset, attitude, quantity() * m_Data->fuelDensity(), no_drag);
	}

private:
	explicit FuelTank(FuelTankData const *data): m_Quantity(data->capacity()), m_Data(data) { assert(data); }
	virtual ~FuelTank() { }
	double m_Quantity;  // l
	Ref<const FuelTankData> m_Data;
};

CSP_NAMESPACE_END

#endif // __CSPSIM_STORES_FUELTANK_H__


