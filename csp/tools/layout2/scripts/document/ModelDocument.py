#!/usr/bin/env python

from Document import Document

class ModelDocument(Document):
    """Document representing a single graphical model. A model can be any .osg or .ive file."""

    def SetFileName(self, fileName):
        self._fileName = fileName

    def GetFileName(self):
        return self._fileName
