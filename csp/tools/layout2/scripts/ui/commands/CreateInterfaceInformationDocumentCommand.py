import wx
from csp import csplib
from Command import Command
from ...document.OutputDocument import OutputDocumentFactory
from ..controls.OutputWindow import OutputWindow
from ..controls.DocumentNotebook import DocumentNotebook

class CreateInterfaceInformationDocumentCommand(Command):
	"""Generates a text document describing all interface objects that exists within the
	CSP project. Each property is documented with information about type and what information
	can be assigned to it."""

	def GetCaption(self):
		return "Create interface information document"

	def GetToolTipText(self):
		return "Create interface information document describing all object interfaces."

	def GetToolBarImageName(self):
		return "generic"

	def Execute(self):
		# Get a document from the DocumentRegistry, where we can add
		# text information about all objects that CSP knows of.
		documentName = 'Interface information'
		application = wx.GetApp()
		documentRegistry = application.GetDocumentRegistry()
		outputDocument = documentRegistry.GetOrCreateDocument( OutputDocumentFactory(documentName) )

		# If the document view already exists we just make it current and returns.
		# No need to open a new view since this is static information generated
		# during compile time.
		pages = DocumentNotebook.Instance.GetAllDocumentPages(outputDocument)
		if len(pages) > 0:
			# We don't create a new view, so we don't need to keep the
			# reference created by the call to GetOrCreateDocument.
			documentRegistry.ReleaseDocument(outputDocument)
			DocumentNotebook.Instance.SetCurrentPage( pages[0] )
			return

		# Create an OutputWindow for the document and add it to the DocumentNotebook
		DocumentNotebook.Instance.AddDocumentPage(OutputWindow, outputDocument)

		# Write general information about the document we are creating.
		outputDocument.WriteLine(
			"This is a list of all object interfaces that CSP is exposing. All these\n" +
			"objects are implemented in C++ and can be serialized from a binary archive.\n" +
			"A xml representation of the binary archive is located within the csp/data/xml directory.")
		outputDocument.WriteLine("")

		# Get the interface registry that is responsible for knowing everything
		# we want.
		interfaceRegistry = csplib.InterfaceRegistry.getInterfaceRegistry()

		# Get all interface names and sort them.
		names = list(interfaceRegistry.getInterfaceNames())
		names.sort()

		# For each interface we print information about it. Variable names
		# and those types of data.
		for name in names:
			# Get the proxy class that describes the object.
			interfaceProxy = interfaceRegistry.getInterface(name)

			# Write class name and information about it.
			classModifier = ""
			if interfaceProxy.isStatic():
				classModifier = "static "
			outputDocument.WriteLine("%sclass %s {" % (classModifier, name))

			# Iterate all members of the class.
			variableNames = interfaceProxy.getVariableNames()
			for variableName in variableNames:
				# Information if the variable is required or not.
				required = ""
				if interfaceProxy.variableRequired(variableName):
					required = " (required)"

				variableType = interfaceProxy.variableType(variableName)

				outputDocument.WriteLine("     %s %s%s;" % (variableType, variableName, required))

			# Write end of class.
			outputDocument.WriteLine("};")
			outputDocument.WriteLine("")
