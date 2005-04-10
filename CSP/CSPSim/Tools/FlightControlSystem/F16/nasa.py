# Copyright 2005 Mark Rose <mkrose@users.sourceforge.net>
# All rights reserved.

"""
Implementation of the F-16A flight control system based on the Nasa
tp1538 paper (1979).
"""

from NodeConstructor import *
RegisterNetwork(globals())

import inputs
import sensors
import outputs
import pitch
import roll
import yaw
import speedbrake
import leadingedge
import trailingedge
import monitoring


