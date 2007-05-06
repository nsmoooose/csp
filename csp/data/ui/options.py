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
Combat Simulator Project : Options menu script
"""

import csp.cspsim
from csp.data.ui.utils import SlotManager
from csp.data.ui.utils import ListBoxManager

class Options(csp.cspsim.Window, SlotManager):
    def __init__(self, cspsim, themeName):
        csp.cspsim.Window.__init__(self)
        SlotManager.__init__(self)

        self.cspsim = cspsim

        # Load the user interface for this window.
        serializer = csp.cspsim.Serialization()
        serializer.load(self, themeName, 'options.xml')
        
        # Make a copy of the current configuration. This copy
        # is used while we have the window open.
        self.configuration = self.cspsim.getConfiguration().clone()
        
        # Listen to the click signal for the ok button.
        okButton = self.getById('ok')
        if okButton != None:
            self.connectToClickSignal(okButton, self.ok_Click)
        
        # Listen to the click signal for the cancel button.
        cancelButton = self.getById('cancel')
        if cancelButton != None:
            self.connectToClickSignal(cancelButton, self.cancel_Click)

        # Class with usefull functions to handle listboxes.
        listBoxManager = ListBoxManager()
                        
        # List all availible themes in the corresponding listbox and select the current one.
        self.themeListBox = self.getById('theme')
        if self.themeListBox != None:
            themes = self.configuration.getUserInterface().enumerateThemes()
            listBoxManager.addListToControl(self.themeListBox, themes)
            self.themeListBox.setSelectedItemByText(self.configuration.getUserInterface().getTheme())
            self.connectToSelectedItemChangedSignal(self.themeListBox, self.theme_Changed)
        
        # List all localizations made and select the current.
        self.languageListBox = self.getById('language')
        if self.languageListBox != None:
            languages = self.configuration.getUserInterface().enumerateLanguages()
            listBoxManager.addListToControl(self.languageListBox, languages)
            self.languageListBox.setSelectedItemByText(self.configuration.getUserInterface().getLanguage())
            self.connectToSelectedItemChangedSignal(self.languageListBox, self.language_Changed)
        
        # List all screen resolutions and set the current value.
        self.resolutionListBox = self.getById('resolution')
        if self.resolutionListBox != None:
            currentResolution = str(self.configuration.getDisplay().getWidth()) + 'x' + str(self.configuration.getDisplay().getHeight())
            displayModes = self.configuration.getDisplay().enumerateDisplayModes()
            listBoxManager = ListBoxManager()
            listBoxManager.addListToControl(self.resolutionListBox, displayModes)
            self.resolutionListBox.setSelectedItemByText(currentResolution)
            self.connectToSelectedItemChangedSignal(self.resolutionListBox, self.resolution_Changed)
        
        # Set the fullscreen status checkbox.
        self.fullscreen = self.getById('fullscreen')
        if self.fullscreen != None:
            self.fullscreen.setChecked(self.configuration.getDisplay().getFullscreen())
            self.connectToCheckedChangedSignal(self.fullscreen, self.fullscreen_Changed)
        
    def resolution_Changed(self):
        newResolution = self.resolutionListBox.getSelectedItem().getText().split('x')
        width = int(newResolution[0])
        height = int(newResolution[1])
        self.configuration.getDisplay().setWidth(width)
        self.configuration.getDisplay().setHeight(height)
        
        from csp.data.ui.messagebox import MessageBox
        messageBox = MessageBox(self.cspsim, self.getTheme())
        messageBox.show(self, '${restart_required}')
        
        
    def theme_Changed(self):
        self.configuration.getUserInterface().setTheme(self.themeListBox.getSelectedItem().getText())
        
    def language_Changed(self):
        self.configuration.getUserInterface().setLanguage(self.languageListBox.getSelectedItem().getText())
        
    def fullscreen_Changed(self):
        self.configuration.getDisplay().setFullscreen(self.fullscreen.getChecked())

    def ok_Click(self):
        # Verify all settings and then apply the new environment.
        self.cspsim.setConfiguration(self.configuration)
        self.close()
        
    def cancel_Click(self):
        # Discard all our changes and close this window. Since we have worked with a copy
        # of the current configuration it is safe just to close the window.
        self.close()
