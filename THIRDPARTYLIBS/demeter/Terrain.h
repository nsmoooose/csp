// Demeter Terrain Visualization Library by Clay Fowler
// Copyright (C) 2002 Clay Fowler

// $Id: Terrain.h,v 1.4 2003/04/06 10:39:18 deltasf Exp $

/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA  02111-1307, USA.
*/

#ifndef _TERRAIN_DEMETER_H_
#define _TERRAIN_DEMETER_H_

//#define _USE_GDAL_
//_USE_GDAL_ tells Demeter to support the GDAL data loading library. This allows Demeter to directly
//read DEM and SDTS files. Turn this option on only if you have installed the GDAL library on
//your system.

//#define _USE_RAYTRACING_SUPPORT_
//_USE_RAYTRACING_SUPPORT_ enables the ray tracing methods on the Terrain class, but increases
//memory usage considerably. Demeter's ray tracing is very fast and can be used for mouse picking,
//line-of-sight tests, etc. If you enable this option, then be aware that MUCH more memory will be needed.

#define _PROTECT_ACCESS_
// _PROTECT_ACCESS_ turns on extra checks to validate parameters passed to various terrain methods to make sure
// the parameters are within range. Turn this option on when you want the Terrain to clamp
// all values to legal ranges for you - this will slow performance but lets external callers
// not worry about clamping values. If external callers can be trusted to keep all parameters within
// legal extents, then disable this option for performance gains.

#ifdef _WIN32
#ifdef TERRAIN_EXPORTS
#define TERRAIN_API __declspec(dllexport)
#else
#define TERRAIN_API __declspec(dllimport)
#endif
#else
#define TERRAIN_API
#endif

typedef unsigned char   Uint8;
typedef signed char     Sint8;
typedef unsigned short  Uint16;
typedef signed short    Sint16;
typedef unsigned int    Uint32;
typedef signed int      Sint32;

#include <string.h>
#ifdef _WIN32
    #include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <vector>

//#include "mmgr.h"

#include "DemeterException.h"
#include "BitArray.h"

#ifndef M_PI
#define M_PI 3.141592654f
#endif

#ifdef _USE_GDAL_
#include <gdal_priv.h>
#endif


#define MAX_VERTICES_PER_FAN 10
#define DEMETER_EPSILON 0.00001f
#define DEMETER_INFINITY 999999999.0f


// Forgive the long comment lines. They are all single lines to accomodate doxygen.

namespace Demeter
{
    class TerrainBlock;
    class Terrain;
    class Texture;
    class TextureCell;
    class Manifest;
    class TriangleStrip;
    class TriangleFan;
    class Vector;

    /// An abstract factory that allows applications to provide and manage their own terrain textures, rather than letting Demeter manage textures. This is most useful for procedural texture algorithms.
    class TERRAIN_API TextureFactory
    {
    public:
        /// Called by the owning Terrain object when a texture has become visible and needs to rendered. The index is a unique ID for the texture, and the other parameters are in world units.
        virtual Texture*        GetTexture(int index,float originX,float originY,float width,float height) = 0;
        /// Called by the owning TerrainLattice object when a texture has become visible and needs to rendered. The parameters are in world units. This method is only called when a factory is being used by TerrainLattice.
        virtual void            GenerateTextureCell(Terrain* pTerrain,TextureCell*,float originX,float originY,float width,float height);
        /// Called by the owning Terrain object when a texture is no longer visible and can, therefore, be unloaded (or set to lower priority, etc.)
        virtual void            UnloadTexture(int index) = 0;

		// Called to set the Terrain being used to generate a texture.
		virtual void SetTerrain(Terrain * pTerrain);

    };

    /// An extremely basic 3D plane class.
    class TERRAIN_API Plane
    {
    public:
                            Plane() {}
                            Plane( Vector& p1,Vector& p2,Vector& p3 );
                            ~Plane() {}
        void                defineFromPoints( Vector& p1,Vector& p2,Vector& p3 );
        float               a,b,c,d;
    };

    /// An extremely basic 3D vector class. Demeter does not rely on all of the bells and whistles of most public vector classes
    class TERRAIN_API Vector
    {
    public:
                            Vector() {}
                            ~Vector() {}
        float               GetLength();
        float               Normalize( float tolerance = DEMETER_EPSILON );
		void				RotateZ(float theta);
        Vector&             operator = ( const Vector& vector );
        float               x,y,z;
    };

    class TERRAIN_API Ray
    {
    public:
                            Ray() {}
                            ~Ray() {}
        Vector              m_Origin;
        Vector              m_Direction;
    };

    class TERRAIN_API Box
    {
    public:
                            Box() {}
                            ~Box() {}
        Vector              m_Max;
        Vector              m_Min;
    };

    class TERRAIN_API Texture
    {
    public:
                        Texture(const Uint8* pBuffer,int width,int height,int stride,int borderSize,bool bClamp,bool useCompression,bool useAlpha = false);
                        Texture();
                        ~Texture();
        GLuint          UploadTexture();
        void            UnloadTexture();
        Uint8*          GetBuffer();
        int             GetWidth();
        int             GetHeight();
        void            Write(FILE* file,Terrain* pTerrain);
        void            Read(FILE* file,Terrain* pTerrain);
        void            SetSharedIndex(int index);
        int             GetSharedIndex();
        void            SetFilename(const char* szFilename);
        const char*     GetFilename();
        void            FlipVertical();
        void            FlipHorizontal();
        bool            IsBound();
        void            SetAlphaMaskOnly(bool isMaskOnly);
        static void     SetDefaultTextureFormat(int GLTextureFormat);
        static void     SetAlphaTextureFormat(int GLTextureFormat);
        static void     SetMaskTextureFormat(int GLTextureFormat);
        static void     SetCompressedTextureFormat(int GLTextureFormat);
    private:
        Uint8*          m_pBuffer;
        int             m_Width;
        int             m_Height;
        int             m_RowLength;
        int             m_BorderSize;
        bool            m_UseCompression;
        bool            m_bClamp;
        GLuint          m_TextureID;
        bool            m_bAlpha;
        int             m_SharedIndex;
        char*           m_szFilename;
        int             m_BytesPerPixel;
        bool            m_bAlphaMaskOnly;
        static int      m_DefaultTextureFormat;
        static int      m_CompressedTextureFormat;
        static int      m_AlphaTextureFormat;
        static int      m_MaskTextureFormat;
    };

    class TERRAIN_API DetailTexture
    {
    public:
                        DetailTexture(Texture* pTexture = NULL);
                        ~DetailTexture();
        void            SetMask(Texture*);
        void            SetTexture(Texture*);
        GLuint          BindMask();
        GLuint          BindTexture();
        void            Unbind();
        Texture*        GetMask();
        Texture*        GetTexture();
        void            Write(FILE* file,Terrain* pTerrain);
        void            Read(FILE* file,Terrain* pTerrain);
        void            FlipVertical();
        void            FlipHorizontal();
    private:
        Texture*        m_pMask;
        Texture*        m_pTexture;
    };
 

	//class   vpDetailTexture:public vector<DetailTexture*>{};

    class TERRAIN_API TextureCell
    {
    public:
                                TextureCell();
                                ~TextureCell();
        void                    SetTexture(Texture*);
        void                    AddDetail(DetailTexture*);
        GLuint                  BindTexture();
        int                     GetNumberOfDetails();
        GLuint                  BindMask(int index);
        GLuint                  BindDetail(int index);
        void                    UnbindAll();
        DetailTexture*          GetDetail(int index);
        DetailTexture*          GetDetail(Texture*);
        void                    Write(FILE* file,Terrain* pTerrain);
        void                    Read(FILE* file,Terrain* pTerrain);
        void                    FlipHorizontal();
        void                    FlipVertical();
    private:
        Texture*                m_pTexture;
		vector<TERRAIN_API DetailTexture*>  m_DetailTextures;
		//vpDetailTexture         m_DetailTextures;

    };
 
	
    class TERRAIN_API TextureSet
    {
    public:
                                TextureSet();
                                ~TextureSet();
        void                    AddTexture(Texture*);
        Texture*                GetTexture(int index);
        int                     GetNumTextures();
        void                    Read(FILE* file,Terrain* pTerrain);
        void                    Write(FILE* file,Terrain* pTerrain);
    private:
        vector<Texture*>        m_Textures;
    };
 
    /// This class represents a single, contiguous chunk of terrain and is the primary public interface to Demeter. Most applications will create a single instance of this class to represent the terrain in the application, but multiple instances can be used to stitch together a very large world comprised of multiple terrains using the TerrainLattice class.
    class TERRAIN_API Terrain
    {
    public:
                    /// Constructs a new terrain from image files representing grayscale elevation and texture data. 
                        Terrain(const char* szElevationsFilename,const char* szTextureFilename,const char* szDetailTextureFilename,float vertexSpacing,float elevationScale,int maxNumTriangles,bool bUseBorders = false,float offsetX = 0.0f,float offsetY = 0.0f,int numTexturesX = 0,int numTexturesY = 0);
                    /// Constructs a new terrain from raw elevation and image data in memory buffers. 
                        Terrain(const float* pElevations,int elevWidth,int elevHeight,const Uint8* pTextureImage,int textureWidth,int textureHeight,const Uint8* pDetailTextureImage,int detailWidth,int detailHeight,float vertexSpacing,float elevationScale,int maxNumTriangles,bool bUseBorders = false,float offsetX = 0.0f,float offsetY = 0.0f,int numTexturesX = 0,int numTexturesY = 0);
                    /// Constructs a new terrain of the specified size with zero elevations and no texture data. 
                        Terrain(int widthVertices,int heightVertices,float vertexSpacing,int maxNumTriangles,bool bUseBorders);
                    /// Destructor.
                        ~Terrain();
                    /// Based on the current viewing parameters, this method breaks the terrain down into a visually optimum set of triangles that can be rendered. Normally, an application will never call this method directly, but will instead call the method ModelViewMatrixChanged() to allow Demeter to take care of this automatically.
        int             Tessellate();
                    /// Renders the terrain to the current OpenGL surface. \warning Applications should always call ModelViewMatrixChanged() at least once prior to calling Render(), so that Demeter will have a chance to tessellate the terrain.
        void            Render();
                    /// Sets the "detail theshold", which controls how much Demeter simplifies the terrain. Higher thresholds will increase performance but make the terrain look worse, while lower thresholds will reduce performance and increase visual quality. Extremely high thresholds can cause significant visual artifacts and make the terrain look very strange.
        void            SetDetailThreshold( float threshold );
                    /// Returns the "detail threshold", which controls how much Demeter simplifies the terrain. Higher thresholds will increase performance but make the terrain look worse, while lower thresholds will reduce performance and increase visual quality. Extremely high thresholds can cause significant visual artifacts and make the terrain look very strange.
        float           GetDetailThreshold();
                    /// Sets the maximum size of blocks that can be simplified when the terrain is tessellated. The parameter stride specifies the number of vertices along one edge of the block (blocks are always square.) This parameter can be used by applications that allow much of the terrain to be visible from above, such as flight simulators, to prevent oversimplification of terrain in the distance. Setting this value too low will adversely affect performance.
        void            SetMaximumVisibleBlockSize( int stride );
                    /// Returns the width of the terrain in vertices (this is the count of vertices along the world's x-axis.)
        int             GetWidthVertices();
                    /// Returns the height of the terrain in vertices (this is the count of vertices along the world's y-axis.)
        int             GetHeightVertices();
                    /// Returns the width of the terrain in real units (this is the length of the terrain along the world's x-axis.)
        float           GetWidth() const;
                    /// Returns the height of the terrain in real units (this is the length of the terrain along the world's y-axis.)
        float           GetHeight() const;
                    /// Returns the number of real units between vertices in the terrain's mesh.
        float           GetVertexSpacing() const;
                    /// Returns the elevation (z-coordinate) in real units of the specified point on the terrain.
        float           GetElevation( float x,float y );
                    /// Returns the elevation (z-coordinate) in real units of the specified terrain vertex.
        float           GetElevation( int index );
                    /// Returns the surface normal of the terrain at the specified point.
        void            GetNormal( float x,float y,float& normalX,float& normalY,float& normalZ );
                    /// Returns the elevation (z-coordinate) in real units of the highest point on the terrain.
        float           GetMaxElevation() const;
                    /// Returns the elevation (z-coordinate) in real units of the specified vertex on the terrain.
        float           GetVertexElevation(int index) const;
                    /// Sets the elevation (z-coordinate) in real units of the specified vertex on the terrain.
        void            SetVertexElevation(int index,float newElevation);
                    /// Sets the elevation (z-coordinate) in real units of the nearest vertex to the specified point.
        void            SetVertexElevation(float x,float y,float newElevation);
		void			SetAllElevations(const char* szFilename,float vertexSpacing,float elevationScale = 1.0f);
		void			SetAllElevations(const float* pElevations,int elevWidth,int elevHeight,float vertexSpacing,float elevationScale = 1.0f);
                    /// Returns the total number of vertices in the terrain's mesh.
        int             GetNumberOfVertices();
                    /// Returns the width (in vertices) of the terrain's texture tiles.
        float           GetTextureTileWidth();
                    /// Returns the height (in vertices) of the terrain's texture tiles.
        float           GetTextureTileHeight();
                    /// Returns the number of texture tiles along the terrain's x-axis.
        int             GetNumberOfTextureTilesWidth();
                    /// Returns the number of texture tiles along the terrain's y-axis.
        int             GetNumberOfTextureTilesHeight();
                    /// Applies the specified graphics image as a texture to the terrain. This is done by breaking the specified image up into smaller textures of 256x256 called "tiles" and mapping these contiguously onto the terrain's surface. The parameter bUseBorders specified whether or not the texture border OpenGL extensions should be used when creating the tiles. Textures are automatically applied when loading compiled map files, so use of this method is strictly optional.
        bool            SetTexture( const char* szFilename,bool bUseBorders );
                    /// Applies the specified graphics image as a texture to the terrain. This is done by breaking the specified image up into smaller textures of 256x256 called "tiles" and mapping these contiguously onto the terrain's surface. The parameter bUseBorders specified whether or not the texture border OpenGL extensions should be used when creating the tiles. Textures are automatically applied when loading compiled map files, so use of this method is strictly optional.
        bool            SetTexture( const Uint8* pTextureImage,int width,int height,bool bUseBorders );
                    /// Uses the specified graphics file to apply a "common" texture to the entire surface of the terrain. A common texture is repeated across the entire terrain and is blended with the terrain's normal texture (if blending is supported by the user's hardware - which covers almost all OpenGL cards.) This is used to provide a "detailed" texture to give the ground some definition when the camera is close to the ground.
        bool            SetCommonTexture( const char* szFilename );
                    /// Uses the specified graphics file to apply a "common" texture to the entire surface of the terrain. A common texture is repeated across the entire terrain and is blended with the terrain's normal texture (if blending is supported by the user's hardware - which covers almost all OpenGL cards.) This is used to provide a "detailed" texture to give the ground some definition when the camera is close to the ground.
        bool            SetCommonTexture( const Uint8* pImage,int width,int height );
                    /// Notifies Demeter that OpenGL's modelview matrix has been modified, allowing Demeter to tessellate the terrain based on the new modelview matrix. It is IMPERATIVE that his method be called every time the modelview matrix is changed, even if this is in every single rendering cycle of the application.
        int             ModelViewMatrixChanged();
                    /// Returns whether or not the specified cuboid is inside of the viewing frustum (as defined at the previous call to ModelViewMatrixChanged())
        bool            CuboidInFrustum( const Box& cuboid );
#ifdef _USE_RAYTRACING_SUPPORT_
                    /// Casts a ray from the specified point, in the specified direction, and calculates the ray's point of intersection with the terrain. The return value is the distance from the starting point to the intersection. This method makes use of the terrain's quad tree to optimize the ray-tracing.
        float           IntersectRay( float startX,float startY,float startZ,float dirX,float dirY,float dirZ,float& intersectX,float& intersectY,float& intersectZ );
                    /// An overload of IntersectRay that also provides information about the texture coordinates of the intersected point in addition the position of the point.
        float           IntersectRay( float startX,float startY,float startZ,float dirX,float dirY,float dirZ,float& intersectX,float& intersectY,float& intersectZ,int& textureCellX,int& textureCellY,float& texU,float& texV );
#endif
                    /// Quickly find the 3D point on the terrain where a given point on the screen is (for example, for mouse picking.) This is cheaper and faster than using the ray tracing methods.
        bool            Pick(int screenX,int screenY,float& pickedX,float& pickedY,float& pickedZ);
                    /// Quickly find the 3D point on the terrain where a given point on the screen is (for example, for screen picking.) This is cheaper and faster than using the ray tracing methods.
        bool            Pick(int screenX,int screenY,float& pickedX,float& pickedY,float& pickedZ,int& textureCellX,int& textureCellY,float& texU,float& texV);
                    /// Returns the texture coordinates of a given point on the terrain in world coordinates.
        void            GetTextureCoordinates(float x,float y,int& textureCellX,int& textureCellY,float& texU,float& texV);
                    /// Indicates whether or not OpenGL multitexture extensions are available from the OpenGL driver that this terrain instance is currently rendering against (as determined at the time the terrain instance was first constructed.)
        bool            IsMultiTextureSupported();
        void            SetLatticePosition(int x,int y);
        void            GetLatticePosition(int& x,int& y) const;
                    /// Installs an application-provided TextureFactory, disabling the default one supplied by Demeter. This allows applications to manage textures for themselves.
        void            SetTextureFactory(TextureFactory* pFactory);
        TextureCell*    GetTextureCell(int index);
        TextureCell*    GetTextureCell(int textureCellX,int textureCellY);
                    /// Writes a binary "surface" file for fine-grained detail textures. NOTE: the format of this file will be changed soon.
        void            Write(char* szFilename);
                    /// Reads a binary "surface" file for fine-grained detail textures. NOTE: the format of this file will be changed soon.
        void            Read(char* szFilename);
        TextureSet*     GetTextureSet();
        void            SetTextureSet(TextureSet* pTextureSet);
        void            SetTextureCell(int index,TextureCell* pCell);
        enum            DIRECTION {DIR_NORTH = 0,DIR_NORTHEAST = 1,DIR_EAST = 2,DIR_SOUTHEAST = 3,DIR_SOUTH = 4,DIR_SOUTHWEST = 5,DIR_WEST = 6,DIR_NORTHWEST = 7,DIR_CENTER = 8,DIR_INVALID = 9};
                    /// Increment the reference count by one, indicating that this object has another pointer which is referencing it.

		int             GetTerrainPolygonsRendered();
        
		
		inline void     ref() const { ++m_refCount; }
                    /// Decrement the reference count by one, indicating that a pointer to this object is referencing it. If the reference count goes to zero, it is assumed that this object is no longer referenced and is automatically deleted.
        inline void     unref() const { --m_refCount; if (m_refCount<=0) delete this; }
    private:
        void            Init(const Uint8* pTextureImage,int textureWidth,int textureHeight,const Uint8* pDetailTextureImage,int detailWidth,int detailHeight,bool bUseBorders,float offsetX = 0.0f,float offsetY = 0.0f,int numTexturesX = 0,int numTexturesY = 0);
        void            UpdateNeighbor(Terrain* pTerrain,DIRECTION direction);
		void            GenerateTextureCoordinates();
        void            SetVertexStatus(int index,bool status);
        bool            GetVertexStatus(int index);
        void            BuildBlocks();
        void            ChopTexture(const Uint8* pImage,int width,int height,int tileSize,bool bUseBorders);
        void            ExtractFrustum();
        void            FlipTexturesForMapping();
        void            PreloadTextures();
        int             m_WidthVertices;
        int             m_HeightVertices;
        float           m_DetailThreshold;
        float           m_VertexSpacing;
        TerrainBlock*   m_pRootBlock;
        BitArray*       m_pVertexStatus;
        int             m_NumberOfVertices;
        vector<TextureCell*> m_TextureCells;
        float           m_TextureTileWidth,m_TextureTileHeight;
        int             m_NumberOfTextureTilesWidth;
        int             m_NumberOfTextureTilesHeight;
        int             m_TileSize;
        Uint8**         m_pTiles;
        int             m_NumberOfTextureTiles;
        Texture*        m_pCommonTexture;
        float           m_FogColorRed,m_FogColorGreen,m_FogColorBlue,m_FogColorAlpha;
        Vector*         m_pVertices;
        int             m_MaximumVisibleBlockSize;
        TriangleStrip*  m_pTriangleStrips;
        TriangleFan*    m_pTriangleFans;
        int             m_CountStrips,m_CountFans;
        float           m_Frustum[6][4];
        bool            m_bMultiTextureSupported;
        int             m_MaxNumberOfPrimitives;
        float           m_MaxElevation;
        float           m_OffsetX,m_OffsetY;
        int             m_LatticePositionX,m_LatticePositionY;
        TextureFactory* m_pTextureFactory;
        TextureSet*     m_pTextureSet;
        float*          m_pTextureMain;
        float*          m_pTextureDetail;
		Vector*			m_pNormals;
        mutable int     m_refCount;
#ifdef _WIN32
		static vector<HGLRC> m_SharedContexts;
#endif

        friend class    Triangle;
        friend class    TriangleStrip;
        friend class    TerrainBlock;
        friend class    TriangleFan;
        friend class    TerrainLattice;
    };

    /// The Settings class is simply a manager of global variables. It provides a single place to set and retrieve all of the global settings that affect Demeter as a whole.
    class TERRAIN_API Settings
    {
    public:
                        /// Destructor.
                            ~Settings();
                        /// Returns the global instance of this class. The Settings class acts a singleton, so there is only one instance of this class per application.
        static Settings*    GetInstance();
                        /// Sets the filesystem path that Demeter will look in when reading textures, maps, etc.
        void                SetMediaPath( const char* szPath );
                        /// Gets the filesystem path that Demeter will look in when reading textures, maps, etc.
        void                GetMediaPath( char** szPath );
                        /// Prepends the current media path to the specified filename. It is the caller's responsibility to allocate szFullFilename with sufficient size to handle the entire filename.
        void                PrependMediaPath( const char* szFilename,char* szFullFilename );
                        /// Specifies whether or not Demeter should output diagnostic information at runtime.
        void                SetVerbose( bool bVerbose );
                        /// Indicates whether or not Demeter is outputting diagnostic information at runtime.
        bool                IsVerbose();
                        /// Tells Demeter what the current width of the rendering surface is.
        void                SetScreenWidth( int width );
                        /// Retrieves the current width of the rendering surface assumed by Demeter.
        int                 GetScreenWidth();
                        /// Tells Demeter what the current height of the rendering surface is.
        void                SetScreenHeight( int height );
                        /// Retrieves the current height of the rendering surface assumed by Demeter.
        int                 GetScreenHeight();
                        /// Sets a named global property to the specified value.
        bool                SetProperty( const char* szProperty,const char* szValue );
                        /// Retrieves a named global property.
        bool                GetProperty( const char* szProperty,char* szValue );
                        /// Indicates whether or not the application that is using Demeter is a terrain compiler-like tool or a real application
        bool                IsCompilerOnly();
                        /// Determins whether or not the application that is using Demeter is a terrain compiler-like tool or a real application
        void                SetCompilerOnly( bool bIsCompilerOnly );
                        /// Specifies whether or not textures should be unloaded when they are not visible. This allows larger amounts of texture data, but at the expense of performance.
        void                SetUseDynamicTextures(bool useDynamic);
        bool                UseDynamicTextures();
                        /// Specifies whether or not textures should be compressed.
        void                SetTextureCompression(bool bCompress);
        bool                IsTextureCompression();
                        /// Puts Demeter in a non-graphics mode. This prevents textures from being loaded, geometry from being built, etc. This allows terrains to be created on a server or other "headless" environment where only the elevation data itself is required.
        void                SetHeadless(bool isHeadless);
        bool                IsHeadless();
                        /// Puts Demeter in editor mode. This makes memory usage much higher but makes dynamically editing the terrain easier for applications such as terrain editors.
        void                SetEditor(bool isEditor);
        bool                IsEditor();
                        /// Set the tessellate method to use for this terrain
        void                SetTessellateMethod(int method);
        int                 GetTessellateMethod();
                        /// ZWeight to make screen Z coordinate more significant in level of detail decisions.
        void                SetTessellateZWeight(float method);
        float               GetTessellateZWeight();
                        /// Specifies how many times detail textures are repeated within their parent textures. Higher values will make the detail textures smaller.
        void                SetDetailTextureRepeats(float repeats);
        float               GetDetailTextureRepeats();
                        /// Specifies the maximum distance between the surface of the terrain and a picked point that can be considered picking the terrain (as opposed to an application-specific object.)
        void                SetPickThreshold(float threshold);
        float               GetPickThreshold();
                        /// Specifies whether or not all of a terrain's textures should be preloaded when the terrain is loaded. This makes terrains load more slowly but they run more smoothly since textures aren't loaded as the user's camera moves.
        void                SetPreloadTextures(bool bPreload);
        bool                GetPreloadTextures();
		void				SetUseNormals(bool bUseNormals);
		bool				UseNormals();

		void                LogStats();

// TM_NEW - the latest method, like TM_OLD_NEW, but with skew bounding boxes.
// TM_SCREEN_RECT - goes purely by 2D screen-size of terrainblock.
// TM_OLD_UPRIGHTONLY - the "original" method
// TM_2D_ROLLONLY - like TM_SCREEN_RECT but also looks at 2D screen size of Z axis of bounding box
// TM_OLD_NEW - goes by "screen-space" 3D length of Z axis of bounding box
        enum                TESSELLATEMETHOD {TM_NEW = 0, TM_SCREEN_RECT = 1, TM_OLD_UPRIGHTONLY = 2, TM_2D_ROLLONLY = 3, TM_OLD_NEW = 4};
    private:
                            Settings();
        static Settings*    m_pInstance;
        char*               m_szMediaPath;
        bool                m_bVerbose;
        bool                m_bIsCompilerOnly;
        bool                m_bCompressTextures;
        int                 m_ScreenWidth,m_ScreenHeight;
        bool                m_IsHeadless;
        bool                m_UseDynamicTextures;
        bool                m_IsEditor;
        int                 m_TessellateMethod;
        float               m_TessellateZWeight;
        float               m_DetailTextureRepeats;
        float               m_PickThreshold;
        bool                m_bPreloadTextures;
		bool				m_bUseNormals;


    };

    class TERRAIN_API TriangleFan
    {
    public:
                        TriangleFan();
                        ~TriangleFan();
        void            Render( Terrain* pTerrain );
        void            Setup( Terrain* pTerrain );
		static int      GetCount() { return s_Count; }
    private:
        int             m_pVertices[MAX_VERTICES_PER_FAN]; // Indices into the terrain vertices
        // TBD: It is a pretty awful waste of memory to preallocate MAX_VERTICES_PER_FAN vertices for every triangle fan,
        // when in most cases only a few vertices are needed. However, dynamically allocating these vertices
        // during every tessellation is not an option either because it causes huge performance problems and
        // badly fragments memory. Any good ideas for this?
        unsigned char   m_NumberOfVertices;
        float           minX,minY;
        int             textureId;

		static int      s_Count;

        friend class TerrainBlock;
        friend class Terrain;
    };

    class TERRAIN_API TriangleStrip
    {
    public:
                        TriangleStrip();
                        ~TriangleStrip();
        void            Render( Terrain* pTerrain );
        void            Setup( Terrain* pTerrain );
		static int      GetCount() { return s_Count; }
    private:
        GLuint          m_pVertices[6]; // Indices into the terrain vertices
        unsigned char   m_NumberOfVertices;
        float           minX,minY;
        int             textureId;
        bool            m_bEnabled;
		static int      s_Count;

        friend class TerrainBlock;
        friend class Terrain;
    };

    class TERRAIN_API Triangle
    {
    public:
                            Triangle();
                            ~Triangle();
        void                DefineFromPoints( Vector& p1,Vector& p2,Vector& p3 );
        Vector*             GetVertex( int index );
#ifdef _USE_RAYTRACING_SUPPORT_
        Plane*              GetPlane();
#endif
    private:
        Vector              m_pVertices[3];
#ifdef _USE_RAYTRACING_SUPPORT_
        Plane               m_Plane;
#endif
    };

    class TerrainBlock
    {
    public:
                            TerrainBlock( TerrainBlock* pParent );
                            TerrainBlock( int homeVertex,int stride,Terrain* pTerrain,TerrainBlock* pParent );
                            ~TerrainBlock();
        void                Tessellate( const double* pMatrixModelview,const double* pMatrixProjection,const int* pViewport,TriangleStrip* pTriangleStrips,int* pCountStrips,Terrain* pTerrain );
        void                Write( FILE* file );
        void                Read( FILE* file,Terrain* pTerrain );
        bool                IsActive();
        void                RepairCracks( Terrain* pTerrain,TriangleFan* pTriangleFans,int* pCountFans );
        int                 GetStride();
        void                EnableStrip( bool bEnabled );
        int                 GetHomeIndex();
        void                CalculateGeometry( Terrain* pTerrain );
		static int          GetCount() { return s_Count; };

#ifdef _USE_RAYTRACING_SUPPORT_
        void                IntersectRay( const Ray& ray,Vector& intersectionPoint,float& lowestDistance,const Terrain* pTerrain );
#endif
    private:
		static int      s_Count;

        void                CreateTriangleStrip( TriangleStrip* pTriangleStrips,int* pCount,Terrain* pTerrain );
        TerrainBlock**      m_pChildren;
        int                 m_HomeIndex;
        short int           m_Stride;
        bool                m_bIsActive : 1;
        bool                m_bChildrenActive : 1;
        TriangleStrip*      m_pTriangleStrip;
        float               m_MinElevation,m_MaxElevation;
    // FIXME: Don't need both BPlane_Max_D/Min_D and Box.m_Max/Min.z
    // Bounding Planes given by A,B,Max_D and A,B,Min_D - C defined as 1
        float               m_BPlane_A, m_BPlane_B, m_BPlane_Max_D, m_BPlane_Min_D;
#ifdef _USE_RAYTRACING_SUPPORT_
        Triangle*           m_pTriangles;
#endif

        friend class Terrain;
    };

    class TerrainLoadListener
    {
    public:
        virtual void        TerrainLoaded(Terrain* pTerrain) = 0;
        virtual void        TerrainUnloading(Terrain* pTerrain) = 0;
    };

    class TERRAIN_API TerrainLattice
    {
    public:
                                        TerrainLattice(const char* szBaseName,const char* szExtensionElev,const char* szExtensionTex,const char* szDetailTextureName,float vertexSpacing,float elevationScale,int maxNumTriangles,bool bUseBorders,int widthTerrains,int heightTerrains);
                                        ~TerrainLattice();
        void                            Load(char* szBaseName,int maxNumTriangles,int maxBlockSize,float commonRepeats,bool bUseBorders = false);
        void                            AddTerrainLoadListener(TerrainLoadListener& listener);
        void                            RemoveTerrainLoadListener(TerrainLoadListener& listener);
        void                            AddTerrain(Terrain* pTerrain,int positionX,int positionY);
        Terrain*                        GetTerrain(int positionX,int positionY);
        Terrain*                        GetTerrainAtPoint(float x,float y);
        void                            SetCameraPosition(float x,float y,float z);
        void                            SetDetailThreshold(float threshold);
        void                            Tessellate();
        void                            Render();
        float                           GetElevation(float x,float y);
        /// Returns the surface normal of the terrain at the specified point.
        void                            GetNormal( float x,float y,float& normalX,float& normalY,float& normalZ );

        float                           GetWidth();
        float                           GetHeight();
		void                            SetTextureFactory(TextureFactory* pFactory,int tilesWidth,int tilesHeight);
        void                            GenerateTextures(TextureSet*,TextureFactory*,int tilesWidth,int tilesHeight);
		int                             GetLatticePolygonsRendered();

        
		/// Increment the reference count by one, indicating that this object has another pointer which is referencing it.
        inline void     ref() const { ++m_refCount; }
        /// Decrement the reference count by one, indicating that a pointer to this object is referencing it. If the reference count goes to zero, it is assumed that this object is no longer referenced and is automatically deleted.
        inline void     unref() const { /*assert(m_refCount > 0);*/ if (m_refCount > 0) {--m_refCount; if (m_refCount==0 && this) delete this;} }

    private:
        vector<TerrainLoadListener*>    m_TerrainLoadListeners;
        Terrain::DIRECTION              GetOppositeDirection(Terrain::DIRECTION direction);
        Terrain*                        GetTerrainRelative(Terrain* pTerrain,int positionX,int positionY);
        Terrain*                        GetTerrainRelative(Terrain* pTerrain,Terrain::DIRECTION direction);
        void                            LoadTerrain(int index);
        int                             m_WidthTerrains,m_HeightTerrains;
        int                             m_WidthActiveTerrains,m_HeightActiveTerrains;
        float                           m_TerrainWidth,m_TerrainHeight;
        Terrain**                       m_pTerrains;
        int                             m_CurrentTerrainIndex[9];
		int                             m_OffsetIndexX[9];
		int								m_OffsetIndexY[9];
        string                          m_BaseName;
        int                             m_MaxNumTriangles;
        bool                            m_bUseBorders;
        float                           m_VertexSpacing;
        float                           m_ElevationScale;
        char*                           m_szExtensionElev;
        char*                           m_szExtensionTex;
        char*                           m_szBaseName;
        char*                           m_szDetailTextureName;
        TextureFactory*                 m_pTextureFactory;
        int                             m_FactoryTilesWidth;
        int                             m_FactoryTilesHeight;
		float                           m_fThreshold;
        mutable int     m_refCount;

    };
}

#endif
