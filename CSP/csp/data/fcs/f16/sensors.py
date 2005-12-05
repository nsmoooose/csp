# Copyright 2005 Mark Rose <mkrose@users.sourceforge.net>
# All rights reserved.

from NodeConstructor import *
RegisterNetwork(globals())


#######################################################################
# flight measurements

class alpha(InputScalarChannel):
	"""Angle of attack in degrees."""
	channel = "FlightDynamics.Alpha"
	scale = RadiansToDegrees
	clamp_lo = -5.0
	clamp_hi = 30.0

class qbar(InputScalarChannel):
	"""Dynamic pressure in si units."""
	channel = "FlightDynamics.QBar"

#class ps(InputScalarChannel):
#	channel = "Conditions.Pressure"

class G(InputScalarChannel):
	"""Normal acceleration in units of nominal gravitational acceleration at sea level."""
	channel = "FlightDynamics.G"

#class normal_accelation(InputVectorZChannel):
#	channel = "Kinetics.AccelerationBody"

class lateral_acceleration(InputVectorXChannel):
	"""Lateral acceleration (body X-axis) in si units."""
	channel = "Kinetics.AccelerationBody"

class pitch_rate(InputVectorXChannel):
	"""Pitch rate in radians/sec."""
	channel = "Kinetics.AngularVelocityBody"

class roll_rate(InputVectorYChannel):
	"""Roll rate in radians/sec."""
	channel = "Kinetics.AngularVelocityBody"

class yaw_rate(InputVectorZChannel):
	"""Yaw rate in radians/sec."""
	channel = "Kinetics.AngularVelocityBody"


#######################################################################
# derived data

class g_minus_1(Scale):
	"""G - 1"""
	input = "G"
	offset = -1.0

class alpha_f(LagFilter):
	"""Lowpass filtered angle of attack."""
	input = "alpha"
	a = 10.0




