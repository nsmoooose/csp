#!/usr/bin/env python

from Action import Action

class ModifyXmlDataAction(Action):
	def __init__(self, nodeName, imageName, node, newText):
		Action.__init__(self, node.documentOwner)
		self.nodeName = nodeName
		self.imageName = imageName
		self.node = node
		self.newText = newText
		self.oldText = self.node.GetText()
	
	def GetCaption(self):
		return "Modify xml data"
	
	def GetDescription(self):
		return "Modify xml data of %s" % self.nodeName
	
	def GetImageName(self):
		return self.imageName
	
	def Do(self):
		self.node.SetText(self.newText)
		self.node.documentOwner.EmitChangedSignal()
		return True
	
	def Undo(self):
		self.node.SetText(self.oldText)
		self.node.documentOwner.EmitChangedSignal()
		return True


class ModifyXmlAttributeAction(Action):
	def __init__(self, imageName, node, newValue):
		Action.__init__(self, node.documentOwner)
		self.imageName = imageName
		self.node = node
		self.newValue = newValue
		self.oldValue = self.node.GetValue()
	
	def GetCaption(self):
		return "Modify xml attribute"
	
	def GetDescription(self):
		return "Modify xml attribute '%s'" % self.node.GetName()
	
	def GetImageName(self):
		return self.imageName
	
	def Do(self):
		self.node.SetValue(self.newValue)
		self.node.documentOwner.EmitChangedSignal()
		return True
	
	def Undo(self):
		self.node.SetValue(self.oldValue)
		self.node.documentOwner.EmitChangedSignal()
		return True
