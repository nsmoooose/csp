# Copyright 2005 Mark Rose <mkrose@users.sourceforge.net>
# All rights reserved.

"""
Generic flight control system to use as a default for new aircraft.
"""

from NodeConstructor import *
RegisterNetwork(globals())

import constants
import inputs
import sensors
import outputs
import nws
import pitch
import roll
#import yaw
import speedbrake
import monitoring


