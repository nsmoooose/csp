#!/usr/bin/python3

# Combat Simulator Project
# Copyright 2004 Mark Rose <mkrose@users.sourceforge.net>
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
Standalone compiler for converting tagged record definitions
into C++ classes.
"""


import sys
import os.path
from optparse import OptionParser, HelpFormatter

import trc_gen
from trc_grammar import yacc


def error(msg):
    print(msg, file=sys.stderr)
    sys.exit(1)


def validate(declarations):
    pass


def generate(input, header, source):
    """Parse the input text string and generate C++ source and header files."""

    declarations = yacc.parse(input)
    declarations = declarations + [trc_gen.Footer()]

    header_output = trc_gen.Output.Header(open(header, 'wt'))
    source_output = trc_gen.Output.Source(open(source, 'wt'))

    validate(declarations)
    for declaration in declarations:
        header_output(declaration)
        source_output(declaration)


# Arg, it would be nice if this was the default for optparse.  The "usage:" string
# in the default help formatter gets in the way of the command title.
class SimpleHelpFormatter(HelpFormatter):
    def __init__(self, indent_increment=2, max_help_position=24, width=80, short_first=1):
        HelpFormatter.__init__(self, indent_increment, max_help_position, width, short_first)

    def format_usage(self, usage):
        return "%s\n" % usage

    def format_heading(self, heading):
        return "%*s%s:\n" % (self.current_indent, "", heading)


def main():
    usage = ('Tagged Record Compiler\n\n'
             'Usage: %s [options] input' % sys.argv[0])
    parser = OptionParser(usage=usage, formatter=SimpleHelpFormatter())
    parser.add_option("--header", help="output header file (required)", metavar="FILE")
    parser.add_option("--source", help="output source file (required)", metavar="FILE")
    (options, args) = parser.parse_args()

    header = options.header
    source = options.source

    if not header or not source:
        parser.error('must specify header and source output files')

    if args:
        if len(args) > 1:
            parser.error('more than one input file specified')
        input = open(args[0]).read()
    else:
        input = sys.stdin.read()

    generate(input, header, source)


if __name__ == '__main__':
    main()
