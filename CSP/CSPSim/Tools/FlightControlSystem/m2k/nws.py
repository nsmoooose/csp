
from NodeConstructor import *
RegisterNetwork(globals())


class left_brake_channel(Adder):
	input_a = "left_brake_input"
	input_b = "yaw_control"
	gain_b = -1.0
	clamp_lo = 0.0
	clamp_hi = 1.0

class right_brake_channel(Adder):
	input_a = "right_brake_input"
	input_b = "yaw_control"
	gain_b = 1.0
	clamp_lo = 0.0
	clamp_hi = 1.0

class nose_wheel_steering(BooleanSwitch):
	input_a = "yaw_control"
	input_b = "zero"
	channel = "LandingGear.FrontGear.WOW"
