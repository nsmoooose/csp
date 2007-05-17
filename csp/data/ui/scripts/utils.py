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
Combat Simulator Project : Utils script
"""

import csp.cspsim

class SlotProxy(csp.cspsim.Slot):
    def __init__(self, method):
        csp.cspsim.Slot.__init__(self)
        self.method = method
        
    def notify(self, data):
        self.method()

class SlotManager:
    def __init__(self):
        self.slots = []
    
    def connectToClickSignal(self, control, method):
        signal = control.getClickSignal()
        slot = SlotProxy(method)
        signal.connect(slot)
        self.slots.append(slot)
        
    def connectToCheckedChangedSignal(self, control, method):
        signal = control.getCheckedChangedSignal()
        slot = SlotProxy(method)
        signal.connect(slot)
        self.slots.append(slot)
        
    def connectToSelectedItemChangedSignal(self, control, method):
        signal = control.getSelectedItemChangedSignal()
        slot = SlotProxy(method)
        signal.connect(slot)
        self.slots.append(slot)

    def connectToInputInterfaceAction(self, cspsim, action, method):
        gameScreen = cspsim.getCurrentScreen()
        interfaceManager = gameScreen.getInputInterfaceManager()
        signal = interfaceManager.registerActionSignal(action)
        slot = SlotProxy(method)
        signal.connect(slot)
        self.slots.append(slot)

class ListBoxManager:
    def addListToControl(self, listbox, texts):
        for text in texts:
            listBoxItem = csp.cspsim.ListBoxItem(text)
            listbox.addItem(listBoxItem)
            
#    def selectListBoxItem(self, listbox, text):
        
