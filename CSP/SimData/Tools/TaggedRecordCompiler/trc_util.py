# SimData: Data Infrastructure for Simulations
# Copyright 2004 Mark Rose <mkrose@users.sourceforge.net>
#
# This file is part of SimData.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

"""
TRC util module: classes for generating tagged record ids
"""

import md5

def md5hash64(s):
	"""return a 64-bit (python long) hash of a string"""
	return long(md5.md5(s).hexdigest()[:16], 16)

def md5hash32(s):
	"""return a pair of 32-bit hashs of a string"""
	digest = md5.md5(s).hexdigest()
	return int(digest[:8], 16), int(digest[8:16], 16)

