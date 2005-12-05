
from NodeConstructor import *
RegisterNetwork(globals())


#######################################################################
# pitch control laws


class de_right(Adder):
	input_a = "elevator_pitch_deflection_command"
	input_b = "da_program"
	gain_a = -1.0
	gain_b = 1.0
	clamp_lo = -25
	clamp_hi = 16

class de_left(Adder):
	input_a = "elevator_pitch_deflection_command"
	input_b = "da_program"
	gain_a = -1.0
	gain_b = -1.0
	clamp_lo = -25
	clamp_hi = 16

class de_f(Adder):
	input_a = "de_left_f"
	input_b = "de_right_f"
	gain_a = 0.5
	gain_b = 0.5

class de_left_f(LagFilter):
#class de_left_f(Scale):
	input = "de_left"
	a = 20.2

class de_right_f(LagFilter):
#class de_right_f(Scale):
	input = "de_right"
	a = 20.2

class g_command(Schedule1):
	"""NASA-79-tp1538 page 212 (F-16A)"""
	schedule = table(breaks="-180.0 -80.0 -30.0 -10.0 10.0 40.0 180.0", values="-4.0 -4.0 -0.8 0.0 0.0 0.5 10.8", spacing=10.0)
	gain = 180.0
	input = "pitch_control"

class g_trim(Constant):
	value = 0.0

class trimmed_g_command(Adder):
	input_a = "g_command"
	input_b = "g_trim"

class positive_g_limit(Scale):
	clamp_lo = 0.0
	clamp_hi = 8.0
	input = "trimmed_g_command"

class negative_g_limit(Scale):
	clamp_lo = -3.0
	clamp_hi = 0.0
	input = "trimmed_g_command"

class limited_g_command(Adder):
	input_a = "negative_g_limit"
	input_b = "positive_g_limit"

class limited_g_command_f(LagFilter):
	a = 8.3
	input = "limited_g_command"

class pitch_rate_f(LeadFilter):
	input = "pitch_rate"
	a = 1.0

class target(Adder):
	input_a = "g_minus_1"
	input_b = "pitch_rate_f"
	gain_b = 0.334

class target_f(LeadLagFilter):
	input = "target"
	a = 4.0
	b = 12.0
	c = 3.0

class elevator_deviation(Adder):
	input_a = "limited_g_command_f"
	input_b = "target_f"
	gain_a = -1.0

class reduced_elevator_deviation(Schedule2):
	input_a = "elevator_deviation"
	input_b = "qbar"
	gain_a = 1.5
	gain_b = 0.001
	schedule = table('0 10.0 44.0 200.0', '1.0 1.0 0.4 0.2', spacing=4.0)

class elevator_pitch_deflection_command(Adder):
	input_a = "reduced_elevator_deviation"
	input_b = "elevator_deviation_integrator"
	gain_a = 8.0
	clamp_lo = -16
	clamp_hi = 25

class elevator_deviation_integrator(Integrator):
	input = "reduced_elevator_deviation"
	gain = 5.0
	clamp_lo = -16.0
	clamp_hi = 25.0

