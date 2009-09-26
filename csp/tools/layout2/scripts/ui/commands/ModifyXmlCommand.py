#!/usr/bin/env python

from ReversibleCommand import ReversibleCommand

class ModifyXmlDataCommand(ReversibleCommand):
	def __init__(self, node, newText):
		self.node = node
		self.newText = newText
		self.oldText = self.node.GetText()
	
	def Execute(self):
		self.node.SetText(self.newText)
		self.node.documentOwner.EmitChangedSignal()
	
	def Undo(self):
		self.node.SetText(self.oldText)
		self.node.documentOwner.EmitChangedSignal()


class ModifyXmlAttributeCommand(ReversibleCommand):
	def __init__(self, node, newValue):
		self.node = node
		self.newValue = newValue
		self.oldValue = self.node.GetValue()
	
	def Execute(self):
		self.node.SetValue(self.newValue)
		self.node.documentOwner.EmitChangedSignal()
	
	def Undo(self):
		self.node.SetValue(self.oldValue)
		self.node.documentOwner.EmitChangedSignal()
