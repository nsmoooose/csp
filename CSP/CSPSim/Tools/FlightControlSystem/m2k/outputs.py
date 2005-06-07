from NodeConstructor import *
RegisterNetwork(globals())


#######################################################################
# output channels

class steering_command(OutputChannel):
	channel = "LandingGear.FrontGear.SteeringCommand"
	clamp_lo = -1.0
	clamp_hi = 1.0
	input = "nose_wheel_steering"
	rate_limit_dec = -4.0
	rate_limit_inc = 4.0

class left_brake_command(OutputChannel):
	channel = "LandingGear.LeftGear.BrakeCommand"
	clamp_lo = 0.0
	clamp_hi = 1.0
	input = "left_brake_channel"
	rate_limit_dec = -4.0
	rate_limit_inc = 4.0

class right_brake_command(OutputChannel):
	channel = "LandingGear.RightGear.BrakeCommand"
	clamp_lo = 0.0
	clamp_hi = 1.0
	input = "right_brake_channel"
	rate_limit_dec = -4.0
	rate_limit_inc = 4.0

class rudder_deflection(OutputChannel):
	channel = "ControlSurfaces.RudderDeflection"
	input = "yaw_control"
	gain = radians(30)
	clamp_lo = radians(-30.0)
	clamp_hi = radians(30.0)
	rate_limit_dec = radians(-120)
	rate_limit_inc = radians(120)

class elevron_deflection_left(OutputChannel):
	channel = "ControlSurfaces.LeftElevronDeflection"
	clamp_lo = radians(-25)
	clamp_hi = radians(16)
	input = "de_left_f"
	gain = DegreesToRadians
	rate_limit_dec = radians(-80)
	rate_limit_inc = radians(80)

class elevron_deflection_right(OutputChannel):
	channel = "ControlSurfaces.RightElevronDeflection"
	clamp_lo = radians(-25)
	clamp_hi = radians(16)
	input = "de_right_f"
	gain = DegreesToRadians
	rate_limit_dec = radians(-80)
	rate_limit_inc = radians(80)

class aileron_deflection(OutputChannel):
	channel = "ControlSurfaces.AileronDeflection"
	clamp_lo = radians(-25)
	clamp_hi = radians(16)
	input = "da_f"
	gain = DegreesToRadians
	rate_limit_dec = radians(-80)
	rate_limit_inc = radians(80)

class elevator_deflection(OutputChannel):
	channel = "ControlSurfaces.ElevatorDeflection"
	clamp_lo = radians(-25)
	clamp_hi = radians(16)
	input = "de_f"
	gain = DegreesToRadians
	rate_limit_dec = radians(-60)
	rate_limit_inc = radians(60)

class airbrake_deflection(OutputChannel):
	channel = "ControlSurfaces.AirbrakeDeflection"
	input = "dsb"
	gain = DegreesToRadians
	clamp_lo = radians(0)
	clamp_hi = radians(60)
	rate_limit_dec = radians(-20)
	rate_limit_inc = radians(20)

class alpha_f_output(OutputChannel):
	channel = "FCS.FilteredAlpha"
	input = "alpha_f"

class pitch_input(OutputChannel):
	channel = "FCS.PitchControl"
	input = "pitch_control"

class pitch_input(OutputChannel):
	channel = "FCS.dE"
	input = "de_f"
