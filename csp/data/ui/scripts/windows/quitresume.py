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
Combat Simulator Project : Quit and resume window script
"""

import csp.cspsim
from csp.data.ui.scripts.utils import SlotManager
from csp.data.ui.scripts.windows.options import Options

class QuitResume(csp.cspsim.Window, SlotManager):
    def __init__(self, cspsim):
        csp.cspsim.Window.__init__(self)
        SlotManager.__init__(self)

        self.cspsim = cspsim

        # Load the user interface for this window.
        serializer = csp.cspsim.Serialization()
        serializer.load(self, 'quit_resume.xml')

        resumeButton = self.getById('resume')
        if resumeButton != None:
            self.connectToClickSignal(resumeButton, self.resume_Click)
       
        endSimButton = self.getById('end_simulation')
        if endSimButton != None:
            self.connectToClickSignal(endSimButton, self.end_simulation_Click)
            
        if self.cspsim.isPaused() == False:
            self.cspsim.togglePause()
        
    def resume_Click(self):
        if self.cspsim.isPaused():
            self.cspsim.togglePause()
        topMenu = self.getWindowManager().getById('topMenuWindow')
        if topMenu != None:
            topMenu.close()
        self.close()
       
    def end_simulation_Click(self):
        # self.cspsim.displayMenuScreen()
        print 'End requested'
