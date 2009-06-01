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
Combat Simulator Project : Tutorials index script
"""

import csp.cspsim
import os.path
from csp.data.ui.scripts.utils import SlotManager
from csp.data.ui.tutorials.takeoff.takeoff import TakeOff

class Tutorials(csp.cspsim.Window, SlotManager):
    def __init__(self, cspsim):
        csp.cspsim.Window.__init__(self)
        SlotManager.__init__(self)
        
        # Install the move window event handler.
        self.moveEventHandler = csp.cspsim.ControlMoveEventHandler(self)

        self.missions = {'${tutorials}' : 'index.xml', 
                       '${tutorials_takeoff}' : 'takeoff/mission.xml'}
        self.cspsim = cspsim
                
    def displayMission(self, mission):
        serializer = csp.cspsim.Serialization()
        serializer.load(self, os.path.join('tutorials', mission))
        
        closeButton = self.getById('close')
        if closeButton != None:
            self.connectToClickSignal(closeButton, self.close_Click)
            
        self.missionListBox = self.getById('missions')
        if self.missionListBox != None:
            self.missionListBox.addItem(csp.cspsim.ListBoxItem('${tutorials}'))
            self.missionListBox.addItem(csp.cspsim.ListBoxItem('${tutorials_takeoff}'))

            # Select the correct item in the listbox.
            for key, value in self.missions.iteritems():
                if value == mission:
                    self.missionListBox.setSelectedItemByText(key)

            self.connectToSelectedItemChangedSignal(self.missionListBox, self.missionListBox_Changed)
            
        startButton = self.getById('start')
        if startButton != None:
            self.connectToClickSignal(startButton, self.start_Click)
        
    def missionListBox_Changed(self):
        newTutorialsWindow = Tutorials(self.cspsim)
        selectedText = self.missionListBox.getSelectedItem().getText()
        newTutorialsWindow.displayMission(self.missions[selectedText])
        self.getWindowManager().show(newTutorialsWindow)
        self.close()

    def close_Click(self):
        self.close()
        
    def start_Click(self):
        mission = TakeOff(self.cspsim)
        mission.startMission()
