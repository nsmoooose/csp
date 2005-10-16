from NodeConstructor import *
RegisterNetwork(globals())


#######################################################################
# yaw control laws

class YawLimiterControl(Node): pass

class yaw_limiter(YawLimiterControl):
	pass

class yaw_rate_f(LagFilter):
	input = "yaw_rate"
	a = 50.0
	gain = RadiansToDegrees

class dr(Schedule2):
	schedule = table('20000.0 40000.0 80000.0', '1.0 0.5 0.05', spacing=20000.0)
	input_a = "dr_raw"
	input_b = "qbar"

class yaw_command_schedule(Schedule1):
	"""Yaw command (deg/sec) as a function of pedal input force (N)"""
	# 480 instead of 489 so allow coarser spacing of the table values.
	schedule = table('-480 -80.0 80.0 480', '30.0 0.0 0.0 -30.0', spacing=40.0)
	input = "yaw_control"
	# map [-1, 1] input to [-500, 500]
	gain = 500.0

class limited_yaw_command_0(Schedule2):
	# dash-one values 14 - 26 degrees (NASA 79 paper uses 20 - 30 degrees)
	schedule = table('-180 14.0 26.0 180', '1.0 1.0 0.0 0.0', spacing=10.0)
	input_a = "yaw_command_schedule"
	input_b = "alpha_f"

class limited_yaw_command_1(Schedule2):
	# yaw control modification, fig. 35 on p. 145 of nasa 1979.
	schedule = table('-40.0 -20.0 20.0 40.0', '0.0 1.0 1.0 0.0', spacing=20.0)
	input_a = "limited_yaw_command_0"
	input_b = "roll_rate"
	gain_b = RadiansToDegrees

class dr_raw(Adder):
	input_a = "limited_yaw_command_1"
	input_b = "yaw_limiter"

class dr_f(LagFilter):
	input = "dr"
	a = 20.2

