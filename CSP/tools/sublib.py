#!/usr/bin/python
#
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
Trivial library for accessing subversion via the command line
tool.  This should really be replaced by either the subversion
python bindings (or a thin wrapper around them) once they reach
a stable state with decent documentation.
"""

import os
import os.path
import popen2
import re
import time

from CSP.base import app

class File:
  ADD = 'ADD'
  DEL = 'DEL'
  MOD = 'MOD'

  def __init__(self, path, root, mode):
    self.mode = mode
    self.root = root
    self.path = path

  def __str__(self):
    return '(%s) //%s' % (self.mode, self.path)

  def abspath(self):
    return os.path.join(self.root, self.path)


def svn_info():
  st = os.popen('svn info').readlines()
  info = {}
  for line in st:
    line = line.strip()
    idx = line.find(':')
    if idx < 0: continue
    info[line[:idx]] = line[idx+1:].strip()
  return info


def svn_st(files=None):
  root = svn_root()
  if not files:
    files = []
    path = root
  else:
    path = ' '.join(files)
  st = os.popen('svn st %s' % path).readlines()
  files = []
  for line in st:
    path = line[1:].strip()
    abspath = os.path.abspath(path)
    assert(abspath.startswith(root))
    relpath = abspath[len(root):]
    if os.path.isabs(relpath):
      relpath = relpath[1:]
    basename = os.path.basename(path)
    if basename.startswith('.svn'): continue
    if line.startswith('M'): mode = File.MOD
    elif line.startswith('A'): mode = File.ADD
    elif line.startswith('D'): mode = File.DEL
    else: continue
    files.append(File(relpath, root, mode))
  return files


#def svn_info(path):
#  info = os.popen('svn info %s' % path)
#  app.log.info(info.readlines())


def svn_rootsvn():
  path = '.svn'
  rootsvn = None
  while os.path.exists(path):
    rootsvn = path
    path = os.path.join('..', path)
  if not rootsvn:
    app.log.fatal('must run in a subversion workspace (.svn not found)')
  return rootsvn


def svn_root():
  return os.path.abspath(os.path.dirname(svn_rootsvn()))


def svn_savediff(file, target, context=100):
  path = file.path
  exit_code, out = runo('svn diff --diff-cmd diff -x "-U %d" %s' % (context, path))
  open(target, 'w').write(''.join(out))
  return exit_code


def runoe(cmd):
  process = popen2.Popen3(cmd, capturestderr=1)
  process.tochild.close()
  out = process.fromchild.readlines()
  err = process.childerr.readlines()
  exit_code = process.wait()
  if exit_code:
    app.log.debug('run "%s" failed (%d): err=%s, out=%s' % (cmd, exit_code, err, out))
  return exit_code, out, err


def runo(cmd):
  exit_code, out, err = runoe(cmd)
  return exit_code, out


def run(cmd):
  exit_code, out, err = runoe(cmd)
  return exit_code


def svn_submit_review(name, log, contents):
  info = svn_info()
  url = info.get('URL', '')
  if not url:
    app.log.fatal('unable to locate repository URL')
  root = os.path.abspath(svn_rootsvn())
  targetdir = 'reviews'
  target = os.path.join(root, '.%s' % targetdir)
  targetfile = os.path.join(target, 'change')
  if os.path.exists(targetfile):
    os.unlink(targetfile)
  while 1:
    source = os.path.join(url, targetdir)
    #print 'svn co %s %s 2>/dev/null' % (source, target)
    exit_code = run('svn co %s %s' % (source, target))
    if not exit_code: break
    app.log.debug('checkout of %s to %s failed.' % (source, target))
    parent = os.path.dirname(url)
    if parent == url:
      app.log.fatal('could not find review path; have you created one?')
    url = parent
  if not os.path.exists(targetfile):
    app.log.fatal('could not find review path; have you created one?')
  app.log.debug('checked out review path from %s' % source)
  app.log.debug('submitting changelist %s for review' % name)
  while 1:
    exit_code = run('svn rm %s' % targetfile)
    if exit_code:
      app.log.error('unable to remove %s' % targetfile)
      break
    f = open(targetfile, 'w')
    f.write(contents)
    f.close()
    exit_code = run('svn add %s' % targetfile)
    if exit_code:
      app.log.error('unable to add %s' % targetfile)
      break
    exit_code, out = runo('svn ci -m "%s" %s' % (log, targetfile))
    if not exit_code:
      ci_log = out
      break
    app.log.info('failed to submit changelist %s for review: %s' % (name, str(ci)))
    exit_code = run('svn revert %s' % targetfile)
    if exit_code:
      app.log.error('unable to revert changes to %s in order to retry commit' % targetfile)
      break
    time.sleep(1)
    exit_code = run('svn up %s' % targetfile)
    if exit_code:
      app.log.error('unable to update %s in order to retry commit' % targetfile)
      break
  if exit_code:
    print 'Failed to submit changelist %s for review; see log for details.' % name
    return 1
  rev = 0
  re_rev = re.compile(r'Committed revision (\d+)')
  for line in ci_log:
    m = re_rev.match(line)
    if m is not None:
      rev = int(m.group(1))
      break
  if rev:
    print "Changelist %s submitted for review; review id %d." % (name, rev)
  else:
    print 'Changelist %s submitted for review, but could not determine a review number'
    print 'Try running "svn log %s"' % source
  return 0

