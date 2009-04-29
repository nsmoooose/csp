#!/usr/bin/python

# Combat Simulator Project
# Copyright (C) 2002-2005 The Combat Simulator Project
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
Combat Simulator Project : Main menu script
"""

import csp.csplib
from csp.data.ui.scripts.gamescreenmanager import GameScreenManager
from csp.data.ui.scripts.utils import SlotManager
from csp.data.ui.scripts.windows.options import Options
from csp.data.ui.scripts.windows.tutorials import Tutorials

class MainMenu(csp.cspsim.Window, SlotManager):
    def __init__(self, cspsim):
        csp.cspsim.Window.__init__(self)
        SlotManager.__init__(self)

        self.cspsim = cspsim
        
        serializer = csp.cspsim.Serialization()
        serializer.load(self, 'main_menu.xml')
        
        instantActionButton = self.getById('instantAction')
        if instantActionButton != None:
            self.connectToClickSignal(instantActionButton, self.instantAction_Click)
        
        optionsButton = self.getById('options')
        if optionsButton != None:
            self.connectToClickSignal(optionsButton, self.options_Click)
            
        tutorialsButton = self.getById('tutorials')
        if tutorialsButton != None:
            self.connectToClickSignal(tutorialsButton, self.tutorials_Click)
        
        quitButton = self.getById('quit')
        if quitButton != None:
            self.connectToClickSignal(quitButton, self.quit_Click)

    def instantAction_Click(self):
        self.getWindowManager().closeAll()
        self.cspsim.displayLogoScreen()
        self.cspsim.loadSimulation()
        
        vec = csp.csplib.Vector3
        m2k = "sim:vehicles.aircraft.m2k"
        f16dj = "sim:vehicles.aircraft.f16dj"
        self.cspsim.createVehicle(f16dj, vec(-29495, -10530, 91.3), vec(0, 0, 0), vec(0.0, 0.0, 180.0), True) 
        self.cspsim.createVehicle(f16dj, vec(-29510, -10530, 91.3), vec(0, 0, 0), vec(0.0, 0.0, 180.0), False) 
        
        gameScreenManager = GameScreenManager(self.cspsim)
        
    def tutorials_Click(self):
        tutorials = Tutorials(self.cspsim)
        tutorials.displayMission('index.xml')
        self.getWindowManager().show(tutorials)
                        
    def options_Click(self):
        options = Options(self.cspsim)
        self.getWindowManager().show(options)
        
    def quit_Click(self):
        self.cspsim.quit()
        
