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

class dr(Adder3):
	input_a = "limited_yaw_command"
	input_b = "yaw_rate_f"
	input_c = "lateral_acceleration_compensation"
	gain_b = 0.75
	gain_c = -0.5

class dr_f(LagFilter):
	input = "dr"
	a = 20.2

class lateral_acceleration_compensation(Scale):
	input = "lateral_acceleration"
	gain = 19.32 / 9.8  # in g's
