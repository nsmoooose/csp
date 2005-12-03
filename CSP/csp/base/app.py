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
Define a standard framework for running python applications.

Applicitions using this module should be structured as follows:

  #! /.../pythonX
  # copyright notices
  docstring (for usage)
  imports
  classes and functions
  def main(args): ...
  app.start()

The docstring serves as the usage text that is printed when --help is
specified.  It can include %(prog)s and %(progpath)s substitutions.

main() is the main entry point for the application.  Non-flag command
line arguments are passed in (excluding the program name), and the
return value serves as the exit code for the application.  The program
name and absolute path can be accessed using app.programName() and
app.programPath(), respectively.  Command line flags are specified
using app.addOption(...) (same arguments as optparse add_option), and
accessed by app.options.<flagname>.

The call to app.start() can optionally be placed inside an if __name__
== '__main__' block.  This is not necessary, however, since app.start()
does nothing if called from a non-main module.
"""

import sys
import os.path
import inspect
import optparse
import exceptions
import logging
import applog


class CustomUsageFormatter(optparse.IndentedHelpFormatter):
  def format_usage(self, usage):
    return usage % {'prog' : programName(), 'progpath' : programPath()}
  def format_option(self, option):
    text = optparse.IndentedHelpFormatter.format_option(self, option).rstrip()
    if not option.default is optparse.NO_DEFAULT and option.default:
      text += ' [default: %s]' % str(option.default)
    return text + '\n'

log = logging.getLogger('app')

def _fatal(msg, *args, **kw):
  """
  Log a critical message and terminate the application, with an optional exit
  code.

  Args:
    same as logging.critical()
    optional keyword argument 'exit_code' to set the exit code (default 1).
  """
  log.critical(msg, *args, **kw)
  code = kw.get('exit_code', 1)
  sys.exit(code)

# override logging.fatal, which is deprecated and equivalent to critical().
# the replacement logs a critical message and terminates the application.
log.fatal = _fatal

opt = optparse.OptionParser(usage='', formatter=CustomUsageFormatter())
addOption = opt.add_option

addOption('-v', '--verbose', default='info', type='string', metavar='<level>',
          help='set verbosity level (numeric, or by name)')
addOption('--logfile', default='', type='string', help='log file (default stderr)')

def usage():
  """Print usage to stdout."""
  opt.print_help()
  print

def fatal(msg):
  """Print an error message to stderr and terminate with exit code 1."""
  print >>sys.stderr, msg
  sys.exit(1)

def programPath():
  """Return the absolute path of the application program."""
  return os.path.abspath(sys.argv[0])

def programName():
  """Return the basename of the application program."""
  return os.path.basename(sys.argv[0])

def start(disable_interspersed_args=0):
  """
  Start the application.

  Parses arguments, sets up logging, and calls main(args).

  Args:
    disable_interspersed_args: if true, command line flags must preceed
      positional arguments.  otherwise, command line flags can be mixed
      with positional arguments.
  """
  if disable_interspersed_args:
    opt.disable_interspersed_args()
  result = 0
  try:
    stack = inspect.stack()
    frame = stack[1][0]
    name = frame.f_globals.get('__name__', '')
    if name == '__main__':
      doc = frame.f_globals.get('__doc__', None)
      if isinstance(doc, str):
        if doc.startswith('\n'): doc = doc[1:]  # ignore first newline in triple-quoted strings
      opt.set_usage(doc)
      main = frame.f_globals.get('main', None)
      global options
      options, args = opt.parse_args()
      if options.logfile:
        logfile = open(options.logfile, 'w')
      else:
        logfile = sys.stderr
      loghandler = logging.StreamHandler(logfile)
      loghandler.setFormatter(applog.formatter)
      loghandler.setLevel(logging.NOTSET)
      log.addHandler(loghandler)
      try:
        level = int(options.verbose)
      except ValueError:
        levelname = options.verbose.upper()
        level = logging._levelNames.get(levelname, -1)
        if level == -1:
          levels = logging._levelNames.keys()
          levels = [x.lower() for x in levels if not isinstance(x, int)]
          levels.sort()
          print >>sys.stderr, 'logging verbosity "%s" unrecognized; valid level names are:' % options.verbose
          print >>sys.stderr, '  (' + ','.join(levels) + ')'
          sys.exit(1)
      log.setLevel(level)
      if main: result = main(args)
  except KeyboardInterrupt:
    log.exception('caught keyboard interrept; aborting.')
  except exceptions.SystemExit, e:
    raise e
  except Exception, e:
    log.exception('caught exception %s; aborting' % e)
  sys.exit(result)


