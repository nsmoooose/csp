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
Helper class for formatting nested output.
"""

import sys

class Format:

  def __init__(self, file=None, indent=''):
    if file is None:
      file = sys.stdout
    self._file = file
    self._indent = indent

  def indent(self):
    self._indent = self._indent + '  '

  def dedent(self):
    self._indent = self._indent[:-2]

  def write(self, text=''):
    if text:
      print >>self._file, '%s%s' % (self._indent, text)
    else:
      print >>self._file

  def template(self, template, dict):
    template = template % dict
    n = 0
    for line in template.split('\n'):
      n += 1
      line = line.strip()
      if not line: continue
      if line == '.':
        self.write();
        continue
      indent = 0
      istr = ''
      while indent < len(line) and line[indent] == '>':
        indent += 1
        istr += '  '
      line = istr + line[indent:]
      if line:
        self.write(line)

