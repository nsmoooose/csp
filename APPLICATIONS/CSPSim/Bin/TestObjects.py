# Combat Simulator Project - CSPSim
# Copyright (C) 2002 The Combat Simulator Project
# http://csp.sourceforge.net
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

import SimData

def create(app):
	v = SimData.Vector3
	m = SimData.LLA()
	vehicle = app.createVehicle
	vehicle("sim:vehicles.aircraft.f16", v(10, 10, 1000.2), v(0, 100.0, 0), v(2.0, 2.0, 140.0))

# 	vehicle("sim:vehicles.aircraft.m2k", v(483010, 499010, 190.2), v(0, 100.0, 0), v(2.0, 2.0, 140.0))
# 	vehicle("sim:vehicles.aircraft.m2k", v(483025, 499015, 190.2), v(0, 100.0, 0), v(5.0, 0.0, 240.0))
# 	vehicle("sim:vehicles.aircraft.m2k", v(483040, 499020, 190.2), v(0, 100.0, 0), v(8.0, 1.0, 240.0))
# 	vehicle("sim:vehicles.aircraft.m2k", v(483055, 499025, 190.2), v(0, 100.0, 0), v(12.0, 3.0, 240.0))


#	m.setDegrees(37.5, -121.0, 600)
#	vehicle("sim:vehicles.aircraft.m2k", m, v(0, 100.0, 0), v(2.0, 2.0, 2.0))

# 	m.setDegrees(37.6, -121.41, 2090)
# 	vehicle("sim:vehicles.aircraft.m2k", m, v(0, 100.0, 0), v(2.0, 2.0, 140.0))
# 	m.setDegrees(40.6, 14.42, 2090)
# 	vehicle("sim:vehicles.aircraft.m2k", m, v(0, 100.0, 0), v(2.0, 2.0, 140.0))
# 	m.setDegrees(42.6, 14.43, 2090)
# 	vehicle("sim:vehicles.aircraft.m2k", m, v(0, 100.0, 0), v(2.0, 2.0, 140.0))
# 
