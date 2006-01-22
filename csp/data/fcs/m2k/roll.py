from NodeConstructor import *
RegisterNetwork(globals())


#######################################################################
# roll control laws

class da_program(Scale):
	input = "roll_control"
	gain = 25.0

class da_f(Adder):
	input_a = "de_left_f"
	input_b = "de_right_f"
	gain_a = -0.5
	gain_b = 0.5
