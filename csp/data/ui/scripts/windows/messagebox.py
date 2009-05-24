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
Combat Simulator Project : Message box script
"""

import csp.cspsim
from csp.data.ui.scripts.utils import SlotManager

class MessageBox(csp.cspsim.Window, SlotManager):
    def __init__(self, cspsim):
        csp.cspsim.Window.__init__(self)
        SlotManager.__init__(self)

        # Install the move window event handler.
        self.moveEventHandler = csp.cspsim.ControlMoveEventHandler(self)

        self.cspsim = cspsim

        # Load the user interface for this window.
        serializer = csp.cspsim.Serialization()
        serializer.load(self, 'messagebox.xml')
                
        # Listen to the click signal for the ok button.
        okButton = self.getById('ok')
        if okButton != None:
            self.connectToClickSignal(okButton, self.ok_Click)
        
    def ok_Click(self):
        self.close()
        
    def setMessage(self, message):
        messageLabel = self.getById('message')
        if messageLabel != None:
            messageLabel.setText(message)
            
    def show(self, parentWindow, message):
        self.setMessage(message)
        parentWindow.getWindowManager().show(self)
