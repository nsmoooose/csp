
class DocumentRegistry:
	"""This class represents all opened documents. It can be
	any loaded document like a text file or an hierchy of 
	xml files for the layout editor."""
	
	def __init__(self):
		self.documents = []

	def Add(self, document):
		self.documents.append(document)

	def GetByName(self, name):
		for document in self.documents:
			if document.GetName() == name:
				return document
		return None

