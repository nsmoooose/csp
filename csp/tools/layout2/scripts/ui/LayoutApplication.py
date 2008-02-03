#!/usr/bin/env python
import dumbdbm
import os.path
import wx
import shelve
from csp.tools.layout2.layout_module import *

from MainFrame import MainFrame
from SelectDataDirectoryDialog import SelectDataDirectoryDialog

class LayoutApplication(wx.App):
	# wxWindows calls this method to initialize the application
	def OnInit(self):
		# First of all we do a sanity check that we can find 
		# the readme.txt file. When the application is started
		# from the win32 installer we have all binaries in the
		# bin directory. The images folder is then one step up
		# in the hiearchy.
		readmeFile = os.path.join(os.getcwd(), 'start.txt')
		if not os.path.isfile(readmeFile):
			os.chdir('..')
		
		readmeFile = os.path.join(os.getcwd(), 'start.txt')
		if not os.path.isfile(readmeFile):
			return False
	
		self.Configuration = shelve.open('.csplayout')

		dlg = SelectDataDirectoryDialog(None, wx.ID_ANY, "CSP Theater Layout Tool")
		if dlg.ShowModal() == wx.ID_OK:

			dataDirectory = self.Configuration.get('LayoutApplication.DataDirectory', '.')

			datadirs = ('images', 'models', 'fonts', 'sounds')
			pathlist = os.pathsep.join([os.path.join(dataDirectory, subdir) for subdir in datadirs])
			CspLayoutApplication.setOpenSceneGraphPathList(pathlist.encode('utf8'))
			shaderPath = os.path.join(dataDirectory, 'shaders').encode('utf8')
			CspLayoutApplication.setShaderPath(shaderPath)

			# Create an instance of our customized Frame class
			frame = MainFrame(None, wx.ID_ANY, "CSP Theater Layout Tool")
			frame.SetSize(wx.Size(800, 600))
			frame.Show(True)

			# Tell wxWindows that this is our main window
			self.SetTopWindow(frame)

			# Return a success flag
			return True
		else:
			return False
