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

from csp import csplib

# Coordinates are from left to right and from bottom to top

def setTerrainInfo(vertexSpacing, nbVertexTilesWidth, nbVertexTilesHeight, nbTextureTilesPerVertexTileWidth, nbTextureTilesPerVertexTileHeight):
    VertexTile.vertexSpacing = vertexSpacing
    VertexTile.nbWidth = nbVertexTilesWidth
    VertexTile.nbHeight = nbVertexTilesHeight
    VertexTile.sizeInMeters = VertexTile.sizeInPixels * vertexSpacing
    VertexTile.totalSizeInMetersWidth = VertexTile.sizeInMeters * VertexTile.nbWidth
    VertexTile.totalSizeInMetersHeight = VertexTile.sizeInMeters * VertexTile.nbHeight
    TextureTile.nbPerVertexTileWidth = nbTextureTilesPerVertexTileWidth
    TextureTile.nbPerVertexTileHeight = nbTextureTilesPerVertexTileHeight
    TextureTile.sizeInMetersWidth = VertexTile.sizeInMeters / nbTextureTilesPerVertexTileWidth
    TextureTile.sizeInMetersHeight = VertexTile.sizeInMeters / nbTextureTilesPerVertexTileHeight

class VertexTile:
    sizeInPixels = 256
    
    def __init__(self, column = 0, row = 0):
        self.column = column
        self.row = row

    def bottomLeft(self):
        return csplib.Vector3(-VertexTile.totalSizeInMetersWidth / 2.0 + self.column * VertexTile.sizeInMeters, -VertexTile.totalSizeInMetersHeight / 2.0 + self.row * VertexTile.sizeInMeters, 0)
    
    def corners(self):
        bottomLeft = self.bottomLeft()
        topLeft = bottomLeft + csplib.Vector3(0, VertexTile.sizeInMeters, 0)
        topRight = topLeft + csplib.Vector3(VertexTile.sizeInMeters, 0, 0)
        bottomRight = bottomLeft + csplib.Vector3(VertexTile.sizeInMeters, 0, 0)
        return topLeft, topRight, bottomRight, bottomLeft
    
    def textureTiles(self):
        for row in range(TextureTile.nbPerVertexTileHeight):
            for column in range(TextureTile.nbPerVertexTileWidth):
                yield TextureTile(self, column, row)

def allVertexTile():
    for row in range(VertexTile.nbHeight):
        for column in range(VertexTile.nbWidth):
            yield VertexTile(column, row)

def allVertexTileFromCenter():
    halfWidth = VertexTile.nbWidth // 2
    halfHeight = VertexTile.nbHeight // 2
    
    class Region:
        def __init__(self, columns, rows):
            self.columns = columns
            self.rows = rows
        
        def containsTile(self, tile):
            return (tile.column in self.columns) and (tile.row in self.rows)
        
        def allTiles(self):
            for row in self.rows:
                for column in self.columns:
                    yield VertexTile(column, row)
    
    def getRegion(size):
        minWidth = max( 0, halfWidth - size )
        maxWidth = min( halfWidth + size + 1, VertexTile.nbWidth )
        minHeight = max( 0, halfHeight - size )
        maxHeight = min( halfHeight + size + 1, VertexTile.nbHeight )
        return Region( range( minWidth, maxWidth ), range( minHeight, maxHeight ) )
    
    previousRegion = Region( [], [] )
    
    for size in range( max(halfWidth, halfHeight) + 1 ):
        currentRegion = getRegion(size)
        for tile in currentRegion.allTiles():
            if not previousRegion.containsTile(tile):
                yield tile
        previousRegion = currentRegion
    
class TextureTile:
    sizeInPixels = 256
    
    def __init__(self, vertexTile = None, column = 0, row = 0):
        self.vertexTile = vertexTile
        if self.vertexTile is None: self.vertexTile = VertexTile()
        self.column = column
        self.row = row

    def bottomLeft(self):
        return self.vertexTile.bottomLeft() + csplib.Vector3(self.column * TextureTile.sizeInMetersWidth, self.row * TextureTile.sizeInMetersHeight, 0)
    
    def corners(self):
        bottomLeft = self.bottomLeft()
        topLeft = bottomLeft + csplib.Vector3(0, TextureTile.sizeInMetersHeight, 0)
        topRight = topLeft + csplib.Vector3(TextureTile.sizeInMetersWidth, 0, 0)
        bottomRight = bottomLeft + csplib.Vector3(TextureTile.sizeInMetersWidth, 0, 0)
        return topLeft, topRight, bottomRight, bottomLeft
    
    def fileNameSuffix(self):
        return "Tex.%d-%d.%d-%d" % (self.vertexTile.column, self.vertexTile.row, self.column, self.row)

def allTextureTile():
    for vertexTile in allVertexTile():
        for textureTile in vertexTile.textureTiles():
            yield textureTile
