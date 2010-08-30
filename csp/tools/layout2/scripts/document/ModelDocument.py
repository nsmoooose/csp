from FileDocument import FileDocument

class ModelDocument(FileDocument):
    """Document representing a single graphical model. A model can be any .osg or .ive file."""


class ModelDocumentFactory():
    def __init__(self, fileName):
        self.fileName = fileName
    
    def GetUniqueId(self):
        """Returns a unique Id identifying the document in the DocumentRegistry."""
        return ModelDocument.MakeUniqueId(self.fileName)
    
    def CreateDocument(self):
        """Returns a new document that will be added in the DocumentRegistry."""
        return ModelDocument(self.fileName)
