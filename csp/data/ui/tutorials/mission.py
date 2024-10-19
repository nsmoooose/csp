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
Combat Simulator Project : Tutorial mission base interface
"""

import csp.cspsim
from csp.data.ui.scripts.gamescreenmanager import GameScreenManager
from csp.data.ui.scripts.utils import SlotManager

class Mission:
    """Base class for all tutorial missions."""

    def __init__(self, cspsim):
        self.cspsim = cspsim

    def name(self):
        """Provides a name to the mission. Usually a name that comes from
        a localized xml resource. Example: ${tutorials_takeoff}"""

        return "No name set"

    def describingUI(self):
        """Name of an xml file that contains the graphical user interface
        for this mission. Must be implemented by all tutorials."""

        raise NotImplementedError("Implement this interface to be able " + \
            "to show a describing user interface for the mission.")

    def theatre(self):
        # Informs about what theatre to load.
        return "No theatre set"

    def startMission(self):
        windowManager = self.cspsim.getWindowManager()
        windowManager.closeAll()
        self.cspsim.displayLogoScreen()
        self.cspsim.loadSimulation()

        self.modifyTheatreForMission()
        gameScreenManager = GameScreenManager(self.cspsim)

        self.onStart()

    def modifyTheatreForMission(self):
        pass

    def onStart(self):
        pass
