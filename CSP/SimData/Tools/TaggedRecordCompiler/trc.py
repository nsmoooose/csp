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

#ifndef __%(module_guard)s_TRF_H__
#define __%(module_guard)s_TRF_H__

#include <SimData/TaggedRecord.h>
%(includes)s
"""

FOOTER = """
#endif // __%(module_guard)s_TRF_H__
"""

SOURCE_HEADER="""// generated file --- do not edit!'

#include <SimData/TaggedRecordRegistry.h>
#include %(include_as)s
"""

SOURCE_FOOTER = """
"""

class DataPackCompiler:

  re_line = re.compile(r'^(\w*:?:?\w+)(\[\])?\s+(\w*)\s*((?:\s+\w+\s*=\s*[a-zA-Z0-9_:]+)*)\s*({|})?$')
  re_opts = re.compile(r'\s*=\s*')
  re_ns = re.compile(r'namespace\s+([a-zA-Z_][a-zA-Z0-9]*)\s*;?')

  class Node:
    pass

  def __init__(self, file, header, source):
    self._root = None
    self._active = None
    self._header = header
    self._source = source
    self._header_file = open(header, 'wt')
    self._source_file = open(source, 'wt')
    self._parse(file)

  def _parse(self, source):
    module = os.path.basename(source)
    module = os.path.splitext(module)[0]
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
    include_as = '"%s"' % os.path.basename(self._header)
    header = 1
    for line in file:
      line_number = line_number + 1
      trim = line.strip()
      if not trim: continue
      if header and trim.startswith('#include'):
        includes.append(trim)
        continue
      if header and trim.startswith('#header'):
        include_as = trim[7:].strip()
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

    module_guard = module.replace('.', '_').upper()
    values = {'module_guard': module_guard, 'module': module, 'includes': includes};
    print >>self._header_file, HEADER % values
    if namespace: print >>self._header_file, 'namespace %s {' % namespace
    [x.dump(file=self._header_file) for x in toplevel]
    if namespace: print >>self._header_file, '} // namespace %s' % namespace
    print >>self._header_file, FOOTER % values

    source_values = {'include_as': include_as};
    print >>self._source_file, SOURCE_HEADER % source_values
    if namespace: print >>self._source_file, 'namespace %s {' % namespace
    [x.dump_source(file=self._source_file) for x in toplevel]
    if namespace: print >>self._source_file, '} // namespace %s' % namespace
    print >>self._source_file, SOURCE_FOOTER % source_values

  def error(self, line_number, msg):
    print >>sys.stderr, 'line %d: %s' % (line_number, msg)
    sys.exit(1)


def main(args):
  if len(args) != 1:
    app.usage()
    return 1
  file = args[0]
  base = os.path.splitext(os.path.basename(file))[0]
  header = base + '.h'
  source = base + '.cpp'
  if app.options.header:
    header = app.options.header
  if app.options.source:
    source = app.options.source
  DataPackCompiler(file, header, source)
  return 0

app.addOption('--header', metavar='FILE', default='', help='output header (default infile.h)')
app.addOption('--source', metavar='FILE', default='', help='output source (default infile.cpp)')
app.start()

