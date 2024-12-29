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
TRC gen module: classes for generating tagged record source code
"""

import time
import re
import sys
import trc_util


def error(msg):
    print(msg, file=sys.stderr)
    sys.exit(1)


class Output:
    """
    Helper class for writing header and source files.  Tracks indentation
    level to properly format the output text.
    """

    HEADER = 0
    SOURCE = 1

    def __init__(self, mode, file=None, indent='\t'):
        """
        Create a new output instance.

          mode can be either HEADER or SOURCE.
          file is an open output file (or None to use stdout)
          indent is the string used for each indentation level
        """
        assert mode in (Output.HEADER, Output.SOURCE)
        self.__mode = mode
        if file is None:
            file = sys.stdout
        self.__file = file
        self.__indent = indent
        self.__indentation = ''
        self.__namespaces = []

    def indent(self):
        """Increase the indentation level"""
        self.__indentation += self.__indent

    def dedent(self):
        """Decrease the indentation level"""
        self.__indentation = self.__indentation[:-len(self.__indent)]

    def __call__(self, object):
        """Write an object or text to the output file"""
        if isinstance(object, str):
            if object != '__IGNORE__':
                print('%s%s' % (self.__indentation, object), file=self.__file)
        elif isinstance(object, tuple):
            print('%s%s' % (self.__indentation, str(object)), file=self.__file)
        else:
            if self.__mode == Output.HEADER:
                object.writeHeader(self)
            else:
                object.writeSource(self)

    def newline(self):
        """Write a blank line to the output file"""
        print('', file=self.__file)

    def Header(file=None):
        """Static factory for creating an Output instance for header files."""
        return Output(Output.HEADER, file)
    Header = staticmethod(Header)

    def Source(file=None):
        """Static factory for creating an Output instance for source files."""
        return Output(Output.SOURCE, file)
    Source = staticmethod(Source)


class Toplevel:
    """
    An abstract base class representing a declaration that can occur outside
    of Message definitions.
    """

    def writeHeader(output):
        raise NotImplementedError

    def writeSource(output):
        raise NotImplementedError


class Include(Toplevel):
    """A class representing an #include statement"""

    def __init__(self, path):
        self.path = path

    def writeHeader(self, output):
        output('#include %s' % self.path)

    def writeSource(self, output):
        pass


class Header(Toplevel):
    """A class representing a #header statement"""

    def __init__(self, include):
        self.include = include
        self.header = '__%s__' % re.sub(r'[^a-zA-Z0-9_]', '_', include[1:-1]).upper()
        self.timestamp = time.ctime()

    def writeBanner(self, output):
        output('// Generated on %s' % self.timestamp)
        output('// This is a generated file --- DO NOT EDIT!')

    def writeHeader(self, output):
        self.writeBanner(output)
        output.newline()
        output('#ifndef %s' % self.header)
        output('#define %s' % self.header)
        output.newline()

    def writeSource(self, output):
        self.writeBanner(output)
        output.newline()
        output('#include %s' % self.include)
        output('#include <csp/csplib/net/TaggedRecordRegistry.h>')
        output.newline()


class Using(Toplevel):
    """
    A class representing a using statement, for importing qualified names
    into the current namespace.
    """

    def __init__(self, name):
        self.name = name

    def writeHeader(self, output):
        output.newline()
        output('using %s;' % self.name)

    def writeSource(self, output):
        pass


class Footer(Toplevel):
    """A class representing the end of the input file."""

    def writeHeader(self, output):
        output.newline()
        output('#endif')

    def writeSource(self, output):
        pass


class Namespace(Toplevel):
    """
    A class representing a namespace declaration.  Somewhat confusingly,
    a namespace can (and generally does) contain other toplevel instances.
    """

    def __init__(self, id, contents):
        self.id = id
        self.contents = contents

    def writeContents(self, output, callback):
        output.newline()
        output('namespace %s {' % self.id)
        output.indent()
        for item in self.contents:
            callback(item)
        output.dedent()
        output.newline()
        output('}  // namespace %s' % self.id)
        output.newline()

    def writeHeader(self, output):
        self.writeContents(output, lambda x: x.writeHeader(output))

    def writeSource(self, output):
        self.writeContents(output, lambda x: x.writeSource(output))


class Element:
    """
    A base class representing any element that can be found inside a message
    definition.
    """

    # Element subclasses may override this value if they require generated
    # variable names.  For example, a bitset declaration is anonymous (ie,
    # no id is specified in the input file), so the BitSet class sets
    # ANONYMOUS = 'bitset' to automatically generate field names of the
    # form "bitsetN' where N = 0, 1, 2, etc.
    ANONYMOUS = 0

    def __init__(self, id):
        self.id = id
        self.options = {}
        self.anonymous_count = {}
        self.required = 0
        self.deprecated = 0
        self.default = None
        self.hasflag = None

    def getHasFlag(self):
        assert(self.hasflag is not None)
        return self.hasflag

    def setHasFlag(self, hasflag):
        self.hasflag = hasflag

    def isGroup(self):
        return 0

    def setOption(self, option, value):
        self.options[option] = value

    def getId(self, parent=None):
        if self.id:
            return self.id
        assert(parent is not None)
        assert(self.__class__.ANONYMOUS)
        anon = self.__class__.ANONYMOUS
        idx = parent.anonymous_count.get(anon, 0)
        parent.anonymous_count[anon] = idx + 1
        self.id = '%s%d' % (anon, idx)
        return self.id

    def getType(self):
        assert 0

    def signature(self):
        return ''

    def writeClear(self, output):
        pass

    def addToParent(self, parent):
        parent._addChild(self)

    def getName(self, parent=None):
        return 'm_%s' % self.getId()

    def writeGetter(self, output):
        output('inline %s const & %s() const { return %s; }' % (self.getType(), self.getId(), self.getName()))

    def writeSetter(self, output):
        if not self.deprecated:
            output('void set_%s(%s const &value) {' % (self.getId(), self.getType()))
            output.indent()
            output('%s = value;' % self.getName())
            output(self.getHasFlag().getSetString())
            output.dedent()
            output('}\n')

    def writeInit(self, output, tail=''):
        if self.default is None:
            output('%s()%s' % (self.getName(), tail))
        else:
            output('%s(%s)%s' % (self.getName(), self.default, tail))

    def writeSerializeWriter(self, output):
        if self.isGroup():
            output('%s->serialize(writer);' % self.getName())
        else:
            output('writer.writer << %s;' % self.getName())

    def writeSerializeReader(self, output):
        if self.isGroup():
            output('%s = new %s;' % (self.getName(), self.type.name))
            output('%s->serialize(reader);' % self.getName())
        else:
            output('reader.reader >> %s;' % self.getName())

    def write(self, output):
        output(str(self))

    def writeSource(self, output):
        pass

    def writeScopedSource(self, output, scope):
        pass


class Option(Element):
    """A class representing a message option specifier (e.g. @id=3;)"""

    def __init__(self, name, value):
        Element.__init__(self, None)
        self.name = name
        self.value = value

    def __str__(self):
        return '@%s = %s' % (self.name, self.value)

    def addToParent(self, parent):
        parent.setOption(self.name, self.value)


class Compound(Element):
    """A base class for elements that can contain other elements."""

    def __init__(self, id, children=None):
        Element.__init__(self, id)
        self.access = 'private'
        self.children = []
        self.localTypes = []
        if children:
            self.addChildren(children)

    def _addChild(self, child):
        self.children.append(child)

    def _addLocalType(self, localType):
        self.localTypes.append(localType)

    def addChild(self, child):
        child.addToParent(self)

    def addChildren(self, children):
        for child in children:
            child.addToParent(self)

    def signature(self):
        sig = []
        for child in self.children:
            sig.append(child.signature())
        return ';'.join(sig)

    def writeChildTags(self, output):
        output('enum {')
        output.indent()
        idx = 8
        for child in self.children[:-1]:
            output('TAG_%s = %d,' % (child.getId(self), idx))
            idx += 1
        output('TAG_%s = %d' % (self.children[-1].getId(self), idx))
        output.dedent()
        output('};\n')

    def writeChildHasFields(self, output):
        has_count = 0
        has_var = ''
        for child in self.children:
            idx = has_count % 32
            if (idx == 0):
                has_var = 'm_has%d' % (has_count / 32)
                output('uint32_t %s;' % has_var)
            child.setHasFlag(BitFlag(has_var, idx))
            has_count += 1
        output('')

    def writeChildFields(self, output):
        for child in self.children:
            output('%s %s;' % (child.getType(), child.getName()))

    def writeChildAccessors(self, output):
        for child in self.children:
            output('inline bool has_%s() const { return %s; }' % (child.getId(), child.getHasFlag().getTestString()))
        output.newline()
        for child in self.children:
            output('inline void clear_%s() {' % child.getId())
            output.indent()
            output(child.getHasFlag().getClearString())
            child.writeClear(output)
            output.dedent()
            output('}\n')
        for child in self.children:
            child.writeGetter(output)
        output.newline()
        for child in self.children:
            child.writeSetter(output)

    def writeAccessLabel(self, output, label):
        if label == self.access:
            return
        output.dedent()
        output('%s:\n' % label)
        output.indent()
        self.access = label

    def writeLocalTypes(self, output):
        for localType in self.localTypes:
            self.writeAccessLabel(output, 'public')
            localType.write(output)
            output.newline()

    def writeConstructor(self, output):
        if self.children:
            output('%s() :' % self.type.name)
            output.indent()
            for idx in range((len(self.children)+31)//32):
                output('m_has%d(0),' % idx)
            for child in self.children[:-1]:
                child.writeInit(output, ',')
            self.children[-1].writeInit(output)
            output.dedent()
            output('{ }\n')
        else:
            output('%s() { }\n' % self.type.name)

    def writeSerialize(self, output):
        output('void serialize(csp::TagWriter &writer) const;')
        output('void serialize(csp::TagReader &reader);')
        output.newline()

    def writeSerializeSource(self, output, scope=''):
        typename = scope + self.type.name
        output('void %s::serialize(csp::TagWriter &writer) const {' % typename)
        output.indent()
        output('writer.beginCompound();')
        has_required = 0
        has_optional = 0
        for child in self.children:
            if child.required:
                has_required = 1
                if has_optional:
                    print('REQUIRED elements cannot follow OPTIONAL element.')
                    print('%s::%s' % (typename, child.getId()))
                    sys.exit(1)
                output('assert(has_%s());' % child.getId())
                child.writeSerializeWriter(output)
            else:
                has_optional = 1
                output('if (has_%s()) {' % child.getId())
                output.indent()
                output('writer.writeTag(TAG_%s);' % child.getId())
                child.writeSerializeWriter(output)
                output.dedent()
                output('}')
        output('writer.endCompound();')
        output.dedent()
        output('}\n')
        output('void %s::serialize(csp::TagReader &reader) {' % typename)
        output.indent()
        output('reader.beginCompound();')
        if has_required:
            for child in self.children:
                if child.required:
                    child.writeSerializeReader(output)
                    output(child.getHasFlag().getSetString())
        has_tags = 0
        for child in self.children:
            if not child.required:
                has_tags = 1
                break
        if has_tags:
            output('for (int tag = reader.nextTag(); tag != 0; tag = reader.nextTag()) {')
            output.indent()
            output('switch (tag) {')
            output.indent()
            for child in self.children:
                if child.required:
                    continue
                output('case TAG_%s: {' % child.getId())
                output.indent()
                child.writeSerializeReader(output)
                output(child.getHasFlag().getSetString())
                output('break;')
                output.dedent()
                output('}')
            output('default: break;')
            output.dedent()
            output('}')
            output.dedent()
            output('}')
        output('reader.endCompound();')
        output.dedent()
        output('}\n')

    def writeDump(self, output, name=None):
        output('void dump(std::ostream &, int) const;')
        output('void dump(csp::DumpWriter &out, const char *name=0) const;')
        output.newline()

    def writeDumpSource(self, output, scope=''):
        typename = scope + self.type.name
        output('void %s::dump(std::ostream &, int) const {}' % typename)
        output('void %s::dump(csp::DumpWriter &out, const char *name) const {' % typename)
        output.indent()
        output('if (name) {')
        output.indent()
        output('out.line() << name << ": %s {";' % self.type.name)
        output.dedent()
        output('} else {')
        output.indent()
        output('out.line() << "%s {";' % self.type.name)
        output.dedent()
        output('}')
        output('out.indent();')
        for child in self.children:
            child.writeDump(output)
        output('out.dedent();')
        output('out.line() << "}";')
        output.dedent()
        output('}\n')

    def writeRef(self, output):
        output('typedef csp::Ref<%s> Ref;' % self.type.name)

    def writeBoilerPlate(self, output):
        pass

    def writeChildren(self, output):
        output('static int m_CustomId;')
        self.writeLocalTypes(output)
        if self.children:
            self.writeAccessLabel(output, 'private')
            self.writeChildTags(output)
            self.writeChildHasFields(output)
            self.writeChildFields(output)
            self.writeAccessLabel(output, 'public')
            self.writeChildAccessors(output)
        self.writeAccessLabel(output, 'public')
        self.writeConstructor(output)
        self.writeSerialize(output)
        self.writeAccessLabel(output, 'public')
        self.writeDump(output)
        self.writeRef(output)
        self.writeBoilerPlate(output)


class Field(Element):
    """A class representing a simple (ie not compound) field (e.g. int32 x;)."""

    def __init__(self, type, name, array=0, default=None):
        Element.__init__(self, name)
        self.type = type
        self.name = name
        self.array = array
        self.default = default

    def isGroup(self):
        return self.type.category in ('MESSAGE', 'GROUP')

    def writeClear(self, output):
        if self.array:
            output('%s.clear();' % self.getName())
        elif self.isGroup():
            output('%s = 0;' % self.getName())

    def __str__(self):
        array = ''
        if self.array:
            array = '[]'
        pre = ''
        if self.deprecated:
            pre = 'DEP '
        elif self.required:
            pre = 'REQ '
        default = ''
        if self.default is not None:
            default = ' = %s' % str(self.default)
        return '%s%s%s %s%s;' % (pre, self.type, array, self.name, default)

    def getType(self):
        type = self.type.name
        if self.isGroup():
            type = '%s::Ref' % type
        if self.array:
            return 'std::vector<%s>' % type
        return type

    def signature(self):
        sig = ''
        if self.array:
            sig += '@'
        if self.required:
            sig += '!'
        sig = '%s%s,%s' % (sig, self.type.name, self.name)
        return sig

    def writeSetter(self, output):
        if not self.deprecated:
            Element.writeSetter(self, output)
            if self.array:
                output('%s &set_%s() {' % (self.getType(), self.getId()))
                output.indent()
                output(self.getHasFlag().getSetString())
                output('return %s;' % self.getName())
                output.dedent()
                output('}\n')

    def writeSerializeWriter(self, output):
        if self.array:
            output('writer.writeLength(%s.size());' % self.getName())
            if self.isGroup():
                output('for (unsigned i=0; i < %s.size(); ++i) {' % self.getName())
                output.indent()
                output('%s[i]->serialize(writer);' % self.getName())
                output.dedent()
                output('}')
            else:
                output('for (unsigned i=0; i < %s.size(); ++i) {' % self.getName())
                output.indent()
                output('writer.writer << %s[i];' % self.getName())
                output.dedent()
                output('}')
        else:
            Element.writeSerializeWriter(self, output)

    def writeSerializeReader(self, output):
        if self.array:
            output('unsigned len = reader.readLength();')
            output('%s.resize(len);' % self.getName())
            if self.isGroup():
                output('for (unsigned i=0; i < len; ++i) {')
                output.indent()
                output('%s[i] = new %s;' % (self.getName(), self.type.name))
                output('%s[i]->serialize(reader);' % (self.getName()))
                output.dedent()
                output('}')
            else:
                output('for (unsigned i=0; i < len; ++i) {')
                output.indent()
                output('reader.reader >> %s[i];' % (self.getName()))
                output.dedent()
                output('}')
        else:
            Element.writeSerializeReader(self, output)

    def writeDump(self, output):
        output('if (has_%s()) {' % self.getId())
        output.indent()
        if self.array:
            output('out.line() << "%s: [";' % self.getId())
            output('out.indent();')
            if self.isGroup():
                output('for (unsigned i=0; i < %s.size(); ++i) {' % self.getName())
                output.indent()
                output('out.prefix() << i << ": ";')
                output('%s[i]->dump(out);' % self.getName())
                output.dedent()
                output('}')
            else:
                output('for (unsigned i=0; i < %s.size(); ++i) {' % self.getName())
                output.indent()
                output('out.prefix() << i << ": ";')
                output('out.line() << %s[i];' % self.getName())
                output.dedent()
                output('}')
            output('out.dedent();')
            output('out.line() << "]";')
        else:
            if self.isGroup():
                output('%s->dump(out, "%s");' % (self.getName(), self.getId()))
            else:
                output('out.line() << "%s: " << %s;' % (self.getId(), self.getName()))
        output.dedent()
        output('}')


class Group(Compound):
    """
    A class representing a group definition.  Groups are csp::Group subclasses
    defined inside of messages or other groups, and local to the enclosing scope.
    """

    def __init__(self, name, body, state=None):
        Compound.__init__(self, name, body)
        if state is not None:
            state.types[name] = 'GROUP'
        self.type = Type('GROUP', name)
        self.name = name
        self.body = body
        self.deprecated = 0

    def addToParent(self, parent):
        parent._addLocalType(self)

    def write(self, output):
        output('class %s : public csp::TaggedGroup {' % (self.type.name))
        output.indent()
        self.writeChildren(output)
        output.dedent()
        output('};')

    def writeScopedSource(self, output, scope=''):
        self.writeSerializeSource(output, scope)
        self.writeDumpSource(output, scope)
        scope += self.type.name + '::' + scope
        for element in self.localTypes:
            element.writeScopedSource(output, scope)

    def writeClear(self, output):
        output('%s = 0;' % self.getName())

    def __str__(self):
        dep = ''
        if self.deprecated:
            dep = 'DEP '
        return '%sgroup %s {\n%s\n}' % (dep, self.name, '\n'.join(map(str, self.body)))

    def getType(self):
        return 'csp::Ref<%s>' % self.type.name

    def signature(self):
        return '%s%%%s' % (self.type.name, Compound.signature(self))


class Bit(Element):
    """
    A class representing a single boolean field in a BitSet.  Bits cannot be
    declared outside of BitSets.
    """

    def __init__(self, mask, name, default=None, deprecated=0):
        Element.__init__(self, name)
        if default is not None:
            default = bool(int(default))
        self.mask = mask
        self.name = name
        self.default = default
        self.deprecated = deprecated

    def writeGetter(self, output, bitset):
        name = bitset.getName()
        output('inline bool %s() const { return (%s & 0x%02x) != 0; }' % (self.getId(), name, self.mask))

    def writeSetter(self, output, bitset):
        name = bitset.getName()
        output('void set_%s(bool value) {' % self.getId())
        output.indent()
        output('%s = (value ? (%s | 0x%02x) : (%s & ~0x%02x));' % (name, name, self.mask, name, self.mask))
        output(bitset.getHasFlag().getSetString())
        output.dedent()
        output('}\n')

    def writeDump(self, output):
        output('out.line() << "%s: " << %s();' % (self.getId(), self.getId()))

    def __str__(self):
        dep = ''
        if self.deprecated:
            dep = 'DEP '
        default = ''
        if self.default is not None:
            default = ' = %s' % str(self.default)
        return '%sbit %s%s' % (dep, self.name, default)


class Bitset(Compound):
    """
    An anonymous, compound element used to group boolean flags into a single field.
    A BitSet can only contain boolean fields.
    """

    # automatically generate field names of the form "bitsetN" for N = 0, 1, 2, etc.
    ANONYMOUS = 'bitset'

    def __init__(self, body):
        Compound.__init__(self, None, body)
        if len(body) > 32:
            error('too many boolean fields in bitset (limit 32)')
        bits = []
        default = 0
        bitshift = 1
        for field in body:
            if field.type.category != 'BOOL_TYPE':
                error('BitSet contains a non-boolean field')
            bit = Bit(bitshift, field.name, field.default, field.deprecated)
            bits.append(bit)
            if bit.default:
                default = default | bitshift
            bitshift = bitshift << 1
        self.bits = bits
        self.default = default
        self.deprecated = 0

    def writeGetter(self, output):
        for bit in self.bits:
            bit.writeGetter(output, self)

    def writeSetter(self, output):
        if not self.deprecated:
            for bit in self.bits:
                bit.writeSetter(output, self)

    def writeDump(self, output):
        for bit in self.bits:
            bit.writeDump(output)

    def __str__(self):
        dep = ''
        if self.deprecated:
            dep = 'DEP '
        return '%sbitset = %d {\n%s\n}' % (dep, self.default, '\n'.join(map(str, self.bits)))

    def getType(self):
        if len(self.children) <= 8:
            return 'uint8_t'
        if len(self.children) <= 16:
            return 'uint16_t'
        assert(len(self.children) <= 32)
        return 'uint32_t'

    def signature(self):
        return 'bits'


class Message(Toplevel, Compound):
    """
    A class representing a message definition that generates a subclass of
    csp::TaggedRecord.
    """

    def __init__(self, name, body, base=None, state=None):
        Compound.__init__(self, name, body)
        assert(state is not None)
        # TODO symbol lookup does not respect namespace
        if name in state.types:
            error('%s multiply defined' % name)
        self.type = Type('MESSAGE', name)
        self.name = name
        self.base = state.typedefs.get(base, base)
        state.types[name] = 'MESSAGE'
        state.messages[name] = self

    def writeBoilerPlate(self, output):
        Compound.writeBoilerPlate(self, output)
        version = int(self.options.get('@version', 0))
        id64 = trc_util.md5hash64('%s_%d' % (self.name, version))
        output('virtual Id getId() const { return _getId(); }')
        output('static const Id _Id = CSP_ULL(%d);' % id64)
        output('static inline Id _getId() { return _Id; }')
        output('virtual int getCustomId() const { return _getCustomId(); }')
        output('static inline int _getCustomId() { return m_CustomId; }')
        output('static inline void _setCustomId(int id) { m_CustomId = id; }')
        output('virtual int getVersion() const { return _getVersion(); }')
        output('static inline int _getVersion() { return %d; }' % version)
        output('virtual std::string getName() const { return _getName(); }')
        output('static inline std::string _getName() { return "%s"; }' % self.name)

    def writeHeader(self, output):
        output.newline()
        output.newline()
        if self.base:
            output('class %s : public %s {' % (self.name, self.base))
        else:
            output('class %s : public csp::TaggedRecord {' % (self.name))
        output.indent()
        self.writeChildren(output)
        output.dedent()
        output('};')

    def writeSource(self, output):
        id = int(self.options.get('@id', 0))
        output.newline()
        output('int %s::m_CustomId = %d;' % (self.name, id))
        # g++ allows preinitialized static const members to be redeclared outside
        # of the class, but msvc does not.  g++ is happy with either style.
        # output('const %s::Id %s::_Id;' % (self.name, self.name));
        output('namespace { csp::TaggedRecordFactory<%s> __%s_factory; }' % (self.name, self.name))
        output.newline()
        self.writeSerializeSource(output)
        self.writeDumpSource(output)
        scope = self.type.name + '::'
        for element in self.localTypes:
            element.writeScopedSource(output, scope)

    def __str__(self):
        if self.base:
            return 'message %s : %s {\n%s\n}' % (self.name, self.base, '\n'.join(map(str, self.body)))
        else:
            return 'message %s {\n%s\n}' % (self.name, '\n'.join(map(str, self.body)))

    def signature(self):
        siglist = ['%s%%%s' % (self.name, Compound.signature(self))]
        for type in self.localTypes:
            sig = type.signature()
            if sig:
                siglist.append(sig)
        return '&'.join(siglist)


class Enum(Element):
    """A class representing an enum declaration within a message or group."""

    def __init__(self, values):
        self.values = values
        index = 0
        for item in values:
            if item.value is None:
                item.value = index
            index = int(item.value) + 1

    def addToParent(self, parent):
        parent._addLocalType(self)

    def write(self, output):
        output('enum {')
        output.indent()
        for item in self.values[:-1]:
            output('%s = %d,' % (item.id, item.value))
        item = self.values[-1]
        output('%s = %d' % (item.id, item.value))
        output.dedent()
        output('};')
        output.newline()


# miscellaneous helper classes

class EnumValue:
    """A class representing a symbol in an enum declaration."""

    def __init__(self, id, value=None):
        self.id = id
        if value is not None:
            value = int(value)
        self.value = value


class Type:
    """A class representing a field type."""

    def __init__(self, category, name):
        self.category = category
        self.name = name

    def __str__(self):
        return '%s(%s)' % (self.name, self.category)


class BitFlag:
    """
    A helper class for generating code that indicates the presence or absence of
    a field in message or group.
    """

    def __init__(self, flagset, bit):
        self.flagset = flagset
        self.mask = (1 << bit)

    def getSetString(self):
        return '%s |= 0x%08x;' % (self.flagset, self.mask)

    def getClearString(self):
        return '%s &= ~0x%08x;' % (self.flagset, self.mask)

    def getTestString(self):
        return '(%s & 0x%08x) != 0' % (self.flagset, self.mask)
