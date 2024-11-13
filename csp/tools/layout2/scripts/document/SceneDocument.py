from .FileDocument import FileDocument


class SceneDocument(FileDocument):
    """This document represents a scene of 3D objects."""

    def __init__(self, *args, **kwargs):
        FileDocument.__init__(self, *args, **kwargs)
        self.node = None

    def SetRootNode(self, node):
        self.node = node
        self.EmitChangedSignal()

    def GetRootNode(self):
        return self.node

    def Clear(self):
        self.node = None
        self.EmitChangedSignal()


class SceneDocumentFactory():
    def __init__(self, fileName, node):
        self.fileName = fileName
        self.node = node

    def GetUniqueId(self):
        """Returns a unique Id identifying the document in the DocumentRegistry."""
        return SceneDocument.MakeUniqueId(self.fileName)

    def CreateDocument(self):
        """Returns a new document that will be added in the DocumentRegistry."""
        document = SceneDocument(self.fileName)
        try:
            document.SetRootNode(self.node)
        except Exception:
            document.Dispose()
            raise

        return document
