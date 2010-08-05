#!/usr/bin/env python
import wx
import os.path

from csp.tools.layout2.scripts.document.OutputDocument import OutputDocument
from Command import Command

# TODO

# Display a warning before we start this compilation. The user
# should be forced to choose yes or no if he want's to continue.

# It would be nice if we could catch errors and display them 
# correctly. (the file might be locked, might be read only)

# Compiler warnings and errors should be displayed in an 
# output window.

class ReCompileDataArchiveCommand(Command):
	"""This command will recompile the sim.dar file located in the bin directory
	of this application. This makes it possible to use the latest demo version
	of CSP and the latest release version of layout tool together. In that case
	you won't need any development tools at all on windows. (you will only need
	the version control in order to check out the xml directory only)."""

	def GetCaption(self):
		return "Recompile data archive"

	def GetToolTipText(self):
		return "Recompiles all files in the data archive (sim.dar)"

	def GetToolBarImageName(self):
		return "package-x-generic.png"

	def Execute(self):
		# Retreive tha application object so we can retreive the 
		# current configuration for this application. We need 
		# the path to the xml directory in order to recompile
		# the data archive.
		application = wx.GetApp()
		dataDirectory = application.Configuration.get('LayoutApplication.DataDirectory', '.')
		archiveFile = os.path.join(dataDirectory, '..', 'bin', 'sim.dar')
		xmlDirectory = application.Configuration.get('LayoutApplication.XmlPath', '.')

		# Instantiate the compiler.
		from csp.tools.data.compile import Compiler
		compiler = Compiler()
		compiler.GetCompilerSignal().Connect(self.compiler_Signal)

		# Compile everything from scratch and retreive the number of warning.
		compiler.CompileAll(xmlDirectory, archiveFile)

	def compiler_Signal(self, event):
		application = wx.GetApp()
		documentName = 'Archive compiler'
		outputDocument = application.GetDocumentRegistry().GetByName(documentName)
		if outputDocument is None:
			outputDocument = OutputDocument(documentName)
			documentRegistry = application.GetDocumentRegistry()
			documentRegistry.Add(outputDocument)
			documentRegistry.SetCurrentDocument(outputDocument)
		outputDocument.WriteLine(event.GetMessage())
