from NodeConstructor import *
RegisterNetwork(globals())


#######################################################################
# roll control laws

class RollLimiterControl(Node): pass

class da_program(RollLimiterControl):
	roll_command_schedule = table('-80.0 -45.0 -25.0 -5.0 5.0 25.0 45.0 80.0', '-308.0 -90.0 -20.0 0.0 0.0 20.0 90.0 308.0', spacing=10.0)

class da(Adder):
	input_a = "da_left"
	input_b = "da_right"
	gain_a = 0.5
	gain_b = -0.5

class da_f(LagFilter):
	input = "da"
	a = 20.2

class da_left(Adder):
	input_a = "da_program"
	input_b = "dtef"
	clamp_hi = 21.5
	clamp_lo = -21.5

class da_right(Adder):
	input_a = "da_program"
	input_b = "dtef"
	gain_a = -1.0
	clamp_hi = 21.5
	clamp_lo = -21.5
