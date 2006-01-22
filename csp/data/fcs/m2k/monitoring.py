from NodeConstructor import *
RegisterNetwork(globals())


#######################################################################
#output channels for monitoring

class elevator_pitch_deflection_command_output(OutputChannel):
	channel = "FCS.ElevatorPitchDeflectionCommand"
	input = "elevator_pitch_deflection_command"

class limited_g_command_f_output(OutputChannel):
	channel = "FCS.FilteredLimitedGCommand"
	input= "limited_g_command_f"

class pitch_control_output(OutputChannel):
	channel = "FCS.PitchControl"
	input = "pitch_control"

class g_command_output(OutputChannel):
	channel = "FCS.GCommand"
	input = "g_command"

class negative_g_limit_output(OutputChannel):
	channel = "FCS.NegativeGLimit"
	input = "negative_g_limit"

class positive_g_limit_output(OutputChannel):
	channel = "FCS.PositiveGLimit"
	input = "positive_g_limit"

class qbar_output(OutputChannel):
	channel = "FCS.DynamicPressure"
	input = "qbar"

class elevator_deviation_output(OutputChannel):
	channel = "FCS.ElevatorDeviation"
	input = "elevator_deviation"

class reduced_elevator_deviation_output(OutputChannel):
	channel = "FCS.ReducedElevatorDeviation"
	input = "reduced_elevator_deviation"

class elevator_deviation_integrator_output(OutputChannel):
	channel = "FCS.ElevatorDeviationIntegrator"
	input = "elevator_deviation_integrator"

#class compensated_elevator_deviation_output(OutputChannel):
#	channel = "FCS.CompensatedElevatorDeviation"
#	input = "compensated_elevator_deviation"

class g_minus_1_output(OutputChannel):
	channel = "FCS.GMinus1"
	input = "g_minus_1"

class filtered_pitch_rate_output(OutputChannel):
	channel = "FCS.FilteredPitchRate"
	input = "pitch_rate_f"

class alpha_f_output(OutputChannel):
	channel = "FCS.FilteredAlpha"
	input = "alpha_f"
