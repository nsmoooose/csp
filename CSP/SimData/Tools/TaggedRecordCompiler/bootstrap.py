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

try:
  import CSP
except:
  import sys
  import os.path
  dn = os.path.dirname
  csp_root = dn(dn(dn(dn(dn(__file__)))))
  sys.path.insert(0, csp_root)
  try:
    import CSP
  except:
    print 'Unable to import the main CSP module.  Check that you have'
    print 'a complete working copy.'
    sys.exit(1)
