
CXXFLAGS = -I../../ThirdPartyLibs/ImageLib/include -g -O0 -Wall -Wpointer-arith -Wmissing-prototypes -Wmissing-declarations -Wcast-align -Wwrite-strings

OBJECTS = BaseResource.o ResourceManager.o TextureManager.o Config.o \
 SCNDataLoader.o TextureZoom.o Frustum.o Sector.o Tools.o GLTexture.o \
 SkyBox.o Vector.o LightingTexture.o TerrainData.o WaterLayer.o \
 MipMapQuad.o TerrainDataLoader.o glextensions.o MipMapRenderer.o \
 TerrainDataManager.o global.o QuadTexture.o  TerrainEngine.o \
 Texture.o

libTerrain.a : $(OBJECTS)
	ar cru libTerrain.a $(OBJECTS)

clean : 
	/bin/rm -f $(OBJECTS) libTerrain.a

