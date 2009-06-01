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
Combat Simulator Project : User interface startup script

Usage: To be called from sim.py
"""

import csp.csplib
import csp.cspsim
from csp.data.ui.scripts.utils import SlotProxy
from csp.data.ui.scripts.windows.desktop import Desktop
from csp.data.ui.scripts.windows.mainmenu import MainMenu
from csp.data.ui.scripts.windows.messagebox import MessageBox
from csp.data.ui.scripts.windows.topmenu import TopMenu

class UserInterfaceStartup:
    def __init__(self, cspsim):
        self.cspsim = cspsim
        self.configuration = cspsim.getConfiguration()
        
    def displayDesktop(self):
        # Displays the background of the desktop. 
        self.desktopWindow = Desktop()
        self.windowManager.show(self.desktopWindow)
        self.desktopWindow.maximizeWindow()
        
    def displayTopMenu(self):
        # Displays the top menu with the most important buttons. 
        self.topMenu = TopMenu(self.cspsim)
        self.windowManager.show(self.topMenu)

    def displayMainMenu(self):
        # Displays the main menu that lets the user interact with the game.
        self.mainMenuWindow = MainMenu(self.cspsim)
        self.windowManager.show(self.mainMenuWindow)
        
    def run(self):
        # We want to connect to the configuration changed event.
        self.configurationChangedSlot = SlotProxy(self.configuration_Changed)
        self.cspsim.getConfigurationChangedSignal().connect(self.configurationChangedSlot)
        
        # Display a screen that is capable of displaying windows and
        # controls.
        self.cspsim.displayMenuScreen()
        self.menuScreen = self.cspsim.getCurrentScreen()
        self.windowManager = self.cspsim.getWindowManager()

        # Display the desktop and the main menu.
        self.displayDesktop()
        self.displayTopMenu()
        self.displayMainMenu()

    def configuration_Changed(self):
        self.configuration = self.cspsim.getConfiguration()

        # We only change all settings when we are in the menu screen mode.
        # When the simulation is running the class name is GameScreen instead.

        # Issue a find on the clss name. We cannot use exact match since swig can generate different
        # names depending on version.
        if self.cspsim.getCurrentScreen().__class__.__name__.find('MenuScreen') != -1:
            print('Reloading all windows...')
            self.windowManager.closeAll()
            self.displayDesktop()
            self.displayTopMenu()
            self.displayMainMenu()
        elif self.cspsim.getCurrentScreen().__class__.__name__.find('GameScreen') != -1:
            messageBox = MessageBox(self.cspsim)
            messageBox.setMessage('${restart_required}')
            self.cspsim.getWindowManager().show(messageBox)
