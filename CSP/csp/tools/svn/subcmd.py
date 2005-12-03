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


import sys
import os
import os.path
import glob
import stat
import random
import tempfile
import optparse
import pickle
import zipfile
import StringIO
import time

from csp.base import app
from csp.tools.svn import sublib


def sendmail(sender, recipients, subject, body, attachments=None):
  import email
  import email.MIMEMultipart
  import email.MIMEText
  import email.MIMEBase
  import smtplib
  import mimetypes
  outer = email.MIMEMultipart.MIMEMultipart()
  outer['Subject'] = subject
  outer['To'] = ', '.join(recipients)
  outer['From'] = sender
  outer.preamble = 'You need a MIME-aware mail reader to view this message.\n'
  outer.epilogue = ''
  outer.attach(email.MIMEText.MIMEText(body))
  if attachments is not None:
    for name, content in attachments:
      ctype, encoding = mimetypes.guess_type(name)
      if ctype is None or encoding is not None:
        ctype = 'application/octet-stream'
      maintype, subtype = ctype.split('/')
      attachment = email.MIMEBase.MIMEBase(maintype, subtype)
      attachment.set_payload(content)
      email.Encoders.encode_base64(attachment)
      attachment.add_header('Content-Disposition', 'attachment', filename=name)
      outer.attach(attachment)
  s = smtplib.SMTP()
  s.connect()
  try:
    s.sendmail(sender, recipients, outer.as_string())
  except smtplib.SMTPException, e:
    return Error('error sending message: %s' % e.smtp_error)
  s.close()
  return Result(0)

def edit(text, cursor=1):
  tmpname = '.svn.cs.%d' % random.randint(0,100000000)
  tmppath = os.path.join(tempfile.gettempdir(), tmpname)
  tmpfile = os.fdopen(os.open(tmppath, os.O_CREAT|os.O_WRONLY|os.O_EXCL), 'w')
  tmpfile.write(text)
  tmpfile.close()
  mtime = os.stat(tmppath)[stat.ST_MTIME]
  os.system('vi +%d %s' % (cursor, tmppath))
  if mtime == os.stat(tmppath)[stat.ST_MTIME]:
    return Result(0)
  saved = ''.join(open(tmppath).readlines())
  os.unlink(tmppath)
  return Result(saved)


class Filter:
  def __init__(self, path):
    self._sub = 0
    if path.endswith('...'):
      path = path[:-3]
      self._sub = 1
    self._abs = os.path.abspath(path)
  def __call__(self, file):
    path = file.abspath()
    dirname = os.path.dirname(path)
    if self._sub:
      return dirname.startswith(self._abs)
    else:
      return dirname == self._abs

class Result:

  def __init__(self, value, error=''):
    self.value = value
    self.error = error
    self.ok = not error

  def __str__(self):
    if self.error:
      return 'error: %s' % self.error
    else:
      return 'result: %s' % self.value


def Error(msg):
  return Result(None, error=msg)


class Changeset:

  MODE_NAME = {
    sublib.File.ADD: 'add',
    sublib.File.DEL: 'delete',
    sublib.File.MOD: 'modified',
    sublib.File.REP: 'replaced',
    sublib.File.CONFLICT: 'XXX conflict',
  }

  def __init__(self, name, files, pending=0):
    self._name = name
    self._description = []
    self._files = files
    self._date = time.time()
    self._pending = pending

  def describe(self, outfile=sys.stdout):
    print >>outfile, ''
    print >>outfile, 'change set: %s' % self._name
    print >>outfile, 'created on: %s' % self.date()
    print >>outfile, ''
    print >>outfile, '\n'.join(self._description)
    print >>outfile, ''
    if self._files:
      for file in self._files: print >>outfile, str(file)
      print >>outfile, ''
    return Result(0)

  def submit(self):
    files = map(lambda x: x.abspath(), self._files)
    description = '\n'.join(self._description).replace("'", "'\"'\"'")
    exitcode = os.system('svn ci -m \'%s\' %s' % (description, ' '.join(files)))
    if exitcode != 0:
      return Error('error submitting changeset')
    self._pending = 0
    return Result(0)

  def edit(self, new=0):
    if not hasattr(self, '_date'):
      self._date = time.time()
    info = sublib.svn_info()
    URL = info.get('URL', '')
    rev = info.get('Revision', '')
    desc = self._description
    cursor = 5
    text = []
    addtext = text.append
    if new:
      self._pending = 1
      desc = ['<enter a description here>']
    addtext('# change set : %s' % self._name)
    addtext('# created on : %s' % self.date())
    if self._pending:
      addtext('# status     : pending')
    else:
      addtext('# status     : submitted')
    if URL:
      addtext('# repository : %s' % URL)
      cursor += 1
    if rev:
      addtext('# revision   : %s' % rev)
      cursor += 1
    addtext('')
    addtext('[description]')
    text += desc
    addtext('')
    addtext('[files]')
    fileref = {}
    for file in self._files:
      mode = file.mode
      path = file.path
      comment = Changeset.MODE_NAME.get(mode, mode.lower())
      addtext('  //%s  # %s' % (path, comment))
      fileref[path] = file
    text = '\n'.join(text)
    result = edit(text, cursor)
    if not result.ok:
      return Error('error editing description, aborting')
    if not result.value:
      return Error('description unchanged, aborting')
    lines = result.value.split('\n')
    state = 'description'
    description = []
    fileset = {}
    for line in lines:
      line = line.strip()
      if state == 'description' and line == '[description]':
        state = 'desc'
      elif state == 'desc':
        if line == '[files]':
          state = 'files'
        else:
          description.append(line)
      elif state == 'files':
        if line:
          idx = line.find('#')
          if idx >= 0:
            line = line[:idx]
          path = line.strip()
          assert path.startswith('//')
          path = path[2:]
          file = fileref.get(path, 0)
          assert file
          assert not path in fileset.keys()
          fileset[path] = file
    if description == ['<enter a description here>', '']:
      return Error('description unchanged, aborting')
    idx = 0
    description.reverse()
    for line in description:
      if line.strip(): break
      idx = idx + 1
    description.reverse()
    description = map(lambda x: x.rstrip(), description[:-idx])
    self._description = description
    self._files = fileset.values()
    return Result(0)

  def _fileIndex(self):
    index = {}
    for file in self._files:
      index[file.abspath()] = file
    return index

  def removeFileByPath(self, abspath):
    index = self._fileIndex()
    file = index.get(abspath, None)
    if not file:
      return Error('file not found in changeset %s' % self._name)
    self._files.remove(file)
    return Result(file)

  def removeFile(self, file):
    abspath = file.abspath()
    return self.removeFileByPath(abspath)

  def addFile(self, file):
    index = self._fileIndex()
    abspath = file.abspath()
    if index.get(abspath, None):
      return Error('file already in changeset %s' % self._name)
    self._files.append(file)

  def isPending(self):
    return self._pending

  def name(self):
    return self._name

  def date(self):
    return time.ctime(self._date)

  def localtime(self):
    return time.localtime(self._date)

  def description(self):
    return self._description[:]

  def files(self):
    return self._files[:]

  def datecmp(x, y):
    return cmp(x._date, y._date)

  datecmp = staticmethod(datecmp)


class Workspace:

  def __init__(self, rootsvn=''):
    self._sets = {}
    self._files = {}
    if not rootsvn: rootsvn = sublib.svn_rootsvn()
    self._load(rootsvn)
    self.cleanup()

  def _load(self, rootsvn):
    ws = os.path.join(rootsvn, '.workspace')
    if os.path.exists(ws):
      file = open(ws)
      wsp = pickle.load(file)
      self._sets = wsp.sets
      for set in self._sets.values():
        for file in set._files:
          abspath = file.abspath()
          assert(not self._files.get(abspath, 0))
          self._files[abspath] = set
    self._rootsvn = rootsvn
    self._ws = ws

  def _save(self):
    file = open(self._ws, 'w')
    pickle.dump(self, file)

  def __getstate__(self):
    return {'sets' : self._sets }

  def getChangeset(self, name):
    return self._sets.get(name, None)

  def _closed(self, file):
    abspath = file.abspath()
    return not self._files.has_key(abspath)

  def cleanup(self):
    filemap = {}
    for file in sublib.svn_st():
      filemap[file.abspath()] = file
    items = self._files.items()
    changed = 0
    for path, set in items:
      if not filemap.has_key(path):
        result = set.removeFileByPath(path)
        assert result.ok
        del self._files[path]
        changed = 1
    if changed: self.save()

  def assign(self, name, files):
    if name == 'default':
      dest = None
    else:
      dest = self.getChangeset(name)
      if not dest:
        return Error('no changeset "%s"' % name)
    files = sublib.svn_st(files)
    if not files:
      return Error('no files found')
    for file in files:
      abspath = file.abspath()
      set = self._files.get(abspath, None)
      if set:
        set.removeFile(file)
        del self._files[abspath]
      if dest:
        dest.addFile(file)
        self._files[abspath] = dest
    self.save()
    return Result(0)

  def abandon(self, name):
    cs = self.getChangeset(name)
    if not cs:
      return Error('no changeset "%s"' % name)
    #for file in cs._files:
    #  del self._files[file.abspath()]
    del self._sets[name]
    self.save()
    return Result(0)

  # TODO this Frankenstein method is in desparate need of refactoring
  def diff(self, names, revision):
    dircmd = os.environ.get('SUBSET_DIFF_DIR', '')
    onecmd = os.environ.get('SUBSET_DIFF_ONE', dircmd)
    rev = 0
    cs = None
    info = None
    if not names:
      name = 'default changeset'
      files = filter(self._closed, sublib.svn_st())
    elif len(names) > 1 or os.path.exists(names[0]):
      files = sublib.svn_st()
      diffs = []
      for file in files:
        path = file.abspath()
        for name in names:
          name = os.path.abspath(name)
          if os.path.isdir(name):
            if path.startswith(name):
              diffs.append(file)
              break
          else:
            if name == path:
              diffs.append(file)
              break
      if not diffs:
        return Result(0)
      files = diffs
    else:
      name = names[0]
      if name[0] == 'r':
        try:
          rev = int(name[1:])
        except ValueError:
          pass
      rev = abs(rev)
      if rev:
        name = 'revision %d' % rev
        info = sublib.svn_revision_info(rev)
        files = []
      else:
        cs = self.getChangeset(name)
        if not cs:
          return Error('no changeset "%s"' % name)
        files = cs.files()
        cs.describe()
    if not files and not rev: return Result(0)
    tmproot = '/tmp/subset.diff.%010d' % random.randint(1, 1000000000)
    os.mkdir(tmproot)
    cleanup = []
    if rev:
      files = sublib.svn_savediffs(tmproot, rev)
    singleton = not cs and (len(files) == 1)
    if singleton and not onecmd:
      print 'SUBSET_DIFF_ONE undefined; cannot view diff.'
      return Result(1)
    if not singleton and not dircmd:
      print 'SUBSET_DIFF_DIR undefined; cannot view diff.'
      return Result(1)
    makeindex = not singleton
    if makeindex:
      diffindex = os.path.join(tmproot, 'index.html')
      index = open(diffindex, 'w')
      index.write('<html><body><small>\n')
      if cs:
        index.write('<h3>Changeset %s</h3>\n' % cs.name())
        index.write('<i>Created on: %s</i><p/>' % cs.date())
        for line in cs.description():
          if line.strip():
            index.write('%s ' % line)
          else:
            index.write('<p/>\n')
        index.write('\n<p/>\n')
      else:
        index.write('<h3>%s</h3>\n' % name)
        if info is not None:
          index.write('<p/><i>Submitted by: %s<br/>\n' % info.author)
          index.write('Submitted on: %s</i><p/>\n' % info.date)
          index.write('%s\n' % str(info.msg).replace('\n', '<br/>\n'))
      cleanup.append(diffindex)
      if revision:
        index.write('<b>diff to revision %s</b>' % revision)
      index.write('<ul>\n')
    if rev:
      for name, path in files:
        cleanup.append(path)
        if makeindex:
          index.write('<li><a href="%s">%s</a></li>\n' % (path, name))
    else:
      for file in files:
        if makeindex:
          if os.path.isdir(file.abspath()):
            index.write('<li>%s/</li>\n' % (file.path))
            continue
        outbase = file.path.replace(os.path.sep, '~') + '.diff'
        outfile = os.path.join(tmproot, outbase)
        cleanup.append(outfile)
        exit_code = sublib.svn_savediff(file, outfile, revision)
        if makeindex:
          if exit_code:
            index.write('<li>%s <i>...unable to diff</i></li>\n' % (file.path))
          else:
            index.write('<li><a href="%s">%s</a></li>\n' % (outbase, file.path))
    if makeindex:
      index.write('</ul>\n</small></body></html>')
      index.close()
    if singleton:
      os.system('%s %s 2>/dev/null' % (onecmd, cleanup[-1]))
    else:
      os.system('%s %s 2>/dev/null' % (dircmd, tmproot))
    for path in cleanup:
      os.unlink(path)
    os.rmdir(tmproot)
    return Result(0)

  def opened(self, filters=None):
    files = sublib.svn_st()
    if filters:
      fileset = {}
      for path_filter in filters:
        filtered = filter(path_filter, files)
        for file in filtered:
          fileset[file.abspath()] = file
      files = fileset.values()
    default = filter(self._closed, files)
    for file in files:
      if file in default:
        name = 'default'
      else:
        set = self._files.get(file.abspath(), 0)
        if set:
          name = set._name
        else:
          name = '?'
      if file.mode is sublib.File.CONFLICT:
        # TODO make these escapes optional
        sys.stdout.write('\033[1;31m')
        print '>%s (%s)' % (file, name)
        sys.stdout.write('\033[0m')
      else:
        print ' %s (%s)' % (file, name)
    return Result(0)

  def change(self, name, allow_default=0):
    if not allow_default and name == 'default':
      return Error('"default" is reserved; please choose another name.')
    cs = self._sets.get(name, None)
    if cs:
      result = cs.edit()
      if result.ok: self.save()
      return result
    files = sublib.svn_st()
    files = filter(self._closed, files)
    cs = Changeset(name, files)
    result = cs.edit(new=1)
    if result.ok:
      self._sets[name] = cs
      self.save()
    return result

  def changes(self, long=False, short=False):
    if short:
      long = False
      pending = '*P*'
      submitted = '-S-'
    else:
      if self._sets: print
      pending = '*PENDING*'
      submitted = 'SUBMITTED'
    sets = self._sets.values()
    sets.sort(Changeset.datecmp)
    for set in sets:
      date = time.strftime('%Y-%M-%d %H:%M', set.localtime())
      name = set.name()
      description = set.description()
      if set.isPending():
        status = pending
      else:
        status = submitted
      print '%s %s | %-10s' % (date, status, name),
      if short:
        desc = description[0][:40]
        print ': %s' % desc
      else:
        print '\n  ' + '\n  '.join(description)
        if long:
          for file in set.files():
            print '    - %s (%s)' % (file.path, file.mode)
        print
    return Result(0)

  def describe(self, name):
    cs = self.getChangeset(name)
    if not cs:
      return Error('no changeset "%s"' % name)
    return cs.describe()

  def submit(self, name):
    if not name:
      name = 'default'
      result = self.change(name, allow_default=1)
      if not result.ok: return result
      cs = self.getChangeset(name)
    else:
      cs = self.getChangeset(name)
      if not cs:
        return Error('Unknown changeset "%s"' % name)
      result = self.change(name)
      if not result.ok: return result
    conflicts = filter(lambda x: x.mode is sublib.File.CONFLICT, sublib.svn_st())
    if conflicts:
      print 'The following files have conflicts:'
      for file in conflicts: print '  %s' % file.path
      print 'Submit anyway [yN]? ',
      input = sys.stdin.readline()
      if input.rstrip() != 'y': return Error('Changeset not submitted.')
    result = cs.submit()
    if result.ok:
      del self._sets[name]
      self.save()
    elif self._sets.has_key('default'):
      del self._sets['default']
    return result

  def review(self, name,
             save='',
             mail='',
             sender='',
             interactive=1,
             context=32):
    cs = self.getChangeset(name)
    if not cs:
      return Error('no changeset "%s"' % name)
    log = ('Code review of changeset %s (created %s)\n\n'
           '%s\n' % (cs.name(), cs.date(), '\n'.join(cs.description())))
    zipbuffer = StringIO.StringIO()
    zip = zipfile.ZipFile(zipbuffer, 'a')
    svn_root = sublib.svn_root()
    for file in cs.files():
      path = file.abspath()
      exit_code, out = sublib.runo('svn --non-interactive --diff-cmd=/usr/bin/diff'
                                   ' --extensions=-U%d diff %s' % (context, path))
      if exit_code:
        return Error('unable to diff %s' % path)

      # replace absolute paths with repository paths
      if len(out) >= 4 and out[2].startswith('--- ') and out[3].startswith('+++ '):
        file1 = out[2][4:]
        if file1.startswith(svn_root):
          out[2] = '--- /' + file1[len(svn_root):]
        file2 = out[3][4:]
        if file2.startswith(svn_root):
          out[3] = '+++ /' + file2[len(svn_root):]

      diff = ''.join(out)
      log = "%s\n%s" % (log, str(file))
      zip.writestr(file.path + '.diff', diff)
    zip.writestr('README.txt', log)
    zip.close()
    zipdata = zipbuffer.getvalue()
    if save:
      open(save, 'w').write(zipdata)
    result = Result(0)
    if mail:
      recipients = mail
      subject = 'Code review, changeset %s' % name
      body = ('%s\n'
              '___________________________________________________________________\n'
              '\n'
              'The attached zipfile contains diffs of each file in the changeset\n'
              'With a properly configured graphical mail client and diff tool, it\n'
              'should be possible to open the individual file diffs with a few\n'
              'mouse clicks.  For assistance, see http://....\n' % log)
      if interactive:
        print 'edit and save the message to send.'
        result = edit(body)
        if result.ok:
          body = result.value
          if not body: return Error('message not saved; aborting.')
      result = sendmail(sender, recipients, subject, body, attachments=[(name + '.zip', zipdata)])
      if result.ok:
        size = len(zipdata) + len(body)
        if size >= 1000000:
          size, unit = '%.1f' % (size/1024.0/1024.0), 'M'
        elif size >= 10000:
          size, unit = '%.0f' % (size/1024.0), 'K'
        elif size >= 1000:
          size, unit = '%.1f' % (size/1024.0), 'K'
        else:
          size, unit = '%d' % size, ' bytes'
        print 'message sent (%s%s).' % (size, unit)
    return result

  def save(self):
    if hasattr(self, '_ws'):
      self._save()


def run(method, *args, **kw):
  ws = Workspace()
  method = getattr(ws, method)
  return method(*args, **kw)


class Command:
  Index = {}

  def __init__(self):
    self._options = []
    self._keys = []
    self._define()

  def help(self):
    self._makeOpt().print_help()

  def _addKeys(self, *keys):
    self._keys = keys
    for key in keys:
      Command.Index[key] = self

  def _addOption(self, *args, **kw):
    self._options.append((args, kw))

  def _makeOpt(self):
    usage = '%s\n' % self._long
    opt = optparse.OptionParser(usage=usage,
                                add_help_option=0,
                                formatter=app.CustomUsageFormatter())
    for optargs, optkw in self._options:
      opt.add_option(*optargs, **optkw)
    return opt

  def _start(self, args):
    opt = self._makeOpt()
    options, args = opt.parse_args(args)
    result = self._run(options, args)
    if not result.ok:
      print result.error

  def run(command, options):
    handler = Command.Index.get(command, None)
    if handler is None:
      app.usage()
      return Error('unknown command "%s"' % command)
    return handler._start(options)
  run = staticmethod(run)


class SVNCommand(Command):

  def _svn(self, command, args):
    paths = os.environ.get('PATH', '')
    for path in paths.split(os.pathsep):
      path = os.path.join(path, 'svn')
      if os.path.exists(path): break
      path = ''
    if not path: return Error('svn not found')
    code = os.spawnv(os.P_WAIT, path, ['svn', command] + args)
    if code: return Error('svn failed')
    return Result(0)

  def _start(self, args):
    command = self._keys[0]
    return self._svn(command, args)

  def help(self):
    command = self._keys[0]
    return self._svn('help', [command,])


class Blame(SVNCommand):
  def _define(self):
    self._addKeys('blame', 'praise', 'annotate', 'ann')
    self._short = 'show revision and author for each line in a file'


class Log(SVNCommand):
  def _define(self):
    self._addKeys('log')
    self._short = 'show log messages for revisions and/or files'


class Info(SVNCommand):
  def _define(self):
    self._addKeys('info')
    self._short = 'display information about a path'


class List(SVNCommand):
  def _define(self):
    self._addKeys('list', 'ls')
    self._short = 'list diretory entries in the repository'


class Status(SVNCommand):
  def _define(self):
    self._addKeys('status', 'stat', 's')
    self._short = 'show the status of working copy files and directories'


class Cat(SVNCommand):
  def _define(self):
    self._addKeys('cat', 'print')
    self._short = 'output the content of specified files or URLs'


class Add(SVNCommand):
  def _define(self):
    self._addKeys('add')
    self._short = 'put files and directories under version control'


class Delete(SVNCommand):
  def _define(self):
    self._addKeys('delete', 'remove', 'rm')
    self._short = 'remove files and directories from version control'


class Mkdir(SVNCommand):
  def _define(self):
    self._addKeys('mkdir')
    self._short = 'create a new directory under version control'


class Copy(SVNCommand):
  def _define(self):
    self._addKeys('copy', 'cp')
    self._short = 'duplicate something in working copy or repos, with history'


class Update(SVNCommand):
  def _define(self):
    self._addKeys('update', 'up', 'sync')
    self._short = 'bring changes from the repository into the working copy'


class Revert(SVNCommand):

  def _define(self):
    self._addKeys('revert')
    self._short = 'restore pristine working copy file'

  def _start(self, args):
    result = SVNCommand._start(self, args)
    if result.ok:
      return run('cleanup')
    return result


class Opened(Command):

  def _define(self):
    self._long = ('opened (o): list files that have been modified, added, or deleted.\n'
                  '\n'
                  'usage: %prog opened [path]')
    self._short = 'show opened files'
    self._addKeys('opened', 'o')

  def _run(self, options, args):
    filters = map(Filter, args)
    return run('opened', filters)


class Changes(Command):

  def _define(self):
    self._long = ('changes: list all active changesets.\n'
                  '\n'
                  'usage: %prog changes [options]')
    self._short = 'show active changesets'
    self._addKeys('changes')
    self._addOption('-l', '--long', default=False, action='store_true', help='list full descriptions and changed files')
    self._addOption('-s', '--short', default=False, action='store_true', help='list only abbreviated descriptions')

  def _run(self, options, args):
    return run('changes', long=options.long, short=options.short)


class Assign(Command):

  def _define(self):
    self._long = ('assign (a): assign files to an existing changeset, or move files\n'
                  'from one changeset to another\n'
                  '\n'
                  'usage: %prog assign changeset file [file...]\n'
                  '\n'
                  'If changeset is "default", the files will be removed from named changesets.')
    self._short = 'reassign open files to a different changeset'
    self._addKeys('assign', 'a')

  def _run(self, options, args):
    if len(args) < 2:
      self.help()
      return Result(1)
    name = args[0]
    files = args[1:]
    return run('assign', name, files)


class Change(Command):

  def _define(self):
    self._long = ('change: create or edit a changeset\n'
                  '\n'
                  'usage: %prog change [options] changeset')
    self._short = 'create or edit a changeset'
    self._addKeys('change')
    self._addOption('-d', '--delete', default=False, action='store_true', help='delete the changeset')

  def _run(self, options, args):
    if len(args) != 1:
      self.help()
      return Result(1)
    name = args[0]
    if options.delete:
      return run('abandon', name)
    else:
      return run('change', name)


class Review(Command):

  def _define(self):
    self._long = ('review: distribute a changeset for review\n'
                  '\n'
                  'usage: %prog review [options] changeset\n'
                  '\n'
                  'Creates a zipfile containing diffs of all files in the changeset.\n'
                  'The zipfile can be saved locally or emailed to reviews along with\n'
                  'the changeset description.\n')
    self._short = 'submit a changeset for review'
    self._addKeys('review')
    self._addOption('--context', default='32', metavar='<n>', help='lines of context in diffs')
    self._addOption('--from', default='', metavar='<addr>', help='return email address')
    self._addOption('--mail', default='', metavar='<addr>', help='recipients (comma separated)')
    self._addOption('--noedit', default=False, action='store_true', help='immediate send (no edit)')
    self._addOption('--save', default='', metavar='<path>', help='save a copy of the review')

  def _run(self, options, args):
    if len(args) != 1:
      self.help()
      return Result(1)
    name = args[0]
    mail = []
    sender = getattr(options, 'from')
    if options.mail:
      mail = options.mail.split(',')
      if not sender:
        sender = os.environ.get('SVN_FROM', '')
        if not sender:
          return Error('could not determine your email address; set SVN_FROM or the --from option')
    try:
      context = int(options.context)
    except ValueError:
      context = 0
    if context < 1:
      return Error('invalid value (%s) for --context option' % options.context)
    interactive = not options.noedit
    save = options.save
    return run('review', name, mail=mail, save=save, sender=sender, interactive=interactive, context=context)


class Submit(Command):

  def _define(self):
    self._long = ('submit: submit (commit) a changeset to the repository\n'
                  '\n'
                  'usage: %prog submit changeset')
    self._short = 'submit a changeset to the repository'
    self._addKeys('submit')
    self._addOption('-d', '--default', default=False, action='store_true', help='submit directly from default changeset')

  def _run(self, options, args):
    if len(args) > 1:
      self.help()
      return Result(1)
    if not args:
      name = ''
      if not options.default:
        return Error('Must specify "-d" to submit directly from the default changeset')
    else:
      name = args[0]
    return run('submit', name)


class Abandon(Command):

  def _define(self):
    self._long = ('abandon: discard a named changeset.  files in that changeset will be reassigned\n'
                  'to the default changeset.\n'
                  '\n'
                  'usage: %prog abandon changeset')
    self._short = 'abandon a changeset'
    self._addKeys('abandon')

  def _run(self, options, args):
    if len(args) != 1:
      self.help()
      return Result(1)
    name = args[0]
    return run('abandon', name)


class Describe(Command):

  def _define(self):
    self._long = ('describe (d): show information about a changeset.\n'
                  '\n'
                  'usage: %prog describe changeset')
    self._short = 'describe a changeset'
    self._addKeys('describe', 'd')

  def _run(self, options, args):
    if len(args) != 1:
      self.help()
      return Result(1)
    name = args[0]
    return run('describe', name)


class Diff(Command):

  def _define(self):
    self._long = ('diff: generate diffs for files or changesets.\n'
                  '\n'
                  'usage: %prog diff [changeset | rREV | file [file...]]'
                  '\n'
                  'If no arguments are specified, all files in the default changeset\n'
                  'will be diffed.  The "rREV" syntax shows diffs of files submitted\n'
                  'at the specified revision (e.g. "r101"), relative to the previous\n'
                  'revision.')
    self._short = 'generate diffs'
    self._addKeys('diff')
    self._addOption('-r', '--revision', default='', metavar='REV', help='diff relative to a specific revision')

  def _run(self, options, args):
    return run('diff', args, options.revision)


class Help(Command):

  def _define(self):
    self._long = ('help (?, h): Describe the usage of this program or its subcommands.\n'
                  '\n'
                  'usage: %prog help [SUBCOMMAND...]')
    self._short = 'describe subcommands'
    self._addKeys('help', 'h', '?')

  def _run(self, options, args):
    if len(args) == 1:
      key = args[0]
      command = Command.Index.get(key, None)
      if command is None:
        return Error('unknown subcommand "%s"' % key)
      command.help()
      return Result(0)
    self.help()
    return Result(0)

  def help(self):
    app.usage()
    print 'Available commands:'
    commands = {}
    for command in Command.Index.values():
      primary = command._keys[0]
      commands[primary] = command
    primaries = commands.keys()
    primaries.sort()
    for primary in primaries:
      command = commands[primary]
      subcommand = primary
      print '  %-14s %s' % (subcommand, command._short)
    print


# register subcommands
Abandon()
Assign()
Change()
Changes()
Describe()
Diff()
Help()
Opened()
Review()
Submit()

# svn subcommands
Add()
Blame()
Cat()
Copy()
Delete()
Info()
List()
Log()
Mkdir()
Revert()
Status()
Update()

