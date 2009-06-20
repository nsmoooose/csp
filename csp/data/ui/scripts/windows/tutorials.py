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
from csp.data.ui.tutorials.mission import Mission
from csp.data.ui.tutorials.takeoff.takeoff import TakeOff
from csp.data.ui.tutorials.timeofday.dawn import TimeOfDayDawn
from csp.data.ui.tutorials.timeofday.day import TimeOfDayDay
from csp.data.ui.tutorials.timeofday.dusk import TimeOfDayDusk
from csp.data.ui.tutorials.timeofday.night import TimeOfDayNight

class Tutorials(csp.cspsim.Window, SlotManager):
    class FakeMission(Mission):
        def name(self):
            return "${tutorials}"
        def describingUI(self):
            return "index.xml"

    def __init__(self, cspsim):
        csp.cspsim.Window.__init__(self)
        SlotManager.__init__(self)
        
        # Install the move window event handler.
        self.moveEventHandler = csp.cspsim.ControlMoveEventHandler(self)
        self.missionDict = {}
        self.cspsim = cspsim
        self.missions = [
            self.FakeMission(self.cspsim),
            TakeOff(self.cspsim),
            TimeOfDayDawn(self.cspsim),
            TimeOfDayDay(self.cspsim),
            TimeOfDayDusk(self.cspsim),
            TimeOfDayNight(self.cspsim)
            ]
        
    def displayMission(self, mission):
        serializer = csp.cspsim.Serialization()
        serializer.load(self, os.path.join('tutorials', mission))
        
        closeButton = self.getById('close')
        if closeButton != None:
            self.connectToClickSignal(closeButton, self.close_Click)

        self.missionListBox = self.getById('missions')
        if self.missionListBox != None:
            for list_mission in self.missions:
                item = csp.cspsim.ListBoxItem(list_mission.name())
                self.missionDict[list_mission.name()] = list_mission
                self.missionListBox.addItem(item)

                if list_mission.describingUI() == mission:
                    self.missionListBox.setSelectedItem(item)

            self.connectToSelectedItemChangedSignal(self.missionListBox, self.missionListBox_Changed)
            
        startButton = self.getById('start')
        if startButton != None:
            self.connectToClickSignal(startButton, self.start_Click)
        
    def missionListBox_Changed(self):
        selectedItem = self.missionListBox.getSelectedItem().getText()

        newTutorialsWindow = Tutorials(self.cspsim)
        newTutorialsWindow.displayMission(self.missionDict[selectedItem].describingUI())
        self.getWindowManager().show(newTutorialsWindow)
        self.close()

    def close_Click(self):
        self.close()
        
    def start_Click(self):
        if self.missionListBox:
            selectedItem = self.missionListBox.getSelectedItem()
            mission = self.missionDict[selectedItem.getText()]
            mission.startMission()
