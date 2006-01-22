from NodeConstructor import *
RegisterNetwork(globals())


#######################################################################
# leading edge flap control laws

class LeadingEdgeFlapControl(Node): pass

class dlef(LeadingEdgeFlapControl):
	"""Leading edge flap deflection in degrees (positive = extended)."""
	dlef_alpha = 79.068
	dlef_qbar_ps = -9.05
	dlef_offset = 1.45
	dlef_ground_deflection = -2.0

