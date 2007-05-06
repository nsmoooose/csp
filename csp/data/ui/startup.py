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

class UserInterfaceStartup:
    def __init__(self, cspsim):
        self.cspsim = cspsim
        self.configuration = cspsim.getConfiguration()
        
    def displayDesktop(self):
        # Displays the background of the desktop. 
        import csp.csplib
        import csp.cspsim
        self.desktopWindow = csp.cspsim.Window()
        serializer = csp.cspsim.Serialization()
        serializer.load(self.desktopWindow, self.configuration.getUserInterface().getTheme(), 'desktop.xml')
        self.windowManager.show(self.desktopWindow)
        self.desktopWindow.maximizeWindow()
        
    def displayMainMenu(self):
        # Displays the main menu that lets the user interact with the game.
        from csp.data.ui.mainmenu import MainMenu
        self.mainMenuWindow = MainMenu(self.cspsim, self.configuration.getUserInterface().getTheme())
        self.windowManager.show(self.mainMenuWindow)
        
    def run(self):
        # We want to connect to the configuration changed event.
        from csp.data.ui.utils import SlotProxy
        self.configurationChangedSlot = SlotProxy(self.configuration_Changed)
        self.cspsim.getConfigurationChangedSignal().connect(self.configurationChangedSlot)
        
        # Display a screen that is capable of displaying windows and
        # controls.
        self.cspsim.displayMenuScreen()
        self.menuScreen = self.cspsim.getCurrentScreen()
        self.windowManager = self.menuScreen.getWindowManager()

        # Display the desktop and the main menu.
        self.displayDesktop()
        self.displayMainMenu()
        self.cspsim.run()

    def configuration_Changed(self):
        self.configuration = self.cspsim.getConfiguration()

        # We only change all settings when we are in the menu screen mode.
        # When the simulation is running the class name is GameScreen instead.
        if self.cspsim.getCurrentScreen().__class__.__name__ == 'MenuScreen':
            self.windowManager.closeAll()
            self.displayDesktop()
            self.displayMainMenu()
        elif self.cspsim.getCurrentScreen().__class__.__name__ == 'GameScreen':
            from csp.data.ui.messagebox import MessageBox
            messageBox = MessageBox(self.cspsim, self.configuration.getUserInterface().getTheme())
            messageBox.setMessage('${restart_required}')
            self.cspsim.getCurrentScreen().getWindowManager().show(messageBox)
            messageBox.centerWindow()
