#!/usr/bin/env python
		
# CSPEdit - Copyright (C) 2003 Mark Rose
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
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, US

import sys, threading
from wxPython.wx import *
from wxPython.help import *

class MainWindow(wxFrame):
	def __init__(self, parent, ID, title):
		wxFrame.__init__(self, parent, ID, title,
						 wxDefaultPosition, wxSize(200, 150))
		self.CreateStatusBar()
		self.SetStatusText("This is the statusbar")
		self.MakeMenu()
		self.help = help = wxHelpController()
		self.Layout()

	def MakeMenu(self):
		ID_ABOUT = 101
		ID_EXIT  = 102

		menu = wxMenu()
		menu.Append(ID_ABOUT, "&About",
					"More information about this program")
		menu.AppendSeparator()
		menu.Append(ID_EXIT, "E&xit", "Terminate the program")

		menuBar = wxMenuBar()
		menuBar.Append(menu, "&File");

		EVT_MENU(self, ID_EXIT, self.OnExit)

		self.SetMenuBar(menuBar)

	def OnExit(self, event):
		try:
			#self.ToolBox.Destroy()
			pass
		except:
			pass
		self.Destroy()
		event.Skip()


class Init(wxApp):
	def __init__(self):
		wxApp.__init__(self, 0)
	def OnInit(self):
		frame = MainWindow(NULL, -1, "CSPEdit")
		frame.Show(true)
		self.SetTopWindow(frame)
		return true

ui = Init()
ui.MainLoop()


