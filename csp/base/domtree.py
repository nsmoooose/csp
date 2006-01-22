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
Very simple XML parsing using minidom and lots of Python magic.
"""

from xml.dom.minidom import parse, parseString


class DomTree:
  """
  Dom node wrapper providing access to child nodes via instance
  attributes and iterators.
  """

  def __init__(self, dom):
    self.name = dom.nodeName
    self.type = dom.nodeType
    self.node = dom
    self.text = ''
    self.children = []
    self._dict = {}
    for node in dom.childNodes:
      if node.nodeType == node.TEXT_NODE:
        self.text += node.data
      else:
        tree = DomTree(node)
        self.children.append(tree)
        self._dict[tree.name] = tree
  def __getattr__(self, key):
    return self.__dict__['_dict'].get(key, None)
  def attr(self, name, default=None):
    return self.node.getAttribute(name)
  def attrs(self):
    return self.node._attrs.keys()
  def hasattr(self, name):
    return self.node.hasAttribute(name)
  def __len__(self): return len(self.children)
  def __str__(self): return self.__repr__()
  def __repr__(self):
    if self.text.strip(): return self.text
    return 'node %s' % self.name
  def keys(self):
    return self._dict.keys()
  def get(self, name, default):
    return self.child
  def __iter__(self):
    for child in self.children:
      yield child
    raise StopIteration


def ParseFile(file):
  return DomTree(parse(file))


def ParseString(text):
  return DomTree(parseString(text))

