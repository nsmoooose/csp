# Combat Simulator Project
# Copyright (C) 2002-2005 Mark Rose <mkrose@users.sf.net>
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
import os.path
import re
import sys
import time
import types
from csp import csplib
from csp.base.signals import Signal
from gzip import GzipFile
from traceback import print_exception
from xml.sax import parseString, ContentHandler, ErrorHandler, make_parser


g_InterfaceRegistry = csplib.InterfaceRegistry.getInterfaceRegistry()


class ParserEvent:
    """This is the base event class for a parser message.
    This object is emitted with a signal to
    notify listeners about informational, warnings or error
    messages."""

    def __init__(self, message):
        self.message = message

    def GetMessage(self):
        """The actual text message."""
        return self.message


class XMLSyntax(Exception):
    def __init__(self, *args):
        self.args = args


class CompilerErrorHandler(ErrorHandler):
    def __init__(self, path, id):
        self._path = path
        self._id = id

    def fatalError(self, error):
        print("FATAL ERROR parsing XML input.")
        locator = error._locator
        if locator is not None:
            line = error._locator.getLineNumber()
            column = error._locator.getColumnNumber()
            place = "[%d:%d]" % (line, column)
        else:
            place = ""
        print("File %s%s - %s" % (self._path, place, ", ".join(error.args)))
        sys.exit(1)


def path_to_id(prefix, path):
    if path.endswith('.gz'):
        path = path[:-3]
    if path.endswith('.xml'):
        path = path[:-4]
    parts = []
    while 1:
        (path, tail) = os.path.split(path)
        if tail != "":
            parts.insert(0, tail)
        else:
            break
    return prefix + ":" + '.'.join(parts)


def id_to_path(id):
    parts = id.split(':')
    if len(parts) == 2:
        id = parts[1]
    return os.path.join(*id.split('.'))


# convert to absolute path id
def adjust_path(base, id):
    if ':' in id:
        return id
    if not id.startswith("."):
        return base + "." + id
    parts = base.split(':')
    if len(parts) == 2:
        prefix, base = parts
    else:
        prefix, base = "", base
    return prefix + ":" + id[1:]


class ElementHandler(ContentHandler):
    def __init__(self, id, base, name, attrs):
        self._handler = None
        self._handler_tag = None
        self._element = None
        self._id = id
        self._base = base
        self._name = name
        self._attrs = attrs
        self._paths = []
        self._externals = []
        self._c = ""

    def getElement(self):
        return self._element

    def characters(self, c):
        if self._handler is not None:
            self._handler.characters(c)
        else:
            self.cdata(c)

    def cdata(self, c):
        self._c = self._c + c

    def endChild(self):
        self._element = self._handler.getElement()

    def handleChild(self, name, attrs):
        return None

    def end(self):
        pass

    def endElement(self, name):
        if self._handler is not None:
            done = self._handler.endElement(name)
            if done:
                self.endChild()
                self._externals.extend(self._handler._externals)
                self._paths.extend(self._handler._paths)
                self._handler = None
                self._handler_tag = None
            return 0
        self.end()
        return 1

    def validateChild(self, name, attrs):
        return 1

    def startElement(self, name, attrs):
        if self._handler is None:
            if not self.validateChild(name, attrs):
                raise XMLSyntax("Invalid child element <%s> in <%s>" % (name, self._name))
            handler = self.handleChild(name, attrs)
            if handler is None:
                handler_name = name+"Handler"
                try:
                    handler = eval(handler_name)
                except Exception:
                    msg = "Unknown child element <%s>" % name
                    raise XMLSyntax(msg)
            self._handler = handler(self._id, self._base, name, attrs)
            self._handler_tag = name
            self._attrs = attrs
        else:
            self._handler.startElement(name, attrs)

    def assign(self, interface, obj, name):
        interface.set(obj, name, self.getElement())

    def getObjectAttribute(self, obj, name, type_):
        attr = getattr(obj, name, None)
        if attr is None:
            if isinstance(obj, csplib.Object):
                if isinstance(type_, type):
                    attr = type_()
                else:
                    attr = type_
                setattr(obj, name, attr)
            else:
                msg = "Unknown attribute '%s' in object %s" % (name, str(obj))
                raise XMLSyntax(msg)
        return attr


class SimpleHandler(ElementHandler):
    def __init__(self, id, base, name, attrs):
        ElementHandler.__init__(self, id, base, name, attrs)

    def validateChild(self, name, attrs):
        return 0


class ListHandler(SimpleHandler):
    def __init__(self, id, base, name, attrs):
        SimpleHandler.__init__(self, id, base, name, attrs)
        self._type = attrs.get("type", None)
        self._element = []

    def endChild(self):
        self._element.append(self._handler.getElement())

    def validateChild(self, name, attrs):
        if self._type is not None:
            return 0
        return name in ('Enum', 'Path', 'Int', 'Bool', 'Number', 'Float',
                        'String', 'Date', 'Vector2', 'Vector3', 'Matrix', 'External', 'Key',
                        'Object', 'Quat', 'LLA', "UTM", "ECEF")

    def end(self):
        if self._type is not None:
            list = self._c.split()
            if self._type == "int":
                f = int
            elif self._type == "float":
                f = float
            elif self._type == "real":
                def spread(x):
                    y = csplib.Real()
                    y.parseXML(x)
                    return y
                f = spread
            elif self._type == "key":
                f = csplib.Key
            else:
                msg = "Unknown LIST type (%s)" % self._type
                raise XMLSyntax(msg)
            self._element = map(f, map(str, list))

    def assign(self, interface, obj, name):
        interface.clear(obj, name)
        for item in self._element:
            interface.push_back(obj, name, item)


class FloatListHandler(ListHandler):
    def __init__(self, id, base, name, attrs):
        ListHandler.__init__(self, id, base, name, attrs)
        self._type = "float"


class IntHandler(SimpleHandler):
    def __init__(self, id, base, name, attrs):
        SimpleHandler.__init__(self, id, base, name, attrs)

    def end(self):
        self._element = int(self._c, 0)


class BoolHandler(SimpleHandler):
    def __init__(self, id, base, name, attrs):
        SimpleHandler.__init__(self, id, base, name, attrs)

    def end(self):
        value = self._c.encode('ascii').strip()
        if value.upper() == "TRUE":
            self._element = 1
        elif value.upper() == "FALSE":
            self._element = 0
        else:
            self._element = int(value)


class FloatHandler(SimpleHandler):
    def __init__(self, id, base, name, attrs):
        SimpleHandler.__init__(self, id, base, name, attrs)

    def end(self):
        self._element = float(self._c)


class RealHandler(SimpleHandler):
    def __init__(self, id, base, name, attrs):
        SimpleHandler.__init__(self, id, base, name, attrs)

    def end(self):
        self._element = csplib.Real()
        self._element.parseXML(self._c.encode('ascii'))


class ECEFHandler(SimpleHandler):
    def __init__(self, id, base, name, attrs):
        SimpleHandler.__init__(self, id, base, name, attrs)

    def end(self):
        self._element = csplib.ECEF()
        self._element.parseXML(self._c.encode('ascii'))


class LLAHandler(SimpleHandler):
    def __init__(self, id, base, name, attrs):
        SimpleHandler.__init__(self, id, base, name, attrs)

    def end(self):
        self._element = csplib.LLA()
        self._element.parseXML(self._c.encode('ascii'))


class UTMHandler(SimpleHandler):
    def __init__(self, id, base, name, attrs):
        SimpleHandler.__init__(self, id, base, name, attrs)

    def end(self):
        self._element = csplib.UTM()
        self._element.parseXML(self._c.encode('ascii'))


class Vector2Handler(SimpleHandler):
    def __init__(self, id, base, name, attrs):
        SimpleHandler.__init__(self, id, base, name, attrs)

    def end(self):
        self._element = csplib.Vector2()
        self._element.parseXML(self._c.encode('ascii'))


class Vector3Handler(SimpleHandler):
    def __init__(self, id, base, name, attrs):
        SimpleHandler.__init__(self, id, base, name, attrs)

    def end(self):
        self._element = csplib.Vector3()
        self._element.parseXML(self._c.encode('ascii'))


class MatrixHandler(SimpleHandler):
    def __init__(self, id, base, name, attrs):
        SimpleHandler.__init__(self, id, base, name, attrs)

    def end(self):
        self._element = csplib.Matrix3()
        self._element.parseXML(self._c.encode('ascii'))


class QuatHandler(SimpleHandler):
    def __init__(self, id, base, name, attrs):
        SimpleHandler.__init__(self, id, base, name, attrs)

    def end(self):
        self._element = csplib.Quat()
        self._element.parseXML(self._c.encode('ascii'))


class DateHandler(SimpleHandler):
    def __init__(self, id, base, name, attrs):
        SimpleHandler.__init__(self, id, base, name, attrs)

    def end(self):
        self._element = csplib.SimDate()
        self._element.parseXML(self._c.encode('ascii'))


class StringHandler(SimpleHandler):
    def __init__(self, id, base, name, attrs):
        SimpleHandler.__init__(self, id, base, name, attrs)

    def end(self):
        self._element = self._c.encode('ascii')

    def assign(self, interface, obj, name):
        interface.set(obj, name, str(self._element))


class EnumHandler(SimpleHandler):
    def __init__(self, id, base, name, attrs):
        SimpleHandler.__init__(self, id, base, name, attrs)

    def end(self):
        self._element = self._c

    def assign(self, interface, object, name):
        value = self._element.encode('ascii')
        interface.set_enum(object, name, value)


class PathHandler(SimpleHandler):
    def __init__(self, id, base, name, attrs):
        SimpleHandler.__init__(self, id, base, name, attrs)

    def getElement(self):
        p = csplib.LinkBase()
        p.setPath(self._element.encode('ascii'))
        return p

    def end(self):
        source = self._c.strip()
        source = adjust_path(self._base, source)
        self._element = source
        self._paths.append(source)

    def assign(self, interface, obj, name):
        interface.set(obj, name, self.getElement())


class ExternalHandler(SimpleHandler):
    def __init__(self, id, base, name, attrs):
        SimpleHandler.__init__(self, id, base, name, attrs)

    def end(self):
        source = self._c.strip()
        source = os.path.join(*source.split("/"))
        self._element = source
        self._externals.append(source)

    def assign(self, interface, obj, name):
        ext = csplib.External()
        ext.setSource(self._element.encode('ascii'))
        interface.set(obj, name, ext)


class KeyHandler(SimpleHandler):
    def __init__(self, id, base, name, attrs):
        SimpleHandler.__init__(self, id, base, name, attrs)

    def end(self):
        id = self._c.encode('ascii')
        self._element = csplib.Key(id)


# Generic LUT handler, specialized below for Table1, Table2, and Table3
class _LUTHandler(SimpleHandler):
    handlers = {
        "Values": FloatListHandler,
    }

    members = list(handlers.keys())
    required_members = ["Values"]

    def __init__(self, dim, id, base, name, attrs):
        SimpleHandler.__init__(self, id, base, name, attrs)
        self._method = attrs.get("method", "linear")
        self._required_members = _LUTHandler.required_members[:]
        for i in range(dim):
            self._required_members.append("Breaks%d" % i)
        self._dim = dim
        self._keys = {}

    def handleChild(self, name, attrs):
        if name.startswith("Breaks"):
            return FloatListHandler
        return _LUTHandler.handlers[name]

    def validateChild(self, name, attrs):
        if name.startswith("Breaks"):
            n = int(name[6:])
            return n >= 0 and n < self._dim
        return name in _LUTHandler.members

    def endChild(self):
        child = self._handler.getElement()
        attrs = self._handler._attrs
        member = self._handler_tag
        self._keys[member] = (child, attrs)

    def assign(self, interface, object, name):
        missing = filter(lambda x, f=self._keys.has_key: not f(x), self._required_members)
        assert len(missing)==0, "LUTHandler required tag(s) missing:\n  %s" % str(missing)
        table = getattr(csplib, "Table%d" % self._dim)()
        tags = self._keys
        breaks = []
        spacing = []
        total = 1
        for i in range(self._dim):
            breakpoints, attrs = tags["Breaks%d" % i]
            if "spacing" not in attrs:
                msg = "LUTHander <Breaks%d> tag missing required attribute 'spacing'" % i
                raise XMLSyntax(msg)
            dx = float(attrs["spacing"])
            if "scale" in attrs:
                scale = float(attrs["scale"])
                breakpoints = [x * scale for x in breakpoints]
                dx *= scale
            breaks.append(breakpoints)
            n = 1 + int((max(breaks[i]) - min(breaks[i])) / dx)
            spacing.append(n)
            total *= len(breakpoints)
        values, attrs = tags["Values"]
        if "scale" in attrs:
            scale = float(attrs["scale"])
            values = [x * scale for x in values]
        if len(values) != total:
            msg = "LUTHander value count does not match breakpoint count (%d vs %d)" % (len(values), total)
            raise XMLSyntax(msg)
        table.load(values, breaks)
        try:
            method = getattr(table, self._method.upper())
        except Exception:
            msg = "LUTHander: unknown interpolation method '%s'" % self._method
            raise XMLSyntax(msg)
        table.interpolate(spacing, method)
        interface.set(object, name, table)


class Table1Handler(_LUTHandler):
    def __init__(self, id, base, name, attrs):
        _LUTHandler.__init__(self, 1, id, base, name, attrs)


class Table2Handler(_LUTHandler):
    def __init__(self, id, base, name, attrs):
        _LUTHandler.__init__(self, 2, id, base, name, attrs)


class Table3Handler(_LUTHandler):
    def __init__(self, id, base, name, attrs):
        _LUTHandler.__init__(self, 3, id, base, name, attrs)


class FileHandler(ElementHandler):
    def __init__(self, prefix="", path=None, id=None):
        if path is not None:
            id = path_to_id(prefix, path)
        self._id = id
        id = id.split('.')
        base = '.'.join(id[:-1])
        self._name = id[-1]
        ElementHandler.__init__(self, self._id, base, None, None)

    def validateChild(self, name, attrs):
        return name in ["Object"]


class ObjectHandler(ElementHandler):
    def __init__(self, id, base, name=None, attrs=None):
        ElementHandler.__init__(self, id, base, name, attrs)
        self._class = attrs.get("class", None)
        self._interface = None
        self._warningSignal = Signal()

        if self._class is None:
            raise XMLSyntax("Object missing class attribute")

        self._class = str(self._class)
        if g_InterfaceRegistry.hasInterface(self._class):
            self._interface = g_InterfaceRegistry.getInterface(self._class)
        else:
            msg = "Class '%s' not available" % self._class
            interface_names = [x.split(":")[0] for x in g_InterfaceRegistry.getInterfaceNames()]
            interface_names.sort()
            raise NameError(msg)
        try:
            obj = self._interface.createObject()
        except Exception:
            msg = "ERROR Creating class %s" % self._class
            raise Exception(msg)
        self._object = obj
        self._assigned = {}
        self._objectClass = self._object.__class__
        self._all_variables = self._interface.getVariableNames()
        self._req_variables = self._interface.getRequiredNames()

        if "static" in attrs:
            self._warningSignal.Emit(ParserEvent("'static' attribute of <Object> is deprecated."))

    def endChild(self):
        name = self._attrs["name"].encode('ascii')
        self._assigned[name] = 1
        self._handler.assign(self._interface, self._object, name)
        self.checkName(name)

    def checkName(self, name):
        if name in self._all_variables:
            return 0
        self._warningSignal.Emit(ParserEvent("Setting unknown member '%s' of class '%s'!" % (name, self._class)))
        return 1

    def end(self):
        self._element = self._object
        self.checkAssigned()
        self._element.parseXML(self._c.encode('ascii'))
        self._element.convertXML()

    def checkAssigned(self):
        isAssigned = self._assigned.has_key
        isUnassigned = lambda x, _a=isAssigned: not _a(x)
        unassigned = filter(isUnassigned, self._req_variables)
        if len(unassigned) > 0:
            msg = "'%s' Object in '%s' has unassigned member(s):\n" % (self._class, self._id)
            for member in unassigned:
                msg += "       : %s\n" % member
            self._warningSignal.Emit(ParserEvent(msg))

    def handleChild(self, name, attrs):
        _handler = None
        return _handler


class ObjectXMLArchive:
    def __init__(self, prefix, path):
        self._basepath = path
        self._objects = {}
        self._paths = {}
        self._externals = {}
        self._prefix = prefix

    def loadPath(self, path, id):
        if not os.path.exists(path):
            path = path + '.xml'
            if not os.path.exists(path):
                path = path + '.gz'
                if not os.path.exists(path):
                    raise IOError("Unable to find data for %s" % id)
        if path.endswith('.gz'):
            if GzipFile is None:
                raise IOError("Unable to open '%s', since zlib appears to be missing or improperly installed (%s)." % (path, GzipError))
            f = GzipFile(path, "rb")
        else:
            f = open(path, "rt")
        data = f.read()
        fh = FileHandler(id=id)
        ceh = CompilerErrorHandler(path, id)
        try:
            parseString(data, fh, ceh)
        except RuntimeError:
            raise
        except Exception as e:
            errorMessage = ''
            locator = fh._locator
            column = locator.getColumnNumber()
            line = locator.getLineNumber()
            public = locator.getPublicId()
            system = locator.getSystemId()
            if column is None:
                errorMessage += "In source file \"%s\" [line %d]" % (path, line)
            else:
                errorMessage += "In source file \"%s\" [line %d, col %d]" % (path, line, column)
            if public is not None or system is not None:
                errorMessage += " - %s, %s" % (public, system)

            errorMessage += "\nException: %s" % e
            raise Exception(errorMessage)
        self.addReferences(id, self._paths, fh._paths)
        self.addReferences(id, self._externals, fh._externals)
        return fh.getElement()

    def addReferences(self, id, dest, src):
        for ref in src:
            base = dest.get(ref, [])
            base.append(id)
            dest[ref] = base

    def getPaths(self):
        return self._paths

    def getExternals(self):
        return self._externals

    def loadAll(self, path=""):
        dirpath = os.path.join(self._basepath, path)
        filenames = os.listdir(dirpath)
        filenames.sort()
        for filename in filenames:
            filepath = os.path.normpath(os.path.join(path, filename))
            fullpath = os.path.normpath(os.path.join(self._basepath, filepath))
            if filename.endswith('.xml') or filename.endswith('.xml.gz'):
                id = path_to_id(self._prefix, filepath)
                self._objects[id] = self.loadPath(fullpath, id)
            elif os.path.isdir(fullpath):
                self.loadAll(filepath)
        return self._objects
