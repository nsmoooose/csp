#!/usr/bin/python
#
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
Standalone compiler for converting tagged record descriptions
into C++ classes.

Usage: %(prog)s [options] infile
"""

import sys
import os
import os.path
import re

import bootstrap
from CSP.base import app

import BaseTypes


HEADER="""// generated file --- do not edit!'

#ifndef __%(module_uc)s_TRF_H__
#define __%(module_uc)s_TRF_H__

#include <SimData/TaggedRecord.h>
%(includes)s
"""

FOOTER = """
#endif // __%(module_uc)s_TRF_H__
"""


class DataPackCompiler:

  re_line = re.compile(r'^(\w*:?:?\w+)(\[\])?\s+(\w*)\s*((?:\s+\w+\s*=\s*\w+)*)\s*({|})?$')
  re_opts = re.compile(r'\s*=\s*')
  re_ns = re.compile(r'namespace\s+([a-zA-Z_][a-zA-Z0-9]*)\s*;?')

  class Node:
    pass

  def __init__(self, file, outfile):
    self._root = None
    self._active = None
    self._outfile = outfile
    self._parse(file)

  def _parse(self, source):
    module = os.path.basename(source)
    module = os.path.splitext(module)[0]
    module = module.replace('.', '_')
    self.source = source
    file = open(source)
    line_number = 0
    depth = 0
    dp = DataPackCompiler
    object = None
    object_stack = []
    toplevel = []
    toplevel_types = {}
    includes = []
    namespace = ''
    header = 1
    for line in file:
      line_number = line_number + 1
      trim = line.strip()
      if not trim: continue
      if header and trim.startswith('#include'):
        includes.append(trim)
        continue
      if header and trim.startswith('namespace'):
        m = DataPackCompiler.re_ns.match(trim)
        if m:
          if namespace: self.error('multiple namespace declarations')
          namespace = m.groups(1)
          continue
        else:
          self.error('syntax error')
      if trim.startswith('//'):
        continue
      header = 0
      while trim:
        rest = ''
        idx1 = trim.find('{')
        idx2 = trim.find(';')
        if idx1 >= 0 and (idx2 < 0 or idx2 > idx1):
          rest = trim[idx1+1:].strip()
          trim = trim[:idx1+1].strip()
        elif idx2 >= 0:
          rest = trim[idx2+1:].strip()
          trim = trim[:idx2].strip()
        while trim.startswith('}'):
          if depth < 1:
            self.error(line_number, 'unmatched brace')
          depth = depth - 1
          object_stack = object_stack[:-1]
          trim = trim[1:]
        if not trim:
          trim = rest
          continue
        m = dp.re_line.match(trim)
        if m is None:
          print trim
          self.error(line_number, 'invalid syntax.')
        type, array, id, options, block = m.groups()
        if type is None:
          self.error(line_number, 'invalid syntax (no type)')
        type_class = None
        if object_stack:
          parent = object_stack[-1]
          type_class = parent.localtypes.get(type, None)
        if type_class is None:
          type_class = BaseTypes.TYPEMAP.get(type, None)
        if type_class is None:
          type_class = toplevel_types.get(type, None)
        if type_class is None:
          self.error(line_number, 'unknown data type "%s"' % type)
        if not id and not type_class.ANONYMOUS:
          self.error(line_number, 'invalid syntax (unnamed field)')
        if options is not None:
          options = dp.re_opts.sub('=', options).strip()
          option_list = options.split()
          options = {}
          for option_pair in option_list:
            option, value = option_pair.split('=')
            options[option] = value
        if array:
          object = BaseTypes.ArrayType(id, type_class, opts=options)
        else:
          object = type_class(id, opts=options)
        if len(object_stack) > 0:
          object_stack[-1].add(object)
        else:
          if not type_class.TOPLEVEL:
            self.error(line_number, 'syntax error')
          toplevel.append(object)
          toplevel_types[id] = object
        if block is not None:
          if block == '{':
            # TODO check that type supports blocks
            depth = depth + 1
            object_stack.append(object)
          else:
            if depth < 1:
              self.error(line_number, 'unmatched brace')
            depth = depth - 1
            object_stack = object_stack[:-1]
        trim = rest
    if depth > 0:
      self.error(line_number, 'unmatched brace')
    includes = '\n'.join(includes)
    values = {'module_uc': module.upper(), 'module': module, 'includes': includes};
    print >>self._outfile, HEADER % values
    if namespace: print >>self._outfile, 'namespace %s {' % namespace
    [x.dump(file=self._outfile) for x in toplevel]
    if namespace: print >>self._outfile, '} // namespace %s' % namespace
    print >>self._outfile, FOOTER % values

  def error(self, line_number, msg):
    print >>sys.stderr, 'line %d: %s' % (line_number, msg)
    sys.exit(1)


def main(args):
  if len(args) != 1:
    app.usage()
    return 1
  output = app.options.output
  if output:
    outfile = open(output, 'wt')
  else:
    outfile = sys.stdout
  file = args[0]
  DataPackCompiler(file, outfile)
  return 0

app.addOption('-o', '--output', metavar='FILE', default='', help='output file (default stdout)')
app.start()

