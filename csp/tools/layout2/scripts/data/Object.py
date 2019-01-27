# Copyright 2004-2005 Mark Rose <mkrose@users.sourceforge.net>
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
A subset of the csplib XML parsing functionality.  Converts a domtree
representation of an Object XML hierarchy into a real Object instance.
Only works for objects that do not contain Link tags.
"""

# TODO implement other tags (e.g. Table, Enum, Key, etc.).

from csp import csplib
from csp import cspsim
from . import domtree
import sys


InterfaceRegistry = csplib.InterfaceRegistry.getInterfaceRegistry()


class Tag_Object:
    def __init__(self, node, parent=None):
        assert(node.type == domtree.Node.ELEMENT_NODE)
        assert(node.name == 'Object')
        self._name = node.attr('name')
        self._klass = str(node.attr('class'))
        assert(InterfaceRegistry.hasInterface(self._klass)), self._klass
        self._interface = InterfaceRegistry.getInterface(self._klass)
        self._object = self._interface.createObject()
        assert(self._object is not None)
        for child in node.children:
            Realize(child, self)
        if parent is not None:
            parent.set(self._name, self._object)

    def set(self, name, value):
        name = str(name)  # .encode('ascii')
        self._interface.set(self._object, name, value)

    def push_back(self, name, value):
        name = str(name)
        self._interface.push_back(self._object, name, value)


class Tag_List:
    def __init__(self, node, parent):
        self._name = node.attr('name')
        self._parent = parent
        [Realize(child, self) for child in node.children]

    def set(self, name, value):
        assert not name
        self._parent.push_back(self._name, value)


class Tag_Float:
    def __init__(self, node, parent):
        name = node.attr('name')
        parent.set(name, float(node.text))


class Tag_Int:
    def __init__(self, node, parent):
        name = node.attr('name')
        parent.set(name, int(node.text))


class Tag_Bool:
    def __init__(self, node, parent):
        name = node.attr('name')
        bool = node.text.strip().upper() == 'TRUE'
        parent.set(name, bool)


class Tag_Vector2:
    def __init__(self, node, parent):
        name = node.attr('name')
        vec = csplib.Vector2(*list(map(float, node.text.split())))
        parent.set(name, vec)


class Tag_Vector3:
    def __init__(self, node, parent):
        name = node.attr('name')
        vec = csplib.Vector3(*list(map(float, node.text.split())))
        parent.set(name, vec)


class Tag_Path:
    def __init__(self, node, parent):
        name = node.attr('name')
        path = csplib.LinkBase()
        path.setPath(node.text.encode('ascii'))
        parent.set(name, path)


class Tag_External:
    def __init__(self, node, parent):
        name = node.attr('name')
        external = csplib.External()
        external.setSource(node.text.encode('ascii'))
        parent.set(name, external)


class Tag_String:
    def __init__(self, node, parent):
        name = node.attr('name')
        parent.set(name, str(node.text))


def Realize(node, parent=None):
    tag = node.name
    if tag.startswith('#'):
        return
    wrapper = globals().get('Tag_' + tag)
    if wrapper is None:
        print('No wrapper for %s' % tag)
        sys.exit(1)
    wrapper(node, parent)


def MakeObject(node, postCreate=1):
    object = Tag_Object(node)._object
    if postCreate and object is not None:
        object._postCreate()
    return object


if __name__ == '__main__':
    # tree = domtree.ParseFile('../../data/xml/theater/balkan/forest.xml')
    tree = domtree.ParseFile('../../data/xml/theater/balkan/tower1/model.xml')
    o = MakeObject(tree.children[0])
    o._postCreate()
    # the constructed object behaves like a csplib::Object pointer.  to do
    # anything useful with it, we need to pass it to C++ code that understands
    # the real class type, or swig the appropriate class.
