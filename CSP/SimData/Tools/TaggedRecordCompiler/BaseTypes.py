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
Data types supported by the trf format.
"""

import sys

import CodeFormat

import md5
def md5hash64(s):
  """return a 64-bit (python long) hash of a string"""
  return long(md5.md5(s).hexdigest()[:16], 16)

def md5hash32(s):
  """return a pair of 32-bit hashs of a string"""
  digest = md5.md5(s).hexdigest()
  return int(digest[:8], 16), int(digest[8:16], 16)


class Declaration:
  DEFAULT = 0
  COMPOUND = 0
  MULTIPLE = 0
  TOPLEVEL = 0
  ANONYMOUS = 0
  TYPEDEF = 0
  ANONYMOUS_COUNT = {}
  DELETE = 0
  POINTER = 0

  def __init__(self, id, opts=None, name=None):
    if id is None: id = ''
    anon = self.__class__.ANONYMOUS
    if anon:
      assert id == ''
      count = Declaration.ANONYMOUS_COUNT.setdefault(anon, 0)
      id = '%s%d' % (anon, count)
      Declaration.ANONYMOUS_COUNT[anon] = count + 1
    if not self.__class__.DEFAULT:
      assert opts is None or not opts.has_key('DEFAULT')
    self.id = id
    self.opts = opts
    self.child = None
    self.elements = []
    self.localtypes = {}
    self.name = name
    self.init()

  def dump_init(self, format, comma):
    text = '%s(%s)' % (self.varname(), self.default())
    if comma: text = text + ','
    format.write(text)

  def dump_clear(self, format):
    format.write('void clear_%s() { m_has_%s = 0; }' % (self.id, self.id))

  def init(self): pass
  def dump_private(self, format): assert 0
  def dump_public(self, format): assert 0
  def dump_delete(self, format): assert 0
  def dump_save(self, format): assert 0
  def dump_load(self, format): assert 0
  def dump_print(self, format): assert 0
  def default(self):
    if self.isPointer(): return 'NULL'
    return self.opts.get('DEFAULT', '')
  def fulltype(self):
    typename = self.typename()
    if self.isPointer(): typename = 'simdata::Ref<%s>' % typename
    return typename
  def typename(self): return self.__class__.TYPENAME
  def isTypedef(self): return self.__class__.TYPEDEF
  def needsDelete(self): return self.__class__.DELETE
  def isPointer(self): return self.__class__.POINTER
  def isCompound(self): return self.__class__.COMPOUND
  def isDeprecated(self): return self.opts.get('DEPRECATED', 0)
  def varname(self):
    if self.name: return self.name
    return 'm_' + self.id


class CompoundType(Declaration):
  COMPOUND = 1

  def add(self, element):
    if element.isTypedef():
      self.localtypes[element.id] = element
    else:
      self.elements.append(element)

  def classdef(self, format, base=None):
    format.write()
    if base is None:
      format.write('class %s {' % (self.id))
    else:
      format.write('class %s: public %s {' % (self.id, base))
    if self.localtypes:
      format.write('public:')
      format.write()
      format.indent()
      for typedef in self.localtypes.values():
        typedef.dump(format)
        format.write()
      format.dedent()
      format.write()
    format.write('private:')
    format.write()
    format.indent()
    if self.elements:
      format.write('enum {')
      format.indent()
      idx = 8
      for element in self.elements:
        format.write('TAG_%s = %d,' % (element.id, idx))
        idx = idx + 1
      format.dedent()
      format.write('};')
      format.write()
    for element in self.elements:
      format.write('simdata::int32 m_has_%s : 1;' % (element.id))
    format.write()
    for element in self.elements:
      element.dump_private(format);
    format.dedent()
    format.write()
    format.write('public:')
    format.write()
    format.indent()
    if self.elements:
      format.write('%s():' % self.id)
      format.indent()
      for idx, element in enumerate(self.elements):
        format.write('m_has_%s(0),' % element.id)
        comma = (idx + 1) < len(self.elements)
        element.dump_init(format, comma)
      format.dedent()
      format.write('{ }')
      format.write()
    deletes = [x for x in self.elements if x.needsDelete()]
    if deletes:
      format.write('~%s() {' % self.id)
      format.indent()
      for delete in deletes:
        delete.dump_delete(format)
      format.dedent()
      format.write('}')
      format.write()
    for element in self.elements:
      format.write('bool has_%s() const { return m_has_%s; }' % (element.id, element.id))
      element.dump_clear(format)
    format.write()
    for element in self.elements:
      element.dump_public(format);
      format.write()
    self._declaration(format)
    format.dedent()
    format.write('public:')
    format.indent()
    self._save(format)
    self._load(format)
    self._print(format)
    self._extra(format)
    format.dedent()
    format.write('};')
    format.write()

  def _declaration(self, format):
    pass

  def _save(self, format):
    format.write('void serialize(simdata::TagWriter &writer) const {')
    format.indent()
    format.write('writer.beginCompound();')
    for element in self.elements:
      format.write('if (m_has_%s) {' % element.id)
      format.indent()
      format.write('writer.writeTag(TAG_%s);' % element.id)
      element.dump_save(format)
      format.dedent()
      format.write('}')
    format.write('writer.endCompound();')
    format.dedent()
    format.write('}')

  def _load(self, format):
    format.write('void serialize(simdata::TagReader &reader) {')
    format.indent()
    format.write('reader.beginCompound();')
    format.write('while (1) {')
    format.indent()
    format.write('int tag = reader.nextTag();')
    format.write('if (!tag) break;')
    format.write('switch (tag) {')
    for element in self.elements:
      format.write('case TAG_%s: {' % element.id)
      format.indent()
      format.write('m_has_%s = 1;' % element.id)
      element.dump_load(format)
      format.write('break; }')
      format.dedent()
    format.write('default: break;')
    format.write('}')
    format.dedent()
    format.write('}')
    format.write('reader.endCompound();')
    format.dedent()
    format.write('}')

  def _print(self, format):
    format.write('void dump(std::ostream &os, int level=0) const {')
    format.indent()
    format.write('simdata::Indent indent(level);')
    format.write('os << "%s {\\n";' % self.id)
    format.write('++indent;')
    for element in self.elements:
      format.write('if (m_has_%s) {' % element.id)
      format.indent()
      format.write('os << indent << "%s: ";' % element.id)
      element.dump_print(format)
      format.write('os << "\\n";')
      format.dedent()
      format.write('}')
    format.write('--indent;')
    format.write('os << indent << "}";')
    format.dedent()
    format.write('}')

  def _extra(self, format):
    pass

  def dump_save(self, format):
    if self.isPointer():
      format.write('%s->serialize(writer);' % self.varname())
    else:
      format.write('%s.serialize(writer);' % self.varname())

  def dump_load(self, format):
    if self.isPointer():
      type = self.typename()
      format.write('%s = new %s();' % (self.varname(), type))
      format.write('%s->serialize(reader);' % self.varname())
    else:
      format.write('%s.serialize(reader);' % self.varname())

  def dump_print(self, format):
    if self.isPointer():
      format.write('%s->dump(os, indent.level);' % self.varname())
    else:
      format.write('%s.dump(os, indent.level);' % self.varname())


class SimpleType(Declaration):
  DEFAULT = 1

  CT_SET = '''
  void set_%(id)s(%(type)s const & value) {
  >m_has_%(id)s = true;
  >m_%(id)s = value;
  }
  '''

  def dump_private(self, format):
    self.dump_decl(format)

  def dump_public(self, format):
    self.dump_get(format)
    self.dump_set(format)

  def dump_decl(self, format):
    type = self.typename()
    format.write('%s %s;' % (type, self.varname()))

  def dump_get(self, format):
    type = self.typename()
    id = self.id
    format.write('%s const & %s() const { return m_%s; }' % (type, id, id))

  def dump_set(self, format):
    if self.isDeprecated(): return
    d = {'id': self.id, 'type': self.typename()}
    format.template(SimpleType.CT_SET, d)
    return
    type = self.typename()
    id = self.id
    format.write('void set_%s(%s const &value) {' % (id, type))
    format.indent()
    format.write('m_has_%s = true;' % (id))
    format.write('%s = value;' % self.varname())
    format.dedent()
    format.write('}')

  def dump_save(self, format):
    format.write('writer.writer << %s;' % self.varname())

  def dump_load(self, format):
    format.write('reader.reader >> %s;' % self.varname())

  def dump_print(self, format):
    format.write('os << %s;' % self.varname())


class ArrayType(Declaration):

  def __init__(self, id, childtype, opts=None):
    Declaration.__init__(self, id, opts=opts)
    self.child = childtype(id, name='%s[i]' % self.varname())

  CT_SET = '''
  std::vector< %(type)s > & set_%(id)s() {
  >m_has_%(id)s = true;
  >return %(name)s;
  }
  '''

  def dump_private(self, format):
    self.dump_decl(format)

  def dump_public(self, format):
    self.dump_get(format)
    self.dump_set(format)

  def dump_decl(self, format):
    type = self.child.fulltype()
    format.write('std::vector< %s > %s;' % (type, self.varname()))

  def dump_clear(self, format):
    format.write('void clear_%s() {' % self.id)
    format.indent()
    format.write('m_has_%s = 0;' % self.id)
    format.write('%s.clear();' % self.varname())
    format.dedent()
    format.write('}')

  def dump_get(self, format):
    type = self.child.fulltype()
    id = self.id
    name = self.varname()
    format.write('std::vector< %s > const & %s() const { return %s; }' % (type, id, name))

  def dump_set(self, format):
    if self.isDeprecated(): return
    type = self.child.fulltype()
    d = {'id': self.id, 'type': type, 'name': self.varname()}
    format.template(ArrayType.CT_SET, d)

  def dump_save(self, format):
    format.write('writer.writeLength(%s.size());' % self.varname())
    format.write('for (int i=0; i < %s.size(); ++i) {' % self.varname())
    format.indent()
    self.child.dump_save(format)
    format.dedent()
    format.write('}')

  def dump_load(self, format):
    format.write('int %s_len = reader.readLength();' % self.id)
    format.write('%s.resize(%s_len);' % (self.varname(), self.id))
    format.write('for (int i=0; i < %s_len; ++i) {' % self.id)
    format.indent()
    self.child.dump_load(format)
    format.dedent()
    format.write('}')

  def dump_print(self, format):
    format.write('os << "[\\n";')
    format.write('++indent;')
    format.write('for (int i=0; i < %s.size(); ++i) {' % self.varname())
    format.indent()
    self.child.dump_print(format)
    format.write('os << "\\n";')
    format.dedent()
    format.write('}')
    format.write('--indent;')
    format.write('os << "]";')


class TrfType(SimpleType):
  def typename(self):
    return 'simdata::' + SimpleType.typename(self)


class Message(CompoundType):
  DEFAULT = 0
  MULTIPLE = 0
  TOPLEVEL = 1
  TYPEDEF = 0
  POINTER = 1
  DELETE = 0
  TYPENAME = 'Message'

  def __call__(self, id, opts=None, name=None):
    message = Message(id, opts=opts, name=name)
    message.TYPENAME = self.id
    return message

  def typename(self): return self.TYPENAME

  def dump(self, format=None, file=file):
    if not format:
      format = CodeFormat.Format(file=file)
    base = self.opts.get('BASE', 'simdata::TaggedRecord')
    self.classdef(format, base)

  def _extra(self, format):
    CompoundType._extra(self, format)
    name = self.id
    try:
      version = int(self.opts.get('VERSION', 0))
    except ValueError:
      print >>sys.stderr, 'ERROR: VERSION option on %s must be an integer' % name
      sys.exit(1)
    id0, id1 = md5hash32('%s_%d' % (name, version))
    d = {'name': name, 'version': version, 'id0': id0, 'id1': id1}
    format.template(Message.TRF_GETID, d)
    format.template(Message.TRF_GETVERSION, d)
    format.template(Message.TRF_GETNAME, d)

  def dump_private(self, format):
    format.write('simdata::Ref<%s> %s;' % (self.typename(), self.varname()))

  def dump_public(self, format):
    d = {'id': self.id, 'name': self.varname(), 'type': self.typename()}
    format.template(Message.TRF_GET, d)
    format.template(Message.TRF_SET, d)

  TRF_GETID = '''
  virtual Id getId() const { return _getId(); }
  static inline Id _getId() { return Id(%(id0)du, %(id1)du); }
  '''

  TRF_GETVERSION = '''
  virtual int getVersion() const { return _getVersion(); }
  static inline int _getVersion() { return %(version)d; }
  '''

  TRF_GETNAME = '''
  virtual std::string getName() const { return _getName(); }
  static inline std::string _getName() { return "%(name)s"; }
  '''

  def dump_delete(self, format):
    format.write('delete %s;' % self.varname())

  def _declaration(self, format):
    pass

  TRF_SET = '''
  simdata::Ref<%(type)s> & set_%(id)s() {
  >m_has_%(id)s = 1;
  >if (%(name)s.isNull()) %(name)s = new %(type)s();
  >return %(name)s;
  }
  '''

  TRF_GET = '''
  simdata::Ref<%(type)s> const &  %(id)s() const {
  >return %(name)s;
  }
  '''


class Group(CompoundType):
  TYPENAME = 'group'
  TYPEDEF = 1

  def add(self, element):
    self.elements.append(element);

  def dump(self, format):
    self.classdef(format, base='simdata::Referenced')

  def __call__(self, id, opts=None, name=None):
    return GroupDeclaration(self.id, id, opts=opts, name=name)


class GroupDeclaration(CompoundType):
  DELETE = 0
  POINTER = 1

  TRF_DECL = '''
  simdata::Ref<%(type)s> m_%(id)s;
  '''

  TRF_SET = '''
  simdata::Ref<%(type)s> & set_%(id)s() {
  >m_has_%(id)s = 1;
  >if (m_%(id)s.isNull()) m_%(id)s = new %(type)s();
  >return m_%(id)s;
  }
  '''

  TRF_GET = '''
  simdata::Ref<%(type)s> const & %(id)s() const {
  >return m_%(id)s;
  }
  '''

  def __init__(self, typename, id, opts=None, name=None):
    CompoundType.__init__(self, id, opts=opts, name=name)
    self._typename = typename

  def typename(self): return self._typename

  def dump_private(self, format):
    d = {'id': self.id, 'type': self._typename}
    format.template(GroupDeclaration.TRF_DECL, d)

  def dump_public(self, format):
    d = {'id': self.id, 'type': self._typename}
    format.template(GroupDeclaration.TRF_GET, d)
    format.template(GroupDeclaration.TRF_SET, d)

  def dump_delete(self, format):
    format.write('delete %s;' % self.varname())


class Bitset(CompoundType):
  TYPENAME = 'bitset'
  ANONYMOUS = 'bitset'

  def add(self, element):
    assert element.TYPENAME == 'bool'
    assert len(self.elements) < 32
    self.elements.append(element);

  def default(self):
    value = 0
    mask = 1
    for bool in self.elements:
      if int(bool.default()):
        value |= mask
      mask <<= 1
    return '0x%08x' % value

  def dump_private(self, format):
    width = 32
    format.write('simdata::int%d %s;' % (width, self.varname()))

  def dump_get(self, format):
    mask = 1
    for bool in self.elements:
     format.write('bool %s() const {' % bool.id)
     format.indent()
     format.write('return (%s & 0x%08x) != 0;' % (self.varname(), mask))
     format.dedent()
     format.write('}')
     mask = mask << 1

  def dump_set(self, format):
    mask = 1
    id = self.id
    name = self.varname()
    for bool in self.elements:
      if not bool.isDeprecated():
        format.write('void set_%s(bool state) {' % bool.id)
        format.indent()
        format.write('m_has_%s = true;' % id);
        format.write('%s = state ? (%s | 0x%08x) : (%s & ~0x%08x);' % (name, name, mask, name, mask))
        format.dedent()
        format.write('}')
      mask = mask << 1

  def dump_public(self, format):
    self.dump_get(format)
    self.dump_set(format)

  def dump_save(self, format):
    format.write('writer.writer << %s;' % self.varname())

  def dump_load(self, format):
    format.write('reader.reader >> %s;' % self.varname())

  def dump_print(self, format):
    for bool in self.elements:
      format.write('os << indent << "%s: " << "\\n";' % bool.id)

class t_bool(SimpleType): TYPENAME = 'bool'
class t_float(SimpleType): TYPENAME = 'float'
class t_double(SimpleType): TYPENAME = 'double'
class t_int64(TrfType): TYPENAME = 'int64'
class t_int32(TrfType): TYPENAME = 'int32'
class t_int16(TrfType): TYPENAME = 'int16'
class t_int8(TrfType): TYPENAME = 'int8'
class t_string(SimpleType): TYPENAME = 'std::string'


class SimDataType(SimpleType):
  def typename(self):
    return SimpleType.typename(self)

class t_Matrix3(SimDataType): TYPENAME = 'simdata::Matrix3'
class t_Quat(SimDataType): TYPENAME = 'simdata::Quat'
class t_Vector3(SimDataType): TYPENAME = 'simdata::Vector3'
class t_Key(SimDataType): TYPENAME = 'simdata::Key'
class t_Date(SimDataType): TYPENAME = 'simdata::SimDate'
class t_Path(SimDataType): TYPENAME = 'simdata::Path'

#class t_TimeStamp(SimDataType): TYPENAME = 'TimeStamp'
#class t_Vector3f(SimDataType): TYPENAME = 'Vector3f'
#class t_Quatf(SimDataType): TYPENAME = 'Quatf'

TYPES = [Message, Bitset, Group]

TYPES.extend([
  t_float,
  t_double,
  t_int64,
  t_int32,
  t_int16,
  t_int8,
  t_bool,
  t_string,
])

TYPES.extend([
  t_Matrix3,
  t_Quat,
  t_Vector3,
  t_Key,
  t_Date,
  t_Path,
])

TYPEMAP = {}

def RegisterType(type):
  global TYPEMAP
  TYPEMAP[type.TYPENAME] = type

map(RegisterType, TYPES)

