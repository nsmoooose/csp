from NodeConstructor import *
RegisterNetwork(globals())


#######################################################################
# speed brake control laws

class dsb(Lesser):
	"""Speed brake deflection angle in degrees."""
	input_a = "speedbrake_control"
	input_b = "speedbrake_limit"
	gain_a = 60.0
