import wx

from .Command import Command
from ..controls.DocumentNotebook import DocumentNotebook

class CloseCurrentDocumentCommand(Command):
    """The purpose of this command is to close the currently
    edited document."""

    caption = "Close"
    tooltip = "Close current document"
    toolbarimage = "generic"

    def Execute(self):
        DocumentNotebook.Instance.CloseCurrentPage()

    @staticmethod
    def Enabled():
        return DocumentNotebook.Instance.GetCurrentPage() is not None
