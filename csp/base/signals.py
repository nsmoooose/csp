
class Signal:
	"""Class that emits an event. Several listeners can connect to the 
	same signal with the Connect method."""
	
	def __init__(self):
		self.slots = []
		
	def Connect(self, slot):
		"""Connects a slot to the list of listeners. The slot is a simple method
		that takes a single parameter (the message)."""
		self.slots.append(slot)
	
	def Emit(self, event):
		"""Emits the signals to all listeners. We are simply iterating the list
		of slots and call each one of them with the message."""
		for slot in self.slots:
			slot(event)
