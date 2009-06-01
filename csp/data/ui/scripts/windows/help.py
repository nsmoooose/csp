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
Combat Simulator Project : Help index script
"""

import csp.cspsim
import os.path
from csp.data.ui.scripts.utils import SlotManager

class Help(csp.cspsim.Window, SlotManager):
    def __init__(self):
        csp.cspsim.Window.__init__(self)
        SlotManager.__init__(self)
        
        # Install the move window event handler.
        self.moveEventHandler = csp.cspsim.ControlMoveEventHandler(self)

        self.topics = {'${help_welcome}' : 'index.xml', 
                       '${help_f16_first_steps}' : 'f16_first_steps.xml',
                       '${help_keyboard_aircraft}' : 'aircraft_controls.xml',
                       '${help_keyboard_view}' : 'view_controls.xml',
                       '${help_mouse}' : 'mouse.xml',
                       '${help_joystick}' : 'joystick.xml'}
                
    def displayTopic(self, topic):
        serializer = csp.cspsim.Serialization()
        serializer.load(self, os.path.join('help', topic))
        
        closeButton = self.getById('close')
        if closeButton != None:
            self.connectToClickSignal(closeButton, self.close_Click)
            
        self.topicsListBox = self.getById('topics')
        if self.topicsListBox != None:
            self.topicsListBox.addItem(csp.cspsim.ListBoxItem('${help_welcome}'))
            self.topicsListBox.addItem(csp.cspsim.ListBoxItem('${help_f16_first_steps}'))
            self.topicsListBox.addItem(csp.cspsim.ListBoxItem('${help_keyboard_aircraft}'))
            self.topicsListBox.addItem(csp.cspsim.ListBoxItem('${help_keyboard_view}'))
            self.topicsListBox.addItem(csp.cspsim.ListBoxItem('${help_mouse}'))
            self.topicsListBox.addItem(csp.cspsim.ListBoxItem('${help_joystick}'))

            # Select the correct item in the listbox.
            for key, value in self.topics.iteritems():
                if value == topic:
                    self.topicsListBox.setSelectedItemByText(key)

            self.connectToSelectedItemChangedSignal(self.topicsListBox, self.topics_Changed)
        
    def topics_Changed(self):
        newHelpWindow = Help()
        selectedText = self.topicsListBox.getSelectedItem().getText()
        newHelpWindow.displayTopic(self.topics[selectedText])
        self.getWindowManager().show(newHelpWindow)
        self.close()

    def close_Click(self):
        self.close()
