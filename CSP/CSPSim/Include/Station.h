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
 * @file Station.h
 *
 **/

#ifndef __STATION_H__
#define __STATION_H__

#include <ObjectModel.h>

#include <SimData/Link.h>
#include <SimData/InterfaceRegistry.h>
#include <SimData/Object.h>

class Bus;
class SceneModel;
class ObjectModel;
class SceneModelChild;


/** A static object representing a player position in a vehicle.  For example,
 *  a tank might have stations for the driver, gunner, and commander.  Each
 *  station defines an optional high-detail cockpit that is used when a human
 *  is controlling the vehicle from that station.  Stations also define a
 *  viewing model that controls view (e.g. by simulating eye, head, and torso
 *  movements) from that position in the vehicle.
 */
class Station: public simdata::Object {
	//simdata::Link<ViewModel> m_ViewModel;  // TODO
	simdata::Link<ObjectModel> m_DetailModel;
	std::string m_Name;
	std::vector<std::string> m_MaskNames;
	mutable unsigned m_Mask;

public:
	SIMDATA_DECLARE_STATIC_OBJECT(Station)

	Station();
	virtual ~Station();

	/** Get the name of this station.
	 */
	std::string const &getName() const { return m_Name; }

	/** Get the names of default cockpit components that should be masked when
	 *  the high-detail cockpit is displayed.
	 */
	std::vector<std::string> const &getMaskNames() const { return m_MaskNames; }

	/** Get the mask used to hide default cockpit components when the station is
	 *  active.  The low bit corresponds to the first child of the __PITS__ node.
	 *  Bits are set if the corresponding pit should be hidden when the detailed
	 *  station model is shown.
	 */
	unsigned getMask() const { return m_Mask; }

	/** Set the mask for this station.  This should only be called by ObjectModel
	 *  when the vehicle model is first loaded.
	 */
	void setMask(unsigned mask) const { m_Mask = mask; }

	/** Construct a new SceneModelChild instance from this station's high-detail
	 *  cockpit model.  Returns null if no cockpit model is defined.
	 */
	SceneModelChild *createDetailModel() const;
};


#endif // __STATION_H__

