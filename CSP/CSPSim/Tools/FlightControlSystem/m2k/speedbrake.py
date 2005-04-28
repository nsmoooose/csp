from NodeConstructor import *
RegisterNetwork(globals())


#######################################################################
# speed brake control laws

class dsb(Lesser):
	"""Speed brake deflection angle in degrees."""
	input_a = "speedbrake_control"
	input_b = "one"
	gain_a = 60.0
	gain_b = 60.0
