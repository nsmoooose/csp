from NodeConstructor import *
RegisterNetwork(globals())

#######################################################################
# control inputs

class pitch_control(InputScalarChannel):
	channel = "ControlInputs.PitchInput"

#class roll_control(InputScalarChannel):
#	channel = "ControlInputs.RollInput"

class yaw_control(InputScalarChannel):
	channel = "ControlInputs.RudderInput"

class speedbrake_control(InputScalarChannel):
	channel = "ControlInputs.AirbrakeInput"

class speedbrake_limit(InputScalarChannel):
	channel = "F16.AirbrakeLimit"
