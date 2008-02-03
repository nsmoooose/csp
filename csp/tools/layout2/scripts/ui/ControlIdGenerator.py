#!/usr/bin/env python

class ControlIdGenerator:
	"""This class purpose is to generate identities for user interface
	controls. The id generated is an integer. These id's are usefull
	together with the wx framework."""
	
	def __init__(self, identity_start = 20000):
		"""Constructs a new generator. If no identity_start is specified
		it will default to 20000."""
		self.identity = identity_start

	def Generate(self):
		"""Generates a new id and returns it."""
		newid = self.identity
		self.identity = self.identity + 1
		return newid
