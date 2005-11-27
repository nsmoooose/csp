# Copyright 2004 Mark Rose <mkrose@users.sourceforge.net>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

"""
Define standard formating for application logs.
"""

import sys
import logging

logging.addLevelName(logging.DEBUG, 'D')
logging.addLevelName(logging.INFO, 'I')
logging.addLevelName(logging.WARNING, 'W')
logging.addLevelName(logging.ERROR, 'E')
logging.addLevelName(logging.CRITICAL, 'C')

formatter = logging.Formatter('%(asctime)s%(levelname)s_%(module)s.%(lineno)d %(message)s', '%m%d%H%M%S')

