from NodeConstructor import *
RegisterNetwork(globals())

#######################################################################
# control inputs

class pitch_control(InputScalarChannel):
	channel = "ControlInputs.PitchInput"

class roll_control(InputScalarChannel):
	channel = "ControlInputs.RollInput"

class yaw_control(InputScalarChannel):
	channel = "ControlInputs.RudderInput"

class speedbrake_control(InputScalarChannel):
	channel = "ControlInputs.AirbrakeInput"

class left_brake_input(InputScalarChannel):
	channel = "ControlInputs.LeftBrakeInput"

class right_brake_input(InputScalarChannel):
	channel = "ControlInputs.RightBrakeInput"
