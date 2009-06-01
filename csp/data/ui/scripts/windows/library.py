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

class Library(csp.cspsim.Window, SlotManager):
    def __init__(self):
        csp.cspsim.Window.__init__(self)
        SlotManager.__init__(self)    
        
        # Install the move window event handler.
        self.moveEventHandler = csp.cspsim.ControlMoveEventHandler(self)

        serializer = csp.cspsim.Serialization()
        serializer.load(self, os.path.join('library', 'index.xml'))

        closeButton = self.getById('close')
        if closeButton != None:
            self.connectToClickSignal(closeButton, self.close_Click)
        
    def displayTopic(self, topic):
        print(topic)

    def close_Click(self):
        self.close()
