#!/usr/bin/env python
import wx
from csp import csplib
from csp.tools.layout2.scripts.document.OutputDocument import OutputDocument
from Command import Command

class CreateInterfaceInformationDocumentCommand(Command):
	"""Generates a text document describing all interface objects that exists within the
	CSP project. Each property is documented with information about type and what information
	can be assigned to it."""

	def GetCaption(self):
		return "Create interface information document"

	def GetToolTipText(self):
		return "Create interface information document describing all object interfaces."

	def GetToolBarImageName(self):
		return "generic.png"

	def Execute(self):
		application = wx.GetApp()
		documentRegistry = application.GetDocumentRegistry()

		# Start by adding a document if it doesn't exist already.
		documentName = 'Interface information'
		outputDocument = documentRegistry.GetByName(documentName)

		# If the document already exists we just make it current and returns.
		# No need to regenerate it since this is static information generated
		# during compile time.
		if outputDocument is not None:
			documentRegistry.SetCurrentDocument(outputDocument)
			return

		# Create a document where we can add text information about all objects
		# that CSP knows of.
		outputDocument = OutputDocument(documentName)
		documentRegistry.Add(outputDocument)
		documentRegistry.SetCurrentDocument(outputDocument)

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


