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

import os
import threading
import copy
import math
import urllib
import wx
import xml.dom.minidom
from csp import csplib
from csp import cspsim
from csp.tools import googlemaps
import geometry
import tiles


class GoogleMapURLopener(urllib.FancyURLopener):
    def http_error_default(self, url, fp, errcode, errmsg, headers):
        return urllib.URLopener.http_error_default(self, url, fp, errcode, errmsg, headers)

urllib._urlopener = GoogleMapURLopener()
urllib.nbGoogleMapDownload = 0

class UrlNotFound(IOError):
    pass

def urlretrieve(url, filename, progress = None):
    try:
        try:
            urllib.nbGoogleMapDownload += 1
            print "Downloading bitmap", urllib.nbGoogleMapDownload, url
            urllib.urlretrieve(url, filename)
        except IOError, e:
            errcode = e.args[1]
            if errcode == 404:
                emptyFile = open(filename, "w")
                emptyFile.close()
                raise UrlNotFound(e.args)
            raise
    finally:
        if not progress is None:
            pass #progress.wait(2)

def fileIsEmpty(filename):
    return not os.path.getsize( filename )


class InfoPanel(wx.Panel):
    def __init__(self, parent, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)
        
        self.mainWindow = parent.mainWindow
        
        sizerPanel = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(sizerPanel)
        
        # Terrain selector
        self.buttonTerrain = wx.Button(self, label = "Choose a terrain")
        self.buttonTerrain.Bind(wx.EVT_BUTTON, self.onSelectTerrain)
        sizerPanel.Add(self.buttonTerrain)
        
        # Terrain informations
        sizerInfo = wx.FlexGridSizer(cols = 2, vgap = 5, hgap = 5)
        sizerPanel.AddSpacer(10)
        sizerPanel.Add(sizerInfo)
        
        self.staticTerrain = self.addInfo("Terrain", sizerInfo)
        self.staticCenter = self.addInfo("Center", sizerInfo)
        self.staticVertexSpacing = self.addInfo("Vertex spacing", sizerInfo)
        self.staticLatticeWidth = self.addInfo("Lattice width", sizerInfo)
        self.staticLatticeHeight = self.addInfo("Lattice height", sizerInfo)
        self.staticLatticeTilesWidth = self.addInfo("Lattice tiles width", sizerInfo)
        self.staticLatticeTilesHeight = self.addInfo("Lattice tiles height", sizerInfo)
        
        # Import parameters
        staticCaption = wx.StaticText(self, label = "Oversampling :")
        sizerInfo.Add(staticCaption, flag = wx.ALIGN_CENTRE_VERTICAL)

        self.SpinCtrlOversampling = wx.SpinCtrl(self, min = 1, max = 4, initial = 2)
        sizerInfo.Add(self.SpinCtrlOversampling)
        
        # Starting button
        self.buttonStart = wx.Button(self, label = "Start")
        self.buttonStart.Disable()
        self.buttonStart.Bind(wx.EVT_BUTTON, self.onSart)
        sizerPanel.AddSpacer(10)
        sizerPanel.Add(self.buttonStart)
        
        # Cancel button
        self.buttonCancel = wx.Button(self, label = "Pause")
        self.buttonCancel.Disable()
        self.buttonCancel.Bind(wx.EVT_BUTTON, self.onCancel)
        sizerPanel.AddSpacer(10)
        sizerPanel.Add(self.buttonCancel)
    
    def addInfo(self, caption, sizer):
        staticCaption = wx.StaticText(self, label = caption + " :")
        sizer.Add(staticCaption)
        staticValue = wx.StaticText(self)
        sizer.Add(staticValue)
        return staticValue
    
    def updateInfo(self):
        self.staticTerrain.SetLabel(self.mainWindow.terrainName)
        self.staticCenter.SetLabel( str(self.mainWindow.center) )
        self.staticVertexSpacing.SetLabel( str(self.mainWindow.vertexSpacing) )
        self.staticLatticeWidth.SetLabel( str(self.mainWindow.latticeWidth) )
        self.staticLatticeHeight.SetLabel( str(self.mainWindow.latticeHeight) )
        self.staticLatticeTilesWidth.SetLabel( str(self.mainWindow.latticeTilesWidth) )
        self.staticLatticeTilesHeight.SetLabel( str(self.mainWindow.latticeTilesHeight) )
    
    def onSelectTerrain(self, event):
        dlg = wx.FileDialog(self, "Choose a terrain file", self.mainWindow.xmlTerrainPath, "", "*.*", wx.OPEN | wx.FILE_MUST_EXIST)
        if dlg.ShowModal() == wx.ID_OK:
            self.mainWindow.terrainFile = dlg.GetFilename()
            self.mainWindow.startReadTerrainInfo()
        dlg.Destroy()
    
    def onSart(self, event):
        msgBox = wx.MessageDialog( self, \
            "Warning, this tool must be used for experimental purpose only.\n" + \
            "It will download a lot of pictures from google maps,\n" + \
            "witch will probably cause your IP to be blocked by \n" + \
            "the google maps service for a few days.\n\n" + \
            "Are you sure you want to continue ?", \
            "Warning", wx.YES_NO | wx.NO_DEFAULT | wx.ICON_EXCLAMATION )
        if msgBox.ShowModal() == wx.ID_YES:
            self.mainWindow.startImportProcess()
    
    def onCancel(self, event):
        self.mainWindow.importProcessThread.progress.doCancel()

class BitmapPanel(wx.Panel):
    def __init__(self, parent, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)
        
        self.mainWindow = parent.mainWindow
        
        self.bitmap = wx.StaticBitmap(self, wx.ID_ANY, wx.BitmapFromImage( wx.EmptyImage( 800, 600 ) ))

class MainPanel(wx.Panel):
    def __init__(self, parent, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)
        
        self.mainWindow = parent
        
        sizerMain = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(sizerMain)
        
        sizerBorder = wx.BoxSizer(wx.HORIZONTAL)
        sizerMain.Add(sizerBorder)
        
        self.infoPanel = InfoPanel(self)
        sizerBorder.Add(self.infoPanel, border = 5, flag = wx.ALL)
        
        self.bitmapPanel = BitmapPanel(self)
        sizerBorder.AddSpacer(10)
        sizerBorder.Add(self.bitmapPanel, border = 5, flag = wx.ALL)
        
        # Progress gauge
        self.progressGauge = wx.Gauge(self, range = 100, style = wx.GA_SMOOTH)
        sizerMain.Add(self.progressGauge, flag = wx.EXPAND)

class MainWindow(wx.Frame):
    def __init__(self, parent, *args, **kwargs):
        wx.Frame.__init__(self, parent, style=wx.DEFAULT_FRAME_STYLE & ~(wx.RESIZE_BORDER | wx.MAXIMIZE_BOX), *args, **kwargs)
        
        self.googlemapsServer = "http://khm%d.google.com"
        
        self.dataPath = os.path.join("..", "..", "data")
        self.xmlTexturePath = os.path.join(self.dataPath, "terrain")
        self.xmlTerrainPath = os.path.join(self.dataPath, "xml", "terrain")
        self.terrainFile = "balkan.xml"
        self.cachePath = "cache"
        
        if not os.path.exists(self.cachePath):
            os.mkdir(self.cachePath)
        
        sizerFrame = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(sizerFrame)
        
        self.mainPanel = MainPanel(self)
        sizerFrame.Add(self.mainPanel)
        
        sizerFrame.SetSizeHints(self)
        self.Show(True)
        
        self.startReadTerrainInfo()
    
    def startReadTerrainInfo(self):
        self.mainPanel.infoPanel.Disable()
        self.readTerrainInfoThread = threading.Thread(target = self.readTerrainInfo)
        self.readTerrainInfoThread.start()
        wx.CallLater(1, self.onReadTerrainInfoTimer)
    
    def readTerrainInfo(self):
        fileFullName = os.path.join(self.xmlTerrainPath, self.terrainFile)
        doc = xml.dom.minidom.parse(fileFullName)
        
        self.terrainName = self.getXmlElementValue(doc, "name")
        self.textureBaseName = self.getXmlElementValue(doc, "lattice_basename")
        self.textureExtension = self.getXmlElementValue(doc, "lattice_texture_extension")
        self.center = csplib.LLA()
        self.center.parseXML( str( self.getXmlElementValue(doc, "center") ) )
        self.vertexSpacing = float( self.getXmlElementValue(doc, "vertex_spacing") )
        self.latticeWidth = int( self.getXmlElementValue(doc, "lattice_width") )
        self.latticeHeight = int( self.getXmlElementValue(doc, "lattice_height") )
        self.latticeTilesWidth = int( self.getXmlElementValue(doc, "lattice_tiles_width") )
        self.latticeTilesHeight = int( self.getXmlElementValue(doc, "lattice_tiles_height") )
        
        tiles.setTerrainInfo(self.vertexSpacing, self.latticeWidth, self.latticeHeight, self.latticeTilesWidth, self.latticeTilesHeight)
        
        self.mainPanel.infoPanel.updateInfo()
        self.updateCspProjection()
        self.updatePreviewProjection()
        self.downloadBackgroundBitmap()
    
    def onReadTerrainInfoTimer(self):
        if self.readTerrainInfoThread.isAlive():
            self.mainPanel.progressGauge.Pulse()
            wx.CallLater(100, self.onReadTerrainInfoTimer)
        else:
            self.mainPanel.progressGauge.SetValue(0)
            self.drawPreviewBackground()
            self.GetSizer().SetSizeHints(self)
            self.mainPanel.infoPanel.Enable()
            self.mainPanel.infoPanel.buttonStart.Enable()
    
    def getXmlElementValue(self, doc, name):
        for node in doc.documentElement.childNodes:
            if node.nodeType == node.ELEMENT_NODE:
                if node.getAttribute("name") == name:
                    if not node.hasChildNodes():
                        return ""
                    if node.firstChild.nodeType != node.TEXT_NODE:
                        return ""
                    return node.firstChild.nodeValue
        return ""
    
    def updateCspProjection(self):
        self.cspProjection = cspsim.SecantGnomonicProjection()
        self.cspProjection.set( self.center, tiles.VertexTile.totalSizeInMetersWidth, tiles.VertexTile.totalSizeInMetersHeight )
    
    def updatePreviewProjection(self):
        gridLLA = []
        for textureTile in tiles.allTextureTile():
            gridLLA.extend( map( self.cspProjection.convert, textureTile.corners() ) )
        
        for zoomLevel in googlemaps.MercatorProjection.zoomLevels:
            self.previewProjection = googlemaps.MercatorProjection( zoomLevel )
            gridPixel = map( self.previewProjection.llaToPixel, gridLLA )
            border = geometry.getBoundingRectangle( gridPixel )
            if border.width() > 1024 / 2 or border.height() > 768 / 2:
                break
        
        border.extend(1.10)
        border.top = self.previewProjection.boundPixelY(border.top)
        border.bottom = self.previewProjection.boundPixelY(border.bottom)
        
        self.previewLeft = border.left
        self.previewTop = border.top
        self.previewWidth = border.width()
        self.previewHeight = border.height()
    
    def allBackgroundBitmap(self):
        class BackgroundBitmap:
            pass
        
        backgroundPixel = googlemaps.Pixel()
        for row in range( int(math.ceil( float(self.previewHeight) / googlemaps.MercatorProjection.tileSize )) + 1 ):
            backgroundPixel.y = self.previewTop + row * googlemaps.MercatorProjection.tileSize
            for column in range( int(math.ceil( float(self.previewWidth) / googlemaps.MercatorProjection.tileSize )) + 1 ):
                backgroundPixel.x = self.previewLeft + column * googlemaps.MercatorProjection.tileSize
                backgroundBitmap = BackgroundBitmap()
                backgroundBitmap.row = row
                backgroundBitmap.column = column
                backgroundBitmap.tilePixel = self.previewProjection.pixelToTilePixel( backgroundPixel )
                backgroundBitmap.url, backgroundBitmap.fileName = self.previewProjection.tileUrl( backgroundBitmap.tilePixel.tile, self.googlemapsServer )
                backgroundBitmap.fileName = os.path.join(self.cachePath, backgroundBitmap.fileName)
                yield backgroundBitmap
    
    def downloadBackgroundBitmap(self):
        for backgroundBitmap in self.allBackgroundBitmap():
            if not os.path.exists(backgroundBitmap.fileName):
                try:
                    urlretrieve(backgroundBitmap.url, backgroundBitmap.fileName)
                except UrlNotFound:
                    pass
    
    def drawPreviewBackground(self):
        bitmap = wx.BitmapFromImage( wx.EmptyImage( self.previewWidth, self.previewHeight ) )
        dc = wx.MemoryDC( bitmap )
        
        for backgroundBitmap in self.allBackgroundBitmap():
            if os.path.exists(backgroundBitmap.fileName) and not fileIsEmpty(backgroundBitmap.fileName):
                image = wx.Image( backgroundBitmap.fileName )
                if image.IsOk():
                    dc.DrawBitmap( wx.BitmapFromImage(image), -backgroundBitmap.tilePixel.pixel.x + backgroundBitmap.column * googlemaps.MercatorProjection.tileSize, -backgroundBitmap.tilePixel.pixel.y + backgroundBitmap.row * googlemaps.MercatorProjection.tileSize )
        
        dc.SetPen( wx.GREEN_PEN )
        dc.SetBrush( wx.TRANSPARENT_BRUSH )
        
        for tile in tiles.allVertexTile():
            corners = map( self.cspProjection.convert, tile.corners() )
            corners = map( self.previewProjection.llaToPixel, corners )
            dc.DrawPolygon( [wx.Point( pixel.x, pixel.y ) for pixel in corners], -self.previewLeft, -self.previewTop )
        
        dc.SetPen( wx.TRANSPARENT_PEN )
        brush = wx.Brush( "YELLOW", wx.BDIAGONAL_HATCH )
        dc.SetBrush( brush )
        
        for vertexTile in tiles.allVertexTile():
            for textureTile in vertexTile.textureTiles():
                fileName = self.textureBaseName + textureTile.fileNameSuffix() + "." + self.textureExtension
                fileFullName = os.path.join(self.xmlTexturePath, fileName)
                if os.path.exists(fileFullName):
                    corners = map( self.cspProjection.convert, textureTile.corners() )
                    corners = map( self.previewProjection.llaToPixel, corners )
                    dc.DrawPolygon( [wx.Point( pixel.x, pixel.y ) for pixel in corners], -self.previewLeft, -self.previewTop )
        
        dc.SelectObject( wx.NullBitmap )
        self.mainPanel.bitmapPanel.bitmap.SetBitmap( bitmap )
    
    def updateGoogleProjection(self):
        cspScale = tiles.VertexTile.sizeInPixels * self.vertexSpacing / ( tiles.TextureTile.sizeInPixels * self.latticeTilesWidth ) # (meters/texture pixel)
        
        for zoomLevel in googlemaps.MercatorProjection.zoomLevels:
            self.googleProjection = googlemaps.MercatorProjection( zoomLevel )
            googleScale = self.googleProjection.scaleAt( self.center.latitude() )
            if googleScale < cspScale:
                zoomLevel = zoomLevel - 1
                break
        
        zoomLevel = zoomLevel + self.mainPanel.infoPanel.SpinCtrlOversampling.GetValue() - 1
        if zoomLevel not in googlemaps.MercatorProjection.zoomLevels:
            if zoomLevel < googlemaps.MercatorProjection.zoomLevels[0]: zoomLevel = googlemaps.MercatorProjection.zoomLevels[0]
            if zoomLevel > googlemaps.MercatorProjection.zoomLevels[-1]: zoomLevel = googlemaps.MercatorProjection.zoomLevels[-1]
            
        self.googleProjection = googlemaps.MercatorProjection( zoomLevel )
    
    def startImportProcess(self):
        class Progress(googlemaps.Progress):
            def __init__(self):
                googlemaps.Progress.__init__(self)
                self.__lock = threading.Lock()
                self.__cancel = threading.Event()
                self.__currentTilePercent = None
                self.__lastTilesProcessed = []
            
            def doCancel(self):
                self.__cancel.set()
            
            def mustCancel(self):
                return self.__cancel.isSet()
            
            def wait(self, timeout):
                self.__cancel.wait(timeout)
            
            def setCurrentTilePercent(self, percent):
                try:
                    self.__lock.acquire()
                    self.__currentTilePercent = percent
                finally:
                    self.__lock.release()
            
            def addLastTileProcessed(self, tile):
                try:
                    self.__lock.acquire()
                    self.__lastTilesProcessed.append( tile )
                finally:
                    self.__lock.release()
            
            def getValue(self):
                try:
                    self.__lock.acquire()
                    return copy.deepcopy(self.__currentTilePercent), copy.deepcopy(self.__lastTilesProcessed)
                finally:
                    if len(self.__lastTilesProcessed): self.__lastTilesProcessed = []
                    self.__lock.release()
        
        self.mainPanel.infoPanel.buttonTerrain.Disable()
        self.mainPanel.infoPanel.SpinCtrlOversampling.Disable()
        self.mainPanel.infoPanel.buttonStart.Disable()
        self.mainPanel.infoPanel.buttonCancel.Enable()
        self.importProcessThread = threading.Thread(target = self.importProcess)
        self.importProcessThread.progress = Progress()
        self.importProcessThread.start()
        wx.CallLater(1, self.onImportProcessTimer)
    
    def importProcess(self):
        self.importProcessThread.progress.setCurrentTilePercent( None )
        self.updateGoogleProjection()
        
        class TextureImageGenerator( googlemaps.TextureImageGenerator ):
            def __init__(self, mainWindow):
                self.mainWindow = mainWindow
                oversampling = int( 2**(mainWindow.mainPanel.infoPanel.SpinCtrlOversampling.GetValue() - 1) )
                
                googlemaps.TextureImageGenerator.__init__(self,
                    mainWindow.importProcessThread.progress,
                    mainWindow.cspProjection,
                    mainWindow.googleProjection,
                    oversampling,
                    tiles.TextureTile.sizeInPixels,
                    tiles.TextureTile.sizeInMetersWidth / tiles.TextureTile.sizeInPixels / oversampling,
                    tiles.TextureTile.sizeInMetersHeight / tiles.TextureTile.sizeInPixels / oversampling)
            
            def getImageData(self, mercatorProjection, mercatorTile):
                url, fileName = mercatorProjection.tileUrl( mercatorTile, self.mainWindow.googlemapsServer )
                fileName = os.path.join(self.mainWindow.cachePath, fileName)
                if not os.path.exists(fileName):
                    try:
                        urlretrieve(url, fileName, self.mainWindow.importProcessThread.progress)
                    except UrlNotFound:
                        return ""
                if fileIsEmpty( fileName ): return ""
                image = wx.Image( fileName )
                if image.IsOk():
                    return image.GetData()
                else:
                    return ""
        
        textureImageGenerator = TextureImageGenerator(self)
        
        for vertexTile in tiles.allVertexTileFromCenter():
            for textureTile in vertexTile.textureTiles():
                fileName = self.textureBaseName + textureTile.fileNameSuffix() + "." + self.textureExtension
                fileFullName = os.path.join(self.xmlTexturePath, fileName)
                if not os.path.exists(fileFullName):
                    print "Generating", fileName
                    imageData = textureImageGenerator.generate( textureTile.bottomLeft() )
                    if not imageData: return
                    image = wx.EmptyImage( tiles.TextureTile.sizeInPixels, tiles.TextureTile.sizeInPixels )
                    image.SetData( imageData )
                    image.SaveFile(fileFullName, wx.BITMAP_TYPE_BMP)
                    self.importProcessThread.progress.setCurrentTilePercent( 100 )
                    self.importProcessThread.progress.addLastTileProcessed(textureTile)
    
    def onImportProcessTimer(self):
        currentTilePercent, lastTilesProcessed = self.importProcessThread.progress.getValue()
        
        if currentTilePercent is None:
            self.mainPanel.progressGauge.Pulse()
        else:
            self.mainPanel.progressGauge.SetValue( currentTilePercent )
        
        if len(lastTilesProcessed):
            bitmap = self.mainPanel.bitmapPanel.bitmap.GetBitmap()
            dc = wx.MemoryDC( bitmap )
            dc.SetPen( wx.TRANSPARENT_PEN )
            brush = wx.Brush( "YELLOW", wx.BDIAGONAL_HATCH )
            dc.SetBrush( brush )
            for tile in lastTilesProcessed:
                corners = map( self.cspProjection.convert, tile.corners() )
                corners = map( self.previewProjection.llaToPixel, corners )
                dc.DrawPolygon( [wx.Point( pixel.x, pixel.y ) for pixel in corners], -self.previewLeft, -self.previewTop )
            dc.SelectObject( wx.NullBitmap )
            self.mainPanel.bitmapPanel.bitmap.SetBitmap( bitmap )
        
        if self.importProcessThread.isAlive():
            wx.CallLater(100, self.onImportProcessTimer)
        else:
            self.mainPanel.infoPanel.buttonTerrain.Enable()
            self.mainPanel.infoPanel.SpinCtrlOversampling.Enable()
            self.mainPanel.infoPanel.buttonStart.Enable()
            self.mainPanel.infoPanel.buttonCancel.Disable()


app = wx.PySimpleApp()
wx.Log.SetActiveTarget( wx.LogStderr() )
frame = MainWindow(None, title = "Google Maps importer", size = (640, 480))
app.MainLoop()
