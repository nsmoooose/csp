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
Combat Simulator Project : GameScreen script that handles UI when a simulation is running.
"""

import csp.cspsim
from csp.data.ui.scripts.utils import SlotManager
from csp.data.ui.scripts.windows.pause import Pause
from csp.data.ui.scripts.windows.quitresume import QuitResume
from csp.data.ui.scripts.windows.topmenu import TopMenu

class GameScreenManager(SlotManager):
    def __init__(self, cspsim):
        SlotManager.__init__(self)

        self.cspsim = cspsim
               
        self.connectToInputInterfaceAction(self.cspsim, 'QUIT', self.on_Quit)
        self.connectToInputInterfaceAction(self.cspsim, 'PAUSE', self.on_Pause)
                                
    def on_Quit(self):
        topMenu = self.cspsim.getCurrentScreen().getWindowManager().getById('topMenuWindow')
        resumeWindow = self.cspsim.getCurrentScreen().getWindowManager().getById('resumeWindow')
        if topMenu != None:
            self.cspsim.getCurrentScreen().getWindowManager().closeAll()
            if self.cspsim.isPaused():
                self.cspsim.togglePause()
        else:
            self.cspsim.getCurrentScreen().getWindowManager().closeAll()
            topMenu = TopMenu(self.cspsim)
            quitResume = QuitResume(self.cspsim)
            self.cspsim.getCurrentScreen().getWindowManager().show(quitResume)
            self.cspsim.getCurrentScreen().getWindowManager().show(topMenu)

    def on_Pause(self):
        if self.cspsim.isPaused():
            self.cspsim.togglePause()
            self.cspsim.getCurrentScreen().getWindowManager().closeAll()
        else:
            self.cspsim.getCurrentScreen().getWindowManager().closeAll()
            pause = Pause(self.cspsim)
            self.cspsim.getCurrentScreen().getWindowManager().show(pause)
        