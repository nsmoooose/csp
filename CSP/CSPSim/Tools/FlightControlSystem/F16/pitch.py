
from NodeConstructor import *
RegisterNetwork(globals())


#######################################################################
# pitch control laws

class filtered_pitch_rate(LeadFilter):
	a = 1.0
	input = "pitch_rate"
	gain = RadiansToDegrees

class de_right(Adder):
	input_a = "elevator_pitch_deflection_command"
	input_b = "da_program"
	gain_b = -0.25

class de_left(Adder):
	input_a = "elevator_pitch_deflection_command"
	input_b = "da_program"
	gain_b = 0.25

class de(Scale):
	input = "elevator_pitch_deflection_command"

class de_left_f(LagFilter):
	input = "de_left"
	gain = -DegreesToRadians
	a = 20.2

class de_right_f(LagFilter):
	input = "de_right"
	gain = -DegreesToRadians
	a = 20.2

class de_f(LagFilter):
	input = "de"
	gain = -DegreesToRadians
	a = 20.2

class g_command(Schedule1):
	"""NASA-79-tp1538 page 212"""
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

class negative_g_limit(Greater):
	input_a = "trimmed_g_command"
	input_b = "negative_g_schedule"
	clamp_hi = 0.0

class negative_g_schedule(Scale):
	clamp_lo = -4.0
	clamp_hi = -1.0
	input = "qbar"
	gain = -0.000444

class limited_g_command(Adder):
	input_a = "negative_g_limit"
	input_b = "positive_g_limit"

class limited_g_command_f(LagFilter):
	a = 8.3
	input = "limited_g_command"

class elevator_deviation(Adder):
	input_a = "limited_g_command_f"
	input_b = "pitch_limiter"
	gain_a = -1.0

class reduced_elevator_deviation(Schedule2):
	input_a = "elevator_deviation"
	input_b = "qbar"
	gain_a = 1.5
	gain_b = 0.001
	schedule = table('0 12.0 44.0 200.0', '1.0 1.0 0.4 0.4', spacing=4.0)

class elevator_pitch_deflection_command(Adder3):
	input_a = "reduced_elevator_deviation"
	input_b = "elevator_deviation_integrator"
	input_c = "alpha_f"
	gain_c = RadiansToDegrees * 0.5

class compensated_elevator_deviation(Adder):
	input_a = "reduced_elevator_deviation"
	input_b = "elevator_compensation"
	gain_b = -5.0

class elevator_compensation(Schedule1):
	input = "elevator_pitch_deflection_command"
	schedule = table('-125.0 -25.0 25.0 125.0', '-100.0 0.0 0.0 100.0', spacing=25.0)

class elevator_deviation_integrator(Integrator):
	input = "compensated_elevator_deviation"
	gain = 5.0
	clamp_lo = -25.0
	clamp_hi = 25.0


# G/AoA limiter ###################################################################

class PitchLimiterControl(Node): pass

class pitch_limiter(PitchLimiterControl):
	"""F16 G/AoA limiter circuit."""
	filtered_alpha = "alpha_f"
	alpha_break1 = 15.0
	alpha_break2 = 20.4
	pitch_rate_schedule = table('0.0 5000.0 15000.0 200000.0', '1.0 1.0 0.35 0.35', spacing=5000.0)

