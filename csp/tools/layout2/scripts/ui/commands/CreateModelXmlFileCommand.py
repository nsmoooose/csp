from Command import Command

class CreateModelXmlFileCommand(Command):
    """Creates a new model xml file describing an open scene graph model.
    For this command to be valid you must select an existing osg or ive file."""

    def GetCaption(self):
        return "Create model XML file"

    def GetToolTipText(self):
        return "Creates a model XML file for a OSG model file (*.osg|*.ive)."

    def GetToolBarImageName(self):
        return "generic"

    def Execute(self):
	
	# Get the name of the currently selected osg model.

	# Create a document which reflects properties from the file.

        pass
