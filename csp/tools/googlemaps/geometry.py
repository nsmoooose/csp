#!/usr/bin/python

# Combat Simulator Project
# Copyright (C) 2002-2005 The Combat Simulator Project
# http://csp.sourceforge.net
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

import sys
from csp.tools import googlemaps

class Rectangle:
    def __init__(self, left = 0, top = 0, right = 0, bottom = 0):
        self.left = left
        self.top = top
        self.right = right
        self.bottom = bottom
    
    def __str__(self):
        return "[left=%d, top=%d, right=%d, bottom=%d]" % (self.left, self.top, self.right, self.bottom)
    
    def width(self):
        return self.right - self.left
    
    def height(self):
        return self.bottom - self.top
    
    def extend(self, percent):
        centerX = (self.left + self.right) / 2.0
        centerY = (self.top + self.bottom) / 2.0
        self.left = int( centerX + (self.left - centerX) * percent )
        self.right = int( centerX + (self.right - centerX) * percent )
        self.top = int( centerY + (self.top - centerY) * percent )
        self.bottom = int( centerY + (self.bottom - centerY) * percent )
        return self

def getBoundingRectangle(pixels):
    left = sys.maxint
    top = sys.maxint
    right = -sys.maxint - 1
    bottom = -sys.maxint - 1
    
    for pixel in pixels:
        if left > pixel.x: left = pixel.x
        if right < pixel.x: right = pixel.x
        if top > pixel.y: top = pixel.y
        if bottom < pixel.y: bottom = pixel.y
    
    return Rectangle( left, top, right, bottom )
