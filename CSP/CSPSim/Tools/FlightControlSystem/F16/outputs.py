from NodeConstructor import *
RegisterNetwork(globals())


#######################################################################
# output channels

class rudder_deflection(OutputChannel):
	channel = "ControlSurfaces.RudderDeflection"
	clamp_lo = radians(-30.0)
	clamp_hi = radians(30.0)
	input = "dr_f"
	gain = DegreesToRadians
	rate_limit_dec = radians(-120)
	rate_limit_inc = radians(120)

class aileron_deflection_left(OutputChannel):
	channel = "ControlSurfaces.LeftAileronDeflection"
	clamp_lo = radians(-21.5)
	clamp_hi = radians(21.5)
	input = "da_left"
	gain = DegreesToRadians
	rate_limit_dec = radians(-80)
	rate_limit_inc = radians(80)

class aileron_deflection_right(OutputChannel):
	channel = "ControlSurfaces.RightAileronDeflection"
	clamp_lo = radians(-21.5)
	clamp_hi = radians(21.5)
	input = "da_right"
	gain = DegreesToRadians
	rate_limit_dec = radians(-80)
	rate_limit_inc = radians(80)

class aileron_deflection(OutputChannel):
	channel = "ControlSurfaces.AileronDeflection"
	clamp_lo = radians(-21.5)
	clamp_hi = radians(21.5)
	input = "da_f"
	gain = DegreesToRadians
	rate_limit_dec = radians(-80)
	rate_limit_inc = radians(80)

class left_elevator_deflection(OutputChannel):
	channel = "ControlSurfaces.LeftElevatorDeflection"
	clamp_lo = radians(-25)
	clamp_hi = radians(25)
	input = "de_left_f"
	rate_limit_dec = radians(-60)
	rate_limit_inc = radians(60)

class right_elevator_deflection(OutputChannel):
	channel = "ControlSurfaces.RightElevatorDeflection"
	clamp_lo = radians(-25)
	clamp_hi = radians(25)
	input = "de_right_f"
	rate_limit_dec = radians(-60)
	rate_limit_inc = radians(60)

class elevator_deflection(OutputChannel):
	channel = "ControlSurfaces.ElevatorDeflection"
	clamp_lo = radians(-25)
	clamp_hi = radians(25)
	input = "de_f"
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

class leading_edge_deflection(OutputChannel):
	channel = "ControlSurfaces.LeadingEdgeDeflection"
	clamp_lo = radians(-2)
	clamp_hi = radians(25)
	input = "dlef"
	gain = DegreesToRadians
	rate_limit_dec = radians(-25)
	rate_limit_inc = radians(25)

class nose_wheel_steering(OutputChannel):
	channel = "LandingGear.FrontGear.SteeringInput"
	clamp_lo = radians(-32)
	clamp_hi = radians(32)
	input = "yaw_control"
	gain = radians(32)
	rate_limit_dec = radians(-120)
	rate_limit_inc = radians(120)

