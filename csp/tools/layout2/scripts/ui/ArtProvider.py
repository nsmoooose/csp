# Combat Simulator Project
# Copyright (C) 2002-2009 The Combat Simulator Project
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

import os
import wx

class MissingImageArtProvider(wx.ArtProvider):
    def CreateBitmap(self, id, client, size):
        if id == wx.ART_MISSING_IMAGE:
            return wx.NullBitmap

        return wx.ArtProvider.GetBitmap(wx.ART_MISSING_IMAGE, client, size)

class SearchInDirectoriesArtProvider(wx.ArtProvider):
    def __init__(self, baseDirectories, *args, **kwargs):
        wx.ArtProvider.__init__(self, *args, **kwargs)

        self.baseDirectories = baseDirectories

    def CreateBitmap(self, id, client, size):
        if ( size == wx.DefaultSize ):
            sizeDirectories = [""]
        else:
            sizeDirectories = ["%dx%d" % (size.GetWidth(), size.GetHeight()), ""]

        for sizeDirectory in sizeDirectories:
            for baseDirectory in self.baseDirectories:
                for ext in ('.png', '.gif'):
                    fullPath = os.path.join(baseDirectory, sizeDirectory, id + ext)
                    if os.path.isfile(fullPath):
                        return wx.Bitmap(fullPath)

        return wx.NullBitmap
