from NodeConstructor import *
RegisterNetwork(globals())


#######################################################################
# yaw control laws

class yaw_command_schedule(Schedule1):
	"""Yaw command (deg/sec) as a function of pedal input force (N)"""
	schedule = table('-500 -80.0 80.0 500', '-32.0 0.0 0.0 32.0', spacing=40.0)
	input = "yaw_control"
	# map [-1, 1] input to [-500, 500]
	gain = 500.0

class limited_yaw_command(Schedule2):
	schedule = table('-180 20.0 30.0 180', '1.0 1.0 0.0 0.0', spacing=10.0)
	input_a = "yaw_command_schedule"
	input_b = "alpha_f"

class yaw_rate_f(Scale):
	input = "yaw_rate"
	gain = RadiansToDegrees

class dr(Adder):
	input_a = "limited_yaw_command"
	input_b = "yaw_rate_f"
	gain_b = 0.75

class dr_f(LagFilter):
	input = "dr"
	a = 20.2

# TODO aoa >= 29 deg
"""
class roll_to_yaw_factor(Multiply):
	input_a = roll_rate  # rad/s
	input_b = alpha_f    # deg

class yaw_roll_target(Adder):
	input_a = "yaw_rate"
	input_b = "roll_to_yaw_factor"
	gain_a = RadiansToDegrees
	gain_b = -1.0

class yaw_roll_target_f(LeadLagFilter):
	input = "yaw_roll_target"
	a = 15.0
	b = 5.0
	c = 3.0

class yaw_roll_target_f2(LeadFilter):
	input = "yaw_roll_target_f"
	gain = 1.5
	a = 1.0

class yaw_target(Adder3):
	input_a = "yaw_roll_target_f2"
	input_b = "lateral_acceleration"
	input_c = "zero" # TODO
	gain_a = 0.5
	gain_b = 0.5 * 19.32 / 9.8  # in g's
"""
