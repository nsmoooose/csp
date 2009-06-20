#!/usr/bin/python

# Combat Simulator Project
# Copyright (C) 2002-2009 The Combat Simulator Project
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

"""
Combat Simulator Project : Take off script
"""

import csp.cspsim
from csp.data.ui.tutorials.mission import Mission

class Vehicles(Mission):
    def __init__(self, cspsim):
        Mission.__init__(self, cspsim)

    def name(self):
        return "${tutorials_vehicles}"

    def describingUI(self):
        return "vehicles/mission.xml"
       
    def startMission(self):
        Mission.startMission(self)
        
    def modifyTheatreForMission(self):
        # We add a single aircraft at the airport.
        vec = csp.csplib.Vector3

        m2k = "sim:vehicles.aircraft.m2k"
        self.cspsim.createVehicle(m2k, vec(-29510, -10530, 91.3), vec(0, 0, 0), vec(0.0, 0.0, 180.0), True)

        f16dj = "sim:vehicles.aircraft.f16dj"
        self.cspsim.createVehicle(f16dj, vec(-29495, -10530, 91.3), vec(0, 0, 0), vec(0.0, 0.0, 180.0), True)
       
        # Set the date and time. We want daylight for the first mission.
        date = csp.csplib.SimDate(2007,6,12,10,0,0)
        self.cspsim.setCurrentTime(date)
        
    def onStart(self):
        pass
