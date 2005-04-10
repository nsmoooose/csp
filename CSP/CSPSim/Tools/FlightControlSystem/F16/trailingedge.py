
from NodeConstructor import *
RegisterNetwork(globals())


#######################################################################
# trailing edge flap control laws

class TrailingEdgeFlapControl(Node): pass

class dtef(TrailingEdgeFlapControl):
	"""Trailing edge flap deflection in degrees (positive = extended)."""
	deflection_limit = 20.0  # degrees
	airspeed_break1 = 240.0  # kts
	airspeed_break2 = 370.0  # kts

