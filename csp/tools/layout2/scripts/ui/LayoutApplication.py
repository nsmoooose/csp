#!/usr/bin/env python
import dumbdbm
import os.path
import locale
import wx
import shelve
import sys

from csp.base.signals import Signal
from csp.tools.layout2.layout_module import *
from csp.tools.layout2.scripts.document.DocumentRegistry import DocumentRegistry
from MainFrame import MainFrame
from SelectDataDirectoryDialog import SelectDataDirectoryDialog

class LayoutApplication(wx.App):
	"""This is a singleton instance of the entire application. It contains information
	about open documents and all other global things."""

	def GetIdleSignal(self):
		"""Returns the idle signal that can be used when you want to do things
		in the background. It can be usefull to render the scene when used together
		with 3D content."""
		return self.idleSignal

	def OnInit(self):
		"""wxWindows calls this method to initialize the application."""

		if sys.platform == "linux2":
			# Change the default language of the application to english.
			loc = wx.Locale(wx.LANGUAGE_ENGLISH)
			locale.setlocale(locale.LC_ALL, 'en_US')
		elif sys.platform == "win32":
			# Locale names is different in windows. What to use?
			pass

		# Create the idle signal that you can connect to for
		# idle processing.
		self.idleSignal = Signal()
	
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

		# Add the default output document that many command
		# objects are using.
		self.documentRegistry = DocumentRegistry()

		dlg = SelectDataDirectoryDialog(None, wx.ID_ANY, "CSP Theater Layout Tool")
		if dlg.ShowModal() == wx.ID_OK:

			dataDirectory = self.Configuration.get('LayoutApplication.DataDirectory', '.')

			datadirs = ('images', 'models', 'fonts', 'sounds')
			pathlist = os.pathsep.join([os.path.join(dataDirectory, subdir) for subdir in datadirs])
			CspLayoutApplication.setOpenSceneGraphPathList(pathlist.encode('utf8'))
			shaderPath = os.path.join(dataDirectory, 'shaders').encode('utf8')
			CspLayoutApplication.setShaderPath(shaderPath)

			# Destroy the modal dialog
			dlg.Destroy()

			# Create an instance of our customized Frame class
			frame = MainFrame(None, wx.ID_ANY, "CSP Theater Layout Tool")
			frame.SetSize(wx.Size(800, 600))
			frame.Show(True)
		
			# Return a success flag
			return True
		else:
			return False

	def GetDocumentRegistry(self):
		"""Returns the document registry. This class has the 
		responsibility to keep track of all opened documents."""
		return self.documentRegistry