// Demeter Terrain Visualization Library by Clay Fowler
// Copyright (C) 2002 Clay Fowler

/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA  02111-1307, USA.
*/

# if defined(_MSC_VER) && (_MSC_VER <= 1300)
#pragma warning( disable : 4786 )
#endif

#include <fstream>

#ifdef _WIN32
    #include <crtdbg.h>
#endif

#include "assert.h"

#define DETAIL_TEXTURE_SIZE 512 

#ifdef _WIN32 
#include "io.h"
#else
#include "sys/io.h"
#endif

#include "fcntl.h"
#include "sys/stat.h"

#include "SDL/SDL_image.h"

#ifndef _WIN32
 #define GL_GLEXT_PROTOTYPES
 #include "SDL_opengl.h"
 
 #define _O_BINARY 0
 #define _open(a,b) open((a),(b))
 #define _close(a) close((a))
 #define _read(a,b,c) read((a),(b),(c))
#endif

//#ifdef DEMETER_MEMORYMANAGER
//#include "mmgr.h"
//#endif

#include "Terrain.h"
#include "BitArray.h"

#undef LoadImage

#define GL_CLAMP_TO_EDGE_EXT                0x812F
#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1
#define GL_COMBINE_RGB_EXT                  0x8571
#define GL_ARRAY_ELEMENT_LOCK_FIRST_EXT     0x81A8
#define GL_ARRAY_ELEMENT_LOCK_COUNT_EXT     0x81A9
#define COMPRESSED_RGB_S3TC_DXT1_EXT        0x83F0

#ifdef _WIN32
    typedef void (APIENTRY * PFNGLMULTITEXCOORD2FARBPROC)(GLenum texture,GLfloat s,GLfloat t);
    typedef void (APIENTRY * PFNGLACTIVETEXTUREARBPROC)(GLenum texture);
    typedef void (APIENTRY * PFNGLLOCKARRAYSEXTPROC)(GLint first, GLsizei count);
    typedef void (APIENTRY * PFNGLUNLOCKARRAYSEXTPROC)(void);
    typedef void (APIENTRY * PFNGLCLIENTACTIVETEXTUREARBPROC)(GLenum texture);
#else
    #ifdef MAC_OS_X
        typedef void (*PFNGLMULTITEXCOORD2FARBPROC)(GLenum texture,GLfloat s,GLfloat t);
        typedef void (*PFNGLACTIVETEXTUREARBPROC)(GLenum texture);
        typedef void (*PFNGLLOCKARRAYSEXTPROC)(GLint first, GLsizei count);
        typedef void (*PFNGLUNLOCKARRAYSEXTPROC)(void);
        typedef void (*PFNGLCLIENTACTIVETEXTUREARBPROC)(GLenum texture);
    #else
        typedef void (GLAPIENTRY *PFNGLMULTITEXCOORD2FARBPROC)(GLenum texture,GLfloat s,GLfloat t);
        typedef void (GLAPIENTRY *PFNGLACTIVETEXTUREARBPROC)(GLenum texture);
        typedef void (GLAPIENTRY *PFNGLLOCKARRAYSEXTPROC)(GLint first, GLsizei count);
        typedef void (GLAPIENTRY *PFNGLUNLOCKARRAYSEXTPROC)(void);
        typedef void (GLAPIENTRY *PFNGLCLIENTACTIVETEXTUREARBPROC)(GLenum texture);
    #endif
#endif

PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB_ptr = NULL;
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB_ptr = NULL;
PFNGLLOCKARRAYSEXTPROC glLockArraysEXT_ptr = NULL;
PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT_ptr = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB_ptr = NULL;

using namespace Demeter;

Settings* pSettingsInstance = NULL; // Singleton instance of the global settings

const int MAX_FILENAME_LENGTH = 2048;

float numBlocks = 0.0f;
float numLevels = 0.0f;
float hashDelta = 0.0f;
bool bMultiTextureSupported = true;

static int m_TerrainPolygonsRendered = 0;
static int m_LatticePolygonsRendered = 0;

int Terrain::GetTerrainPolygonsRendered() { return m_TerrainPolygonsRendered; }
int TerrainLattice::GetLatticePolygonsRendered() { return m_LatticePolygonsRendered; }

GLuint CreateTexture(const Uint8* pTexels,int width,int height,int rowLength,int border,int internalFormat,bool bClamp,bool bAlpha = false);
void LoadImage(const char* szFilename,int& pWidth,int& pHeight,Uint8** pBuffer,bool bAlpha = false);
void LoadRawImage(const char* szFilename,int& pWidth,int& pHeight,Uint8** pBuffer,bool bAlpha = false);
int RayPlaneIntersect(const Ray *ray,const Plane *plane,Vector* point,float *distance);
int RayBoxIntersect(const Ray *ray,const Box *box,Vector *point,float *distance);
bool IsPowerOf2(double number);
void DimensionPowerOf2(int origX,int origY,int& newX,int& newY);
void LoadGLExtensions();

int Texture::m_DefaultTextureFormat = GL_RGB8;
int Texture::m_CompressedTextureFormat = COMPRESSED_RGB_S3TC_DXT1_EXT;
int Texture::m_MaskTextureFormat = GL_RGBA;
int Texture::m_AlphaTextureFormat = GL_ALPHA;



#ifdef _WIN32
vector<HGLRC> Terrain::m_SharedContexts;
#endif

std::ofstream m_Logfile("Demeter.log");

int TriangleFan::s_Count = 0;
int TriangleStrip::s_Count = 0;
int TerrainBlock::s_Count = 0;

#ifndef _WIN32
long _filelength(int handle)
{
   int len = 0;
   struct stat statbuf;
	
	if (fstat(handle, &statbuf) == -1)
		strerror(errno);
	
	len = statbuf.st_size;
	return len;
}
#endif


TerrainBlock::TerrainBlock(TerrainBlock* pParent)
{
	s_Count++;
    m_pChildren = NULL;
    m_pTriangleStrip = NULL;
#ifdef _USE_RAYTRACING_SUPPORT_
    m_pTriangles = NULL;
#endif
}

TerrainBlock::TerrainBlock(int homeVertex,int stride,Terrain* pTerrain,TerrainBlock* pParent)
{
	s_Count++;
    m_pTriangleStrip = NULL;
    m_HomeIndex = homeVertex;
    m_Stride = stride;
    static int numBlocksBuilt = 0;

	if (Settings::GetInstance()->IsVerbose())
    {
        if (hashDelta <= numBlocksBuilt++)
        {
            m_Logfile  << "#" << flush;
            numBlocksBuilt = 0;
        }
    }

// Recursively build children blocks of this block.
    if (2 < m_Stride)
    {
        m_pChildren = new TerrainBlock*[4];
        int childrenStride = m_Stride / 2;
        m_pChildren[0] = new TerrainBlock(homeVertex,childrenStride,pTerrain,this);
        m_pChildren[1] = new TerrainBlock(homeVertex + childrenStride,childrenStride,pTerrain,this);
        m_pChildren[2] = new TerrainBlock(homeVertex + childrenStride * pTerrain->GetWidthVertices() + childrenStride,childrenStride,pTerrain,this);
        m_pChildren[3] = new TerrainBlock(homeVertex + childrenStride * pTerrain->GetWidthVertices(),childrenStride,pTerrain,this);
    }
    CalculateGeometry(pTerrain);
}

TerrainBlock::~TerrainBlock()
{
	s_Count--;
    m_pTriangleStrip = NULL;
    if (m_pChildren != NULL && 2 < m_Stride)
    {
        for (int i = 0; i < 4; i++)
        {
            delete m_pChildren[i];
            m_pChildren[i] = NULL;
        }
        delete[] m_pChildren;
    }
}

bool TerrainBlock::IsActive()
{
    return m_bIsActive;
}

void TerrainBlock::Tessellate(const double* pMatModelView,const double* pMatProjection,const int* pViewport,TriangleStrip* pTriangleStrips,int* pCountStrips,Terrain* pTerrain)
{
    Box boundingBox;
    float width = m_Stride * pTerrain->GetVertexSpacing();
    boundingBox.m_Min.x = pTerrain->m_pVertices[m_HomeIndex].x;
    boundingBox.m_Min.y = pTerrain->m_pVertices[m_HomeIndex].y;
    boundingBox.m_Min.z = m_MinElevation;
    boundingBox.m_Max.x = boundingBox.m_Min.x + width;
    boundingBox.m_Max.y = boundingBox.m_Min.y + width;
    boundingBox.m_Max.z = m_MaxElevation;

    if ((*pCountStrips < pTerrain->m_MaxNumberOfPrimitives) && pTerrain->CuboidInFrustum(boundingBox))
    {
        if (m_Stride == 2)
        {
            int offset;

            pTerrain->SetVertexStatus(m_HomeIndex,1);
            pTriangleStrips[*pCountStrips].m_pVertices[0] = m_HomeIndex;
            offset = m_HomeIndex + pTerrain->GetWidthVertices();
            pTerrain->SetVertexStatus(offset,1);
            pTriangleStrips[*pCountStrips].m_pVertices[1] = offset;
            offset = m_HomeIndex + 1;
            pTerrain->SetVertexStatus(offset,1);
            pTriangleStrips[*pCountStrips].m_pVertices[2] = offset;
            offset = m_HomeIndex + 1 + pTerrain->GetWidthVertices();
            pTerrain->SetVertexStatus(offset,1);
            pTriangleStrips[*pCountStrips].m_pVertices[3] = offset;
            offset = m_HomeIndex + 2;
            pTerrain->SetVertexStatus(offset,1);
            pTriangleStrips[*pCountStrips].m_pVertices[4] = offset;
            offset = m_HomeIndex + 2 + pTerrain->GetWidthVertices();
            pTerrain->SetVertexStatus(offset,1);
            pTriangleStrips[*pCountStrips].m_pVertices[5] = offset;
            pTriangleStrips[*pCountStrips].m_NumberOfVertices = 6;
            pTriangleStrips[*pCountStrips].m_bEnabled = true;
            *pCountStrips = *pCountStrips + 1;

            if (*pCountStrips < pTerrain->m_MaxNumberOfPrimitives)
            {
                offset = pTerrain->GetWidthVertices() + m_HomeIndex;
                pTerrain->SetVertexStatus(offset,1);
                pTriangleStrips[*pCountStrips].m_pVertices[0] = offset;
                offset = pTerrain->GetWidthVertices() + m_HomeIndex + pTerrain->GetWidthVertices();
                pTerrain->SetVertexStatus(offset,1);
                pTriangleStrips[*pCountStrips].m_pVertices[1] = offset;
                offset = pTerrain->GetWidthVertices() + m_HomeIndex + 1;
                pTerrain->SetVertexStatus(offset,1);
                pTriangleStrips[*pCountStrips].m_pVertices[2] = offset;
                offset = pTerrain->GetWidthVertices() + m_HomeIndex + 1 + pTerrain->GetWidthVertices();
                pTerrain->SetVertexStatus(offset,1);
                pTriangleStrips[*pCountStrips].m_pVertices[3] = offset;
                offset = pTerrain->GetWidthVertices() + m_HomeIndex + 2;
                pTerrain->SetVertexStatus(offset,1);
                pTriangleStrips[*pCountStrips].m_pVertices[4] = offset;
                offset = pTerrain->GetWidthVertices() + m_HomeIndex + 2 + pTerrain->GetWidthVertices();
                pTerrain->SetVertexStatus(offset,1);
                pTriangleStrips[*pCountStrips].m_pVertices[5] = offset;
                pTriangleStrips[*pCountStrips].m_NumberOfVertices = 6;
                pTriangleStrips[*pCountStrips].m_bEnabled = true;
                *pCountStrips = *pCountStrips + 1;
            }

            m_bIsActive = true;
        }
        else
        {
            if (pTerrain->m_MaximumVisibleBlockSize < m_Stride)
            {
                m_pChildren[0]->Tessellate(pMatModelView,pMatProjection,pViewport,pTriangleStrips,pCountStrips,pTerrain);
                m_pChildren[1]->Tessellate(pMatModelView,pMatProjection,pViewport,pTriangleStrips,pCountStrips,pTerrain);
                m_pChildren[2]->Tessellate(pMatModelView,pMatProjection,pViewport,pTriangleStrips,pCountStrips,pTerrain);
                m_pChildren[3]->Tessellate(pMatModelView,pMatProjection,pViewport,pTriangleStrips,pCountStrips,pTerrain);
                m_bIsActive = false;
                m_bChildrenActive = true;
            }
            else
            {
            // Check screen coordinates of center of each face of bounding box
                double screenTopX,screenTopY,screenTopZ,screenBottomX,screenBottomY,screenBottomZ;
                float halfWidth = (boundingBox.m_Max.x - boundingBox.m_Min.x) / 2;
                // calculate z half way up the BoundingBox
                float CenterZ = (boundingBox.m_Min.z + boundingBox.m_Max.z) * 0.5f;
                double screenDist;
                int tm = Settings::GetInstance()->GetTessellateMethod();

                float faceX, faceY, faceZ; //FIXME: use faceX/Y/Z throughout rest of method too.
                // bottom face
                faceX = boundingBox.m_Min.x+halfWidth;
                faceY = boundingBox.m_Min.y+halfWidth;
                if (tm == Settings::TM_NEW)
                {
                    faceZ = -halfWidth*m_BPlane_A - halfWidth*m_BPlane_B - m_BPlane_Max_D;
                }
                else
                {
                    faceZ = boundingBox.m_Min.z;
                }
                gluProject(faceX,faceY,faceZ,pMatModelView,pMatProjection,(GLint*)pViewport,&screenBottomX,&screenBottomY,&screenBottomZ);
                // top face
                //faceZ = boundingBox.m_Max.z;
                faceZ = -halfWidth*m_BPlane_A - halfWidth*m_BPlane_B - m_BPlane_Min_D;
                gluProject(faceX,faceY,faceZ,pMatModelView,pMatProjection,(GLint*)pViewport,&screenTopX,&screenTopY,&screenTopZ);
                if ((tm == Settings::TM_NEW) || (tm == Settings::TM_OLD_NEW))
                {
                    double deltaX, deltaY, deltaZ;
                    deltaX = screenTopX - screenBottomX;
                    deltaY = screenTopY - screenBottomY;
                    double zweight = Settings::GetInstance()->GetTessellateZWeight();
                    deltaZ = (screenTopZ - screenBottomZ) * zweight;
                    screenDist = sqrt(deltaX*deltaX + deltaY*deltaY + deltaZ*deltaZ);
                }
                else 
                {
                    double deltaX, deltaY, boxHeight;
                    double screenDistHorizontal, screenDistVertical, screenDistNew;
                    if (tm == Settings::TM_OLD_UPRIGHTONLY) 
                    {
                    // Disregard DistHorizontal: assume we're vertical
                        screenDistHorizontal = 0;
                    }
                    else
                    {
                        screenDistHorizontal = fabs(screenTopX - screenBottomX);
                        if (tm == Settings::TM_2D_ROLLONLY)
                        {
                            deltaX = screenTopX-screenBottomX;
                            deltaY = screenTopY-screenBottomY;
                            boxHeight = sqrt(deltaX*deltaX+deltaY*deltaY);
                        }
                    }
                    screenDistVertical = fabs(screenTopY - screenBottomY);

                    // four side faces
                    gluProject(boundingBox.m_Min.x+halfWidth,boundingBox.m_Min.y,CenterZ,pMatModelView,pMatProjection,(GLint*)pViewport,&screenBottomX,&screenBottomY,&screenBottomZ);
                    gluProject(boundingBox.m_Min.x+halfWidth,boundingBox.m_Max.y,CenterZ,pMatModelView,pMatProjection,(GLint*)pViewport,&screenTopX,&screenTopY,&screenTopZ);
                    screenDistNew = fabs(screenTopX - screenBottomX);
                    if (screenDistNew > screenDistHorizontal) screenDistHorizontal = screenDistNew;
                    if (tm != Settings::TM_OLD_UPRIGHTONLY) 
                    {
                        screenDistNew = fabs(screenTopY - screenBottomY);
                        if (screenDistNew > screenDistVertical) screenDistVertical = screenDistNew;
                    }

                    gluProject(boundingBox.m_Min.x,boundingBox.m_Min.y+halfWidth,CenterZ,pMatModelView,pMatProjection,(GLint*)pViewport,&screenBottomX,&screenBottomY,&screenBottomZ);
                    gluProject(boundingBox.m_Max.x,boundingBox.m_Min.y+halfWidth,CenterZ,pMatModelView,pMatProjection,(GLint*)pViewport,&screenTopX,&screenTopY,&screenTopZ);
                    screenDistNew = fabs(screenTopX - screenBottomX);
                    if (screenDistNew > screenDistHorizontal) screenDistHorizontal = screenDistNew;
                    if (tm != Settings::TM_OLD_UPRIGHTONLY) 
                    {
                        screenDistNew = fabs(screenTopY - screenBottomY);
                        if (screenDistNew > screenDistVertical) screenDistVertical = screenDistNew;
                    }

                    // Use the smaller of vertical and horizontal screen size to decide whether or not the block should be simplified.
                    screenDist = screenDistHorizontal < screenDistVertical ? screenDistHorizontal : screenDistVertical;
                    if (tm == Settings::TM_2D_ROLLONLY) 
                    {
                        if (boxHeight < screenDist) screenDist = boxHeight;
                    }
                }

                if (screenDist <= pTerrain->GetDetailThreshold())
                {
                // This block is simplified, so add its triangles to the list and stop recursing.
                    CreateTriangleStrip(pTriangleStrips,pCountStrips,pTerrain);
                    m_bIsActive = true;
                    m_bChildrenActive = false;
                }
                else
                {
                    m_pChildren[0]->Tessellate(pMatModelView,pMatProjection,pViewport,pTriangleStrips,pCountStrips,pTerrain);
                    m_pChildren[1]->Tessellate(pMatModelView,pMatProjection,pViewport,pTriangleStrips,pCountStrips,pTerrain);
                    m_pChildren[2]->Tessellate(pMatModelView,pMatProjection,pViewport,pTriangleStrips,pCountStrips,pTerrain);
                    m_pChildren[3]->Tessellate(pMatModelView,pMatProjection,pViewport,pTriangleStrips,pCountStrips,pTerrain);
                    m_bIsActive = false;
                    m_bChildrenActive = true;
                }
            }
        }
    }
    else
    {
        m_bIsActive = false;
        m_bChildrenActive = false;
    }
}

void TerrainBlock::CreateTriangleStrip(TriangleStrip* pTriangleStrips,int* pCount,Terrain* pTerrain)
{
    if (*pCount < pTerrain->m_MaxNumberOfPrimitives)
    {
        pTerrain->SetVertexStatus(m_HomeIndex,1);
        pTriangleStrips[*pCount].m_pVertices[0] = m_HomeIndex;
        int offset = m_HomeIndex + pTerrain->GetWidthVertices() * m_Stride;
        pTerrain->SetVertexStatus(offset,1);
        pTriangleStrips[*pCount].m_pVertices[1] = offset;
        offset = m_HomeIndex + m_Stride;
        pTerrain->SetVertexStatus(offset,1);
        pTriangleStrips[*pCount].m_pVertices[2] = offset;
        offset = m_HomeIndex + m_Stride + pTerrain->GetWidthVertices() * m_Stride;
        pTerrain->SetVertexStatus(offset,1);
        pTriangleStrips[*pCount].m_pVertices[3] = offset;
        pTriangleStrips[*pCount].m_bEnabled = true;
        pTriangleStrips[*pCount].m_NumberOfVertices = 4;

        m_pTriangleStrip = &pTriangleStrips[*pCount];
        *pCount = *pCount + 1;
    }
}

void TerrainBlock::EnableStrip(bool bEnabled)
{
    m_pTriangleStrip->m_bEnabled = false;
}

int TerrainBlock::GetStride()
{
    return m_Stride;
}

int TerrainBlock::GetHomeIndex()
{
    return m_HomeIndex;
}

void TerrainBlock::RepairCracks(Terrain* pTerrain,TriangleFan* pTriangleFans,int* pCountFans)
{
    if (2 < m_Stride)
    {
        if (m_bIsActive)
        {
            int halfStride = m_Stride / 2;
            int bottomLeft = m_HomeIndex + m_Stride * pTerrain->GetWidthVertices();
            int bottomRight = bottomLeft + m_Stride;
            int i,previousVertex;
            int v0;
            int numVertices = 0;

            bool bNeedToFix = false;
            for (i = m_HomeIndex + m_Stride - 1; m_HomeIndex < i && !bNeedToFix; i--)
                bNeedToFix = (pTerrain->GetVertexStatus(i) == 1);
            if (!bNeedToFix)
            {
                for (i = m_HomeIndex + pTerrain->GetWidthVertices(); i < m_HomeIndex + m_Stride * pTerrain->GetWidthVertices() && !bNeedToFix; i += pTerrain->GetWidthVertices())
                    bNeedToFix = (pTerrain->GetVertexStatus(i) == 1);
                if (!bNeedToFix)
                {
                    for (i = bottomLeft + 1; i < bottomRight && !bNeedToFix; i++)
                        bNeedToFix = (pTerrain->GetVertexStatus(i) == 1);
                    if (!bNeedToFix)
                    {
                        for (i = bottomRight - pTerrain->GetWidthVertices(); m_HomeIndex + m_Stride < i && !bNeedToFix; i -= pTerrain->GetWidthVertices())
                            bNeedToFix = (pTerrain->GetVertexStatus(i) == 1);
                    }
                }
            }

            if (bNeedToFix)
            {
                EnableStrip(false);
                v0 = m_HomeIndex + halfStride + halfStride * pTerrain->GetWidthVertices();
                assert(0 <= v0);
                assert(v0 < pTerrain->GetNumberOfVertices());
                pTriangleFans[*pCountFans].m_pVertices[0] = v0;
                numVertices = 0;
                for (i = m_HomeIndex + m_Stride; m_HomeIndex <= i; i--)
                {
                    assert(0 <= i);
                    assert(i < pTerrain->GetNumberOfVertices());
                    if (pTerrain->GetVertexStatus(i) == 1)
                    {
                        if (++numVertices == MAX_VERTICES_PER_FAN - 1)
                        {
                        // We have reached the maximum size for a fan, so start a new fan.
                            pTriangleFans[*pCountFans].m_NumberOfVertices = numVertices;
                            *pCountFans = *pCountFans + 1;
                            pTriangleFans[*pCountFans].m_pVertices[0] = v0;
                            pTriangleFans[*pCountFans].m_pVertices[1] = previousVertex;
                            numVertices = 2;
                        }
                        pTriangleFans[*pCountFans].m_pVertices[numVertices] = i;
                        previousVertex = i;
                    }
                }
                for (i = m_HomeIndex + pTerrain->GetWidthVertices(); i <= m_HomeIndex + m_Stride * pTerrain->GetWidthVertices(); i += pTerrain->GetWidthVertices())
                {
                    assert(0 <= i);
                    assert(i < pTerrain->GetNumberOfVertices());
                    if(pTerrain->GetVertexStatus(i) == 1)
                    {
                        if (++numVertices == MAX_VERTICES_PER_FAN - 1)
                        {
                        // We have reached the maximum size for a fan, so start a new fan.
                            pTriangleFans[*pCountFans].m_NumberOfVertices = numVertices;
                            *pCountFans = *pCountFans + 1;
                            pTriangleFans[*pCountFans].m_pVertices[0] = v0;
                            pTriangleFans[*pCountFans].m_pVertices[1] = previousVertex;
                            numVertices = 2;
                        }
                        pTriangleFans[*pCountFans].m_pVertices[numVertices] = i;
                        previousVertex = i;
                    }
                }
                for (i = bottomLeft; i <= bottomRight; i++)
                {
                    assert(0 <= i);
                    assert(i < pTerrain->GetNumberOfVertices());
                    if (pTerrain->GetVertexStatus(i) == 1)
                    {
                        if (++numVertices == MAX_VERTICES_PER_FAN - 1)
                        {
                        // We have reached the maximum size for a fan, so start a new fan.
                            pTriangleFans[*pCountFans].m_NumberOfVertices = numVertices;
                            *pCountFans = *pCountFans + 1;
                            pTriangleFans[*pCountFans].m_pVertices[0] = v0;
                            pTriangleFans[*pCountFans].m_pVertices[1] = previousVertex;
                            numVertices = 2;
                        }
                        pTriangleFans[*pCountFans].m_pVertices[numVertices] = i;
                        previousVertex = i;
                    }
                }
                for (i = bottomRight - pTerrain->GetWidthVertices(); m_HomeIndex + m_Stride <= i; i -= pTerrain->GetWidthVertices())
                {
                    assert(0 <= i);
                    assert(i < pTerrain->GetNumberOfVertices());
                    if(pTerrain->GetVertexStatus(i) == 1)
                    {
                        if (++numVertices == MAX_VERTICES_PER_FAN - 1)
                        {
                        // We have reached the maximum size for a fan, so start a new fan.
                            pTriangleFans[*pCountFans].m_NumberOfVertices = numVertices;
                            *pCountFans = *pCountFans + 1;
                            pTriangleFans[*pCountFans].m_pVertices[0] = v0;
                            pTriangleFans[*pCountFans].m_pVertices[1] = previousVertex;
                            numVertices = 2;
                        }
                        pTriangleFans[*pCountFans].m_pVertices[numVertices] = i;
                        previousVertex = i;
                    }
                }
                pTriangleFans[*pCountFans].m_NumberOfVertices = numVertices + 1;
                *pCountFans = *pCountFans + 1;
            }
        }
        else if (m_bChildrenActive)
        {
            m_pChildren[0]->RepairCracks(pTerrain,pTriangleFans,pCountFans);
            m_pChildren[1]->RepairCracks(pTerrain,pTriangleFans,pCountFans);
            m_pChildren[2]->RepairCracks(pTerrain,pTriangleFans,pCountFans);
            m_pChildren[3]->RepairCracks(pTerrain,pTriangleFans,pCountFans);
        }
    }
}

void TerrainBlock::CalculateGeometry(Terrain* pTerrain)
{
    // Calculate slope of best plane approximation of TerrainBlock
    //FIXME: (later, don't optimize too early) - decrease multiplies by adding first.
    //       or include e.g. the 0.5 in the scaling factor below.
    m_BPlane_A = 0.5 * pTerrain->GetElevation(m_HomeIndex)
    - 0.5 * pTerrain->GetElevation(m_HomeIndex + m_Stride)
    - 0.5 * pTerrain->GetElevation(m_HomeIndex + m_Stride + pTerrain->GetWidthVertices()*m_Stride)
    + 0.5 * pTerrain->GetElevation(m_HomeIndex + pTerrain->GetWidthVertices());
    m_BPlane_B = 0.5 * pTerrain->GetElevation(m_HomeIndex)
    + 0.5 * pTerrain->GetElevation(m_HomeIndex + m_Stride)
    - 0.5 * pTerrain->GetElevation(m_HomeIndex + m_Stride + pTerrain->GetWidthVertices()*m_Stride)
    - 0.5 * pTerrain->GetElevation(m_HomeIndex + pTerrain->GetWidthVertices());
    float VertexSpacing = pTerrain->GetVertexSpacing();
    float scalefactor = m_Stride * VertexSpacing;
    m_BPlane_A /= scalefactor;
    m_BPlane_B /= scalefactor;

    // Find this block's bounding box.
    m_MinElevation = pTerrain->GetElevation(m_HomeIndex);
    m_MaxElevation = pTerrain->GetElevation(m_HomeIndex);
    m_BPlane_Min_D = -pTerrain->GetElevation(m_HomeIndex); // top plane
    m_BPlane_Max_D = -pTerrain->GetElevation(m_HomeIndex); // bottom plane
    int WidthVertices = pTerrain->GetWidthVertices();
    for (int i = 0; i <= m_Stride; i++)
    {
        for (int j = 0; j <= m_Stride; j++)
        {
            float elevation = pTerrain->GetElevation(m_HomeIndex + i*WidthVertices + j);
            float planeD = - m_BPlane_A*(j*VertexSpacing) - m_BPlane_B*(i*VertexSpacing) - elevation;
            if (planeD < m_BPlane_Min_D)
                m_BPlane_Min_D = planeD;
            if (m_BPlane_Max_D < planeD)
                m_BPlane_Max_D = planeD;
            if (elevation < m_MinElevation)
                m_MinElevation = elevation;
            if (m_MaxElevation < elevation)
                m_MaxElevation = elevation;
        }
    }

/*    float width = m_Stride * pTerrain->GetVertexSpacing();
    m_BoundingBox.m_Min.x = pTerrain->m_pVertices[m_HomeIndex].x;
    m_BoundingBox.m_Min.y = pTerrain->m_pVertices[m_HomeIndex].y;
    m_BoundingBox.m_Min.z = m_MinElevation;
    m_BoundingBox.m_Max.x = m_BoundingBox.m_Min.x + width;
    m_BoundingBox.m_Max.y = m_BoundingBox.m_Min.y + width;
    m_BoundingBox.m_Max.z = m_MaxElevation;*/

#ifdef _USE_RAYTRACING_SUPPORT_
// Build triangles for ray intersection and collision detection.
    if (m_Stride == 2)
    {
        m_pTriangles = new Triangle[8];
        m_pTriangles[0].DefineFromPoints(pTerrain->m_pVertices[m_HomeIndex],pTerrain->m_pVertices[m_HomeIndex + pTerrain->GetWidthVertices()],pTerrain->m_pVertices[m_HomeIndex + 1]);
        m_pTriangles[1].DefineFromPoints(pTerrain->m_pVertices[m_HomeIndex + 1],pTerrain->m_pVertices[m_HomeIndex + pTerrain->GetWidthVertices()],pTerrain->m_pVertices[m_HomeIndex + pTerrain->GetWidthVertices() + 1]);
        m_pTriangles[2].DefineFromPoints(pTerrain->m_pVertices[m_HomeIndex + 1],pTerrain->m_pVertices[m_HomeIndex + 1 + pTerrain->GetWidthVertices()],pTerrain->m_pVertices[m_HomeIndex + 2]);
        m_pTriangles[3].DefineFromPoints(pTerrain->m_pVertices[m_HomeIndex + 2],pTerrain->m_pVertices[m_HomeIndex + 1 + pTerrain->GetWidthVertices()],pTerrain->m_pVertices[m_HomeIndex + 2 + pTerrain->GetWidthVertices()]);
        m_pTriangles[4].DefineFromPoints(pTerrain->m_pVertices[m_HomeIndex + pTerrain->GetWidthVertices()],pTerrain->m_pVertices[m_HomeIndex + pTerrain->GetWidthVertices() + pTerrain->GetWidthVertices()],pTerrain->m_pVertices[m_HomeIndex  + pTerrain->GetWidthVertices() + 1]);
        m_pTriangles[5].DefineFromPoints(pTerrain->m_pVertices[m_HomeIndex + pTerrain->GetWidthVertices() + 1],pTerrain->m_pVertices[m_HomeIndex + pTerrain->GetWidthVertices() + pTerrain->GetWidthVertices()],pTerrain->m_pVertices[m_HomeIndex  + pTerrain->GetWidthVertices() + pTerrain->GetWidthVertices() + 1]);
        m_pTriangles[6].DefineFromPoints(pTerrain->m_pVertices[m_HomeIndex + pTerrain->GetWidthVertices() + 1],pTerrain->m_pVertices[m_HomeIndex + pTerrain->GetWidthVertices() + 1 + pTerrain->GetWidthVertices()],pTerrain->m_pVertices[m_HomeIndex  + pTerrain->GetWidthVertices() + 2]);
        m_pTriangles[7].DefineFromPoints(pTerrain->m_pVertices[m_HomeIndex + pTerrain->GetWidthVertices() + 2],pTerrain->m_pVertices[m_HomeIndex + pTerrain->GetWidthVertices() + 1 + pTerrain->GetWidthVertices()],pTerrain->m_pVertices[m_HomeIndex  + pTerrain->GetWidthVertices() + 2 + pTerrain->GetWidthVertices()]);
    }
#endif
}

Terrain::Terrain(int widthVertices,int heightVertices,float vertexSpacing,int maxNumTriangles,bool bUseBorders)
{
    m_pCommonTexture = NULL;
    m_pTriangleStrips = NULL;
    m_pTriangleFans = NULL;
    m_pVertices = NULL;
    m_pVertexStatus = NULL;
    m_pRootBlock = NULL;
    m_pTextureMain = NULL;
    m_pTextureDetail = NULL;
	m_pNormals = NULL;
	m_pTextureFactory = NULL;
	m_pTextureSet = NULL;
    m_MaxNumberOfPrimitives = maxNumTriangles / 4;
    m_MaximumVisibleBlockSize = 8;
    m_OffsetX = 0.0f;
	m_OffsetY = 0.0f;
	m_LatticePositionX = 0;
	m_LatticePositionY = 0;
	float* pElevations = new float[widthVertices * heightVertices];
	for (int i = 0; i < widthVertices * heightVertices; i++)
		pElevations[i] = 0.0f;

	SetAllElevations(pElevations,widthVertices,heightVertices,vertexSpacing);
    Init(NULL,0,0,NULL,0,0,bUseBorders,0.0f,0.0f,0,0);

	delete[] pElevations;
}

Terrain::Terrain(const float* pElevations,int elevWidth,int elevHeight,const Uint8* pTextureImage,int textureWidth,int textureHeight,const Uint8* pDetailTextureImage,int detailWidth,int detailHeight,float vertexSpacing,float elevationScale,int maxNumTriangles,bool bUseBorders,float offsetX,float offsetY,int numTexturesX,int numTexturesY)
{
    m_pCommonTexture = NULL;
    m_pTriangleStrips = NULL;
    m_pTriangleFans = NULL;
    m_pVertices = NULL;
    m_pVertexStatus = NULL;
    m_pRootBlock = NULL;
    m_pTextureMain = NULL;
    m_pTextureDetail = NULL;
	m_pNormals = NULL;
	m_pTextureFactory = NULL;
	m_pTextureSet = NULL;
    m_MaxNumberOfPrimitives = maxNumTriangles / 4;
    m_MaximumVisibleBlockSize = 8;
    m_OffsetX = offsetX;
    m_OffsetY = offsetY;
	m_LatticePositionX = 0;
	m_LatticePositionY = 0;

	SetAllElevations(pElevations,elevWidth,elevHeight,vertexSpacing,elevationScale);
    Init(pTextureImage,textureWidth,textureHeight,pDetailTextureImage,detailWidth,detailHeight,bUseBorders,offsetX,offsetY,numTexturesX,numTexturesY);
}

Terrain::Terrain(const char* szElevationsFilename,const char* szTextureFilename,const char* szDetailTextureFilename,float vertexSpacing,float elevationScale,int maxNumTriangles,bool bUseBorders,float offsetX,float offsetY,int numTexturesX,int numTexturesY)
{
    m_pCommonTexture = NULL;
    m_pTriangleStrips = NULL;
    m_pTriangleFans = NULL;
    m_pVertices = NULL;
    m_pVertexStatus = NULL;
    m_pRootBlock = NULL;
    m_pTextureMain = NULL;
    m_pTextureDetail = NULL;
	m_pNormals = NULL;
	m_pTextureFactory = NULL;
	m_pTextureSet = NULL;
    m_MaxNumberOfPrimitives = maxNumTriangles / 4;
    m_MaximumVisibleBlockSize = 8;
    m_OffsetX = offsetX;
    m_OffsetY = offsetY;
	m_LatticePositionX = 0;
	m_LatticePositionY = 0;

	SetAllElevations(szElevationsFilename,vertexSpacing,elevationScale);

    // Load the texture data.
    int texWidth = 0,texHeight = 0;
    Uint8* pTextureImage = NULL;
    if (szTextureFilename != NULL)
    {
        LoadImage(szTextureFilename,texWidth,texHeight,&pTextureImage,false);
        if (texWidth == 0)
        {
            string msg("Failed to load texture image file '");
            msg += szTextureFilename;
            msg += "'; This means that the file was not found or it is not an image type that Demeter can read.";
            throw new DemeterException(msg);
        }
    }
    
    // Load the detail texture data.
    int detailWidth = 0;
	int detailHeight = 0;
    Uint8* pDetailImage = NULL;
    if (szDetailTextureFilename != NULL)
    {
        LoadImage(szDetailTextureFilename,detailWidth,detailHeight,&pDetailImage,false);
        if (detailWidth == 0)
        {
            string msg("Failed to load detail texture image file '");
            msg += szDetailTextureFilename;
            msg += "'; This means that the file was not found or it is not an image type that Demeter can read.";
            throw new DemeterException(msg);
        }
    }

    Init(pTextureImage,texWidth,texHeight,pDetailImage,detailWidth,detailHeight,bUseBorders,offsetX,offsetY,numTexturesX,numTexturesY);

    delete[] pDetailImage;
    delete[] pTextureImage;
}

Terrain::~Terrain()
{
	for (unsigned int i = 0; i < m_TextureCells.size();  i++)
	{
		delete m_TextureCells[i];
		m_TextureCells[i] = NULL;
	}
	m_TextureCells.clear();

	if (m_pTextureFactory)
	{
		for (int i = 0; i < m_NumberOfTextureTiles; i++)
		{
			m_pTextureFactory->SetTerrain(this);
			m_pTextureFactory->UnloadTexture(i);
		}
	}

    if (m_pCommonTexture)
        delete m_pCommonTexture;
    if (m_pTriangleStrips)
        delete[] m_pTriangleStrips;
    if (m_pTriangleFans)
        delete[] m_pTriangleFans;
    if (m_pVertices)
        delete[] m_pVertices;
    if (m_pVertexStatus)
        delete m_pVertexStatus;
    if (m_pRootBlock)
        delete m_pRootBlock;
    if (m_pTextureMain)
        delete[] m_pTextureMain;
    if (m_pTextureDetail)
        delete[] m_pTextureDetail;
	if (m_pNormals)
		delete[] m_pNormals;
	if (m_pTextureSet)
		delete m_pTextureSet;
}

void Terrain::Init(const Uint8* pTextureImage,int textureWidth,int textureHeight,const Uint8* pDetailTextureImage,int detailWidth,int detailHeight,bool bUseBorders,float offsetX,float offsetY,int numTexturesX,int numTexturesY)
{
    m_pTextureSet = new TextureSet;
    m_DetailThreshold = 10.0f;

    if (pTextureImage != NULL)
        SetTexture(pTextureImage,textureWidth,textureHeight,bUseBorders);
    if (pDetailTextureImage != NULL)
        SetCommonTexture(pDetailTextureImage,detailWidth,detailHeight);

    if (glActiveTextureARB_ptr == NULL && !Settings::GetInstance()->IsHeadless())
        LoadGLExtensions();

    if (0 < numTexturesX)
    {
        m_NumberOfTextureTilesWidth = numTexturesX;
        m_NumberOfTextureTilesHeight = numTexturesY;
		m_TextureTileWidth = GetWidth() / (float)numTexturesX;
		m_TextureTileHeight = GetHeight() / (float)numTexturesY;
        m_NumberOfTextureTiles = numTexturesX * numTexturesY;
        m_TileSize = (int)(numTexturesX * m_TextureTileWidth);
        if (pTextureImage == NULL)
        {
            for (int i = 0; i < m_NumberOfTextureTiles; i++)
                m_TextureCells.push_back(new TextureCell);

			GenerateTextureCoordinates();
        }
    }
}

void Terrain::SetAllElevations(const char* szElevationsFilename,float vertexSpacing,float elevationScale)
{
	bool useRaw = (strstr(szElevationsFilename, ".raw") || strstr(szElevationsFilename, ".RAW") );

#ifdef _USE_GDAL_
    bool useGDAL = (strstr(szElevationsFilename,".ddf") || strstr(szElevationsFilename,".DDF") || strstr(szElevationsFilename,".dem") || strstr(szElevationsFilename,".DEM"));
#endif

    char szFullFilename[MAX_FILENAME_LENGTH];
	if (strstr(szElevationsFilename,"\\") || strstr(szElevationsFilename,"/"))
		sprintf(szFullFilename,szElevationsFilename);
	else
		Settings::GetInstance()->PrependMediaPath(szElevationsFilename,szFullFilename);

    float* pImageData = NULL;
    int elevWidth;
    int elevHeight;
    int i;

#ifdef _USE_GDAL_
    if(useGDAL) 
    {
        GDALDataset *poDataset;
        GDALAllRegister();
        poDataset = (GDALDataset*)GDALOpen(szFullFilename,GA_ReadOnly);
        if (poDataset == NULL)
            m_Logfile  << "TERRAIN: Failed to load DEM data" << endl;
        m_Logfile  << "TERRAIN: Input size is " << poDataset->GetRasterXSize() << "x" << poDataset->GetRasterYSize() << "x" << poDataset->GetRasterCount() << endl;
        int dataWidth = poDataset->GetRasterXSize();
        int dataHeight = poDataset->GetRasterYSize();
        DimensionPowerOf2(poDataset->GetRasterXSize(),poDataset->GetRasterYSize(),elevWidth,elevHeight);
        m_Logfile  << "TERRAIN: Changing size to " << elevWidth << "," << elevHeight << endl;
        
        GDALRasterBand *poBand;
        int nBlockXSize, nBlockYSize;
        poBand = poDataset->GetRasterBand(1);
        poBand->GetBlockSize(&nBlockXSize,&nBlockYSize);

        pImageData = new float[elevWidth * elevHeight];
        
        for (i = 0; i < elevHeight * elevWidth; i++)
            pImageData[i] = 0.0f;

        for(i = 0; i < dataHeight; i++)
        {
            float *pafScanline;
            int nXSize = poBand->GetXSize();
            pafScanline = (float *) CPLMalloc(sizeof(float)*nXSize);
            poBand->RasterIO(GF_Read,0,i,nXSize,1,pafScanline,nXSize,1,GDT_Float32,0,0);
            int index = i * elevWidth;
            for (int j = 0; j < dataWidth; j++)
                pImageData[index + j] = pafScanline[j];
        }

        delete poDataset;
    }
	else if (useRaw)
	{
        Uint8* pPixels;
        int imageWidth,imageHeight;

        LoadRawImage(szFullFilename,imageWidth,imageHeight,&pPixels);
        pImageData = new float[imageWidth * imageHeight];
        for (int i = 0;i<imageHeight;i++)
            for (int j = 0; j < imageWidth; j++)
			{
				int pixelValue = (pPixels[i*imageHeight + j] << 8) + 
								 pPixels[i*imageHeight + j + 1];
                pImageData[i*imageHeight+j] = (float)pixelValue;
			}

	}
    else
    {
#endif
        // Load the elevation data.
        Uint8* pPixels;
        int imageWidth,imageHeight;
        LoadImage(szFullFilename,imageWidth,imageHeight,&pPixels);
        if (imageWidth == 0)
        {
            string msg("Failed to load elevations image file '");
            msg += szElevationsFilename;
            msg += "'; This means that the file was not found or it is not an image type that Demeter can read.";
            throw new DemeterException(msg);
        }
        int j,k;
        // Force the input data to be a power of 2 in width and height by zero-filling up to the next power of 2.
        DimensionPowerOf2(imageWidth,imageHeight,elevWidth,elevHeight);
        pImageData = new float[elevWidth * elevHeight];
	if (pImageData == 0) {
	     cout << "TERRAIN: out of memory allocating new terrain elevation map\n";
	     exit(1);
	}
        if (imageWidth != elevWidth || imageHeight != elevHeight)
        {
            m_Logfile  << "TERRAIN: WARNING! Input elevations file is not a power of 2 in width and height - forcing to power of 2 by zero filling! You should fix your input data!" << endl;
            int size = elevWidth * elevHeight;
            for (i = 0; i < size; i++)
                pImageData[i] = 0.0f;
        }
        int size = imageWidth * imageHeight * 3;
        int pitch = imageWidth * 3;
        for (i = 0,k = 0; i < size; i += pitch)
            for (j = 0; j < pitch; j += 3,k++)
			{
				int r = pPixels[i + j];
				int g = pPixels[i + j + 1];
				int b = pPixels[i + j + 2];
				int val = r << 16 | g << 8 | b;
                pImageData[k] = (float)val;
			}
        delete[] pPixels;
#ifdef _USE_GDAL_
    }
#endif
	SetAllElevations(pImageData,elevWidth,elevHeight,vertexSpacing,elevationScale);
    delete[] pImageData;
}

void Terrain::SetAllElevations(const float* pElevations,int elevWidth,int elevHeight,float vertexSpacing,float elevationScale)
{
    if (m_pVertices)
        delete[] m_pVertices;
    if (m_pVertexStatus)
        delete m_pVertexStatus;
    if (m_pRootBlock)
        delete m_pRootBlock;
	if (m_pNormals)
		delete[] m_pNormals;

	m_VertexSpacing = vertexSpacing;

	if (!IsPowerOf2(elevWidth) || !IsPowerOf2(elevHeight))
    {
        string msg("The elevation data is NOT a power of 2 in both width and height. Elevation data must be a power of 2 in both width and height.");
        throw new DemeterException(msg);
        m_pTriangleStrips = NULL;
        m_pTriangleFans = NULL;
        m_pVertices = NULL;
    }

    m_WidthVertices = elevWidth + 1; // Add 1 dummy pixel line to edge for block strides
    m_HeightVertices = elevHeight + 1;
    m_NumberOfVertices = m_WidthVertices * m_HeightVertices;
    m_pVertices = new Vector[m_WidthVertices * m_HeightVertices]; 
    int i,j;
    float x,y;
    y = 0.0f;
    m_MaxElevation = 0.0f;
    int end = elevWidth * elevHeight;
    for (i = 0,j = 0; i < end; i += elevWidth,y += m_VertexSpacing)
    {
        const float* pImageRow = pElevations + i;
        x = 0.0f;
        for (const float* pImagePixel = pImageRow; pImagePixel < pImageRow + elevWidth; pImagePixel++,j++,x += m_VertexSpacing)
        {
            m_pVertices[j].x = x + m_OffsetX;
            m_pVertices[j].y = y + m_OffsetY;
            m_pVertices[j].z = *pImagePixel * elevationScale;
            if (m_MaxElevation < m_pVertices[j].z)
                m_MaxElevation = m_pVertices[j].z;
        }
        // Account for dummy column on right edge
        m_pVertices[j].x = (m_WidthVertices - 1) * m_VertexSpacing + m_OffsetX;
        m_pVertices[j].y = y + m_OffsetY;
        m_pVertices[j].z = m_pVertices[j - 1].z;
        if (m_MaxElevation < m_pVertices[j].z)
            m_MaxElevation = m_pVertices[j].z;
        j++; 
        x += m_VertexSpacing;
    }

    x = 0.0f;
    for (i = m_NumberOfVertices - m_WidthVertices; i < m_NumberOfVertices; i++,x += m_VertexSpacing)
    {
        m_pVertices[i].x = x + m_OffsetX;
        m_pVertices[i].y = (m_HeightVertices - 1) * m_VertexSpacing + m_OffsetY;
        m_pVertices[i].z = m_pVertices[i - m_WidthVertices].z;
    }

    BuildBlocks();

	// Generate vertex normal arrays
	if (Settings::GetInstance()->UseNormals())
	{
		m_pNormals = new Vector[m_NumberOfVertices];
		assert(m_pNormals);
		const float delta = (M_PI * 2.0f) / 8.0f;
		for (i = 0; i < m_NumberOfVertices; i++)
		{
			int indexX,indexY;
			indexX = i % m_WidthVertices;
			indexY = i / m_WidthVertices;
			float vertexX,vertexY;
			vertexX = indexX * m_VertexSpacing;
			vertexY = indexY * m_VertexSpacing;
#define NOSLOWNORMAL
#ifdef SLOWNORMAL
			/*
			// This average over 8 nearby normals is extremely slow compared
			// to all the other terrain loading code.  First, the offset vector
			// calculation is ineffecient, since v is recomputed 8 times at
			// every vertex to get exactly the same set of vectors each time.
			// Next, GetNormal() is slow, so doing such an average is costly.
			// For now I've stripped out this computation, and substituted a
			// single normal evaluation at a slight (fixed) offset from each
			// vertex.  This seems to produe comparable results.  If better
			// normal matching needs to be done at the boundaries of terrain
			// tiles, this should be done in a separate computation that only
			// operates on vertices near the edges of the terrain tiles.
			// -MR
			*/
			Vector avgNormal;
			avgNormal.x = avgNormal.y = avgNormal.z = 0.0f;
			for (float theta = -0.5f * delta; theta < (M_PI * 2.0f); theta+=delta)
			{
				Vector v;
				v.x = 1.0f;
				v.y = 0.0f;
				v.z = 0.0f;
				v.RotateZ(theta);
				v.x += vertexX;
				v.y += vertexY;
				float nx, ny, nz;
				GetNormal(v.x,v.y,nx,ny,nz);
				avgNormal.x += nx;
				avgNormal.y += ny;
				avgNormal.z += nz;
			}
			m_pNormals[i].x = avgNormal.x / 8.0f;
			m_pNormals[i].y = avgNormal.y / 8.0f;
			m_pNormals[i].z = avgNormal.z / 8.0f;
#else
			float nx, ny, nz;
			GetNormal(vertexX+0.2,vertexY+0.8,nx,ny,nz);
			m_pNormals[i].x = nx;
			m_pNormals[i].y = ny;
			m_pNormals[i].z = nz;
#endif
		}
	}
}

bool Terrain::IsMultiTextureSupported()
{
    return bMultiTextureSupported;
}

int Terrain::GetNumberOfVertices()
{
    return m_NumberOfVertices;
}

void Terrain::UpdateNeighbor(Terrain* pTerrain,Terrain::DIRECTION direction)
{
    int thisVertex,otherVertex;
    if (direction == Terrain::DIR_SOUTH)
    {
        for (thisVertex = 0,otherVertex = m_NumberOfVertices - m_WidthVertices; thisVertex < m_WidthVertices; thisVertex++,otherVertex++)
        {
            if (GetVertexStatus(thisVertex))
                pTerrain->SetVertexStatus(otherVertex,true);
        }
    }
    else if (direction == Terrain::DIR_NORTH)
    {
        for (thisVertex = m_NumberOfVertices - m_WidthVertices,otherVertex = 0; thisVertex < m_NumberOfVertices; thisVertex++,otherVertex++)
        {
            if (GetVertexStatus(thisVertex))
                pTerrain->SetVertexStatus(otherVertex,true);
        }
    }
    else if (direction == Terrain::DIR_WEST)
    {
        for (thisVertex = 0,otherVertex = m_WidthVertices - 1; thisVertex < m_NumberOfVertices; thisVertex += m_WidthVertices,otherVertex += m_WidthVertices)
        {
            if (GetVertexStatus(thisVertex))
                pTerrain->SetVertexStatus(otherVertex,true);
        }
    }
    else if (direction == Terrain::DIR_EAST)
    {
        for (thisVertex = m_WidthVertices - 1,otherVertex = 0; thisVertex < m_NumberOfVertices; thisVertex += m_WidthVertices,otherVertex += m_WidthVertices)
        {
            if (GetVertexStatus(thisVertex))
                pTerrain->SetVertexStatus(otherVertex,true);
        }
    }
    else if (direction == Terrain::DIR_NORTHWEST)
    {
        if (GetVertexStatus(m_NumberOfVertices - m_WidthVertices))
            pTerrain->SetVertexStatus(m_WidthVertices - 1,true);
    }
    else if (direction == Terrain::DIR_NORTHEAST)
    {
        if (GetVertexStatus(m_NumberOfVertices - 1))
            pTerrain->SetVertexStatus(0,true);
    }
    else if (direction == Terrain::DIR_SOUTHEAST)
    {
        if (GetVertexStatus(m_WidthVertices - 1))
            pTerrain->SetVertexStatus(m_NumberOfVertices - m_WidthVertices,true);
    }
    else if (direction == Terrain::DIR_SOUTHWEST)
    {
        if (GetVertexStatus(0))
            pTerrain->SetVertexStatus(m_NumberOfVertices - 1,true);
    }
}

float Terrain::GetElevation(int index)
{
    return m_pVertices[index].z;
}

float Terrain::GetElevation(float x,float y)
{
    Plane plane;
    int vertexID;
    float elevation;

    x -= m_OffsetX;
    y -= m_OffsetY;

#ifdef _PROTECT_ACCESS_
    if (x < 0.0f || y < 0.0f || GetWidth() < x || GetHeight() < y)
        elevation = 0.0f;
    else
    {
#endif
        vertexID = ((int)(y / m_VertexSpacing)) * m_WidthVertices + ((int)(x / m_VertexSpacing));

        if ((fmod(y,m_VertexSpacing) + fmod(x,m_VertexSpacing)) <= m_VertexSpacing)
            plane.defineFromPoints(m_pVertices[vertexID],m_pVertices[vertexID + m_WidthVertices],m_pVertices[vertexID + 1]);
        else
            plane.defineFromPoints(m_pVertices[vertexID + 1],m_pVertices[vertexID + 1 + m_WidthVertices],m_pVertices[vertexID + m_WidthVertices]);

        elevation = -1.0f * ((plane.a * (x + m_OffsetX) + plane.b * (y + m_OffsetY) + plane.d) / plane.c);
#ifdef _PROTECT_ACCESS_
    }
#endif

    return elevation;
}

void Terrain::GetNormal(float x,float y,float& normalX,float& normalY,float& normalZ)
{
    Plane plane;
    int vertexID;
    
    x -= m_OffsetX;
    y -= m_OffsetY;
#ifdef _PROTECT_ACCESS_
    if (x < 0.0f || y < 0.0f || GetWidth() < x || GetHeight() < y)
    {
        normalX = normalY = 0.0f;
        normalZ = 1.0f;
    }
    else
    {
#endif
        vertexID = (int)(y / m_VertexSpacing) * m_WidthVertices + (int)(x / m_VertexSpacing);

        if ((fmod(y,m_VertexSpacing) - fmod(x,m_VertexSpacing)) >= 0.0f)
            plane.defineFromPoints(m_pVertices[vertexID],m_pVertices[vertexID + 1],m_pVertices[vertexID + m_WidthVertices]);
        else
            plane.defineFromPoints(m_pVertices[vertexID + 1],m_pVertices[vertexID + 1 + m_WidthVertices],m_pVertices[vertexID + m_WidthVertices]);

        normalX = plane.a;
        normalY = plane.b;
        normalZ = plane.c;
#ifdef _PROTECT_ACCESS_
    }
#endif
}

void Terrain::SetDetailThreshold(float threshold)
{
    m_DetailThreshold = threshold;
}

float Terrain::GetDetailThreshold()
{
    return m_DetailThreshold;
}

int Terrain::GetWidthVertices()
{
    return m_WidthVertices;
}

int Terrain::GetHeightVertices()
{
    return m_HeightVertices;
}

float Terrain::GetWidth() const
{
    return (float)(m_WidthVertices - 1) * m_VertexSpacing;
}

float Terrain::GetHeight() const
{
    return (float)(m_HeightVertices - 1) * m_VertexSpacing;
}

float Terrain::GetMaxElevation() const
{
    return m_MaxElevation;
}

void Terrain::SetTextureFactory(TextureFactory* pFactory)
{
    m_pTextureFactory = pFactory;
}

float Terrain::GetVertexElevation(int index) const
{
#ifdef _PROTECT_ACCESS_
    if (index < 0 || m_NumberOfVertices <= index)
        return 0.0f;
    else
#endif
        return m_pVertices[index].z;
}

void Terrain::SetVertexElevation(int index,float newElevation)
{
#ifdef _PROTECT_ACCESS_
    if (0 <= index && index < m_NumberOfVertices)
#endif
        m_pVertices[index].z = newElevation;
}

void Terrain::SetVertexElevation(float x,float y,float newElevation)
{
    double vx,vy;
    
    vx = fmod(x,m_VertexSpacing);
    vy = y / m_VertexSpacing;

    int index;

    if ((fmod(y,m_VertexSpacing) + fmod(x,m_VertexSpacing)) <= m_VertexSpacing)
        index = (int)vy * m_WidthVertices + (int)vx;
    else
        index = ((int)vy + 1) * m_WidthVertices + ((int)vx + 1);

    SetVertexElevation(index,newElevation);
}

float Terrain::GetVertexSpacing() const
{
    return m_VertexSpacing;
}

void Terrain::BuildBlocks()
{
    if (Settings::GetInstance()->IsHeadless())
        return;
    numLevels = 0.0f;
    numBlocks = 0.0f;
    for (int i = m_WidthVertices - 1; 2 <= i; i /= 2)
        numLevels += 1.0f;
    for (double j = 0.0f; j < numLevels; j += 1.0f)
        numBlocks += pow(4,j);
    if (Settings::GetInstance()->IsVerbose())
    {
        m_Logfile  << "TERRAIN: Building " << numBlocks << " blocks; please wait..." << endl;
#ifdef _USE_RAYTRACING_SUPPORT_
        m_Logfile  << "TERRAIN: Memory required at runtime for blocks = " << numBlocks * (sizeof(TerrainBlock) + 8 * sizeof(Triangle)) << " bytes" << endl;
#else
        m_Logfile  << "TERRAIN: Memory required at runtime for blocks = " << numBlocks * sizeof(TerrainBlock) << " bytes" << endl;
#endif
        m_Logfile  << ".............................." << endl;
        hashDelta = static_cast<float>(numBlocks / 30.0);
        m_Logfile  << "#" << flush;
    }
    m_pVertexStatus = new BitArray(m_WidthVertices * m_HeightVertices);
    // We assume that the terrain's width is always a power of 2 + 1!
    m_pRootBlock = new TerrainBlock(0,m_WidthVertices - 1,this,NULL);
    if (Settings::GetInstance()->IsVerbose())
        m_Logfile  << endl;
}

void Terrain::SetVertexStatus(int vertexIndex,bool status)
{
    if (status)
        m_pVertexStatus->SetBit(vertexIndex);
    else
        m_pVertexStatus->ClearBit(vertexIndex);
}

bool Terrain::GetVertexStatus(int vertexIndex)
{
    return m_pVertexStatus->IsBitSet(vertexIndex);
}

int Terrain::Tessellate()
{
    if (m_pTriangleStrips == NULL)
    {
        long int maxNumStrips = (GetWidthVertices() - 1) * (GetHeightVertices() - 1);
        try
        {
            if (m_MaxNumberOfPrimitives < maxNumStrips)
                maxNumStrips = m_MaxNumberOfPrimitives;
            if (Settings::GetInstance()->IsVerbose())
                m_Logfile << "TERRAIN: Allocating " << maxNumStrips << " triangle strips and fans (" << maxNumStrips * sizeof(TriangleStrip) + maxNumStrips * sizeof(TriangleFan) << " bytes)\n" << endl;
            m_pTriangleStrips = new TriangleStrip[maxNumStrips];
            m_pTriangleFans = new TriangleFan[maxNumStrips];
            if (m_pTriangleStrips == NULL || m_pTriangleFans == NULL)
            {
                m_Logfile << "TERRAIN: " << "Not enough memory to build terrain triangles" << endl;
                exit(1);
            }
        }
        catch(...)
        {
            m_Logfile << "TERRAIN: " << "Not enough memory to build terrain triangles" << endl;
            exit(1);
        }
    }

    double matModelview[16];
    double matProjection[16];
    GLint viewport[4];
    glGetDoublev(GL_MODELVIEW_MATRIX, matModelview);
    glGetDoublev(GL_PROJECTION_MATRIX, matProjection);
    glGetIntegerv(GL_VIEWPORT,viewport);

    ExtractFrustum();

    m_pVertexStatus->Clear();
    m_CountStrips = m_CountFans = 0;
    m_pRootBlock->Tessellate((double*)matModelview,(double*)matProjection,(int*)viewport,(TriangleStrip*)m_pTriangleStrips,&m_CountStrips,this);
    return m_CountStrips * 2 + m_CountFans * 6;
}

bool Terrain::SetCommonTexture(const Uint8* pBuffer,int width,int height)
{
    bool bSuccess = false;
    // Test to see if the image is a power of 2 in both width and height.
    if (!IsPowerOf2(width) || !IsPowerOf2(height))
    {
        string msg("The detail texture image file is NOT a power of 2 in both width and height.\nTexture files must be a power of 2 in both width and height.");
        throw new DemeterException(msg);
    }
    m_pCommonTexture = new Texture(pBuffer,width,height,width,0,false,Settings::GetInstance()->IsTextureCompression());
    bSuccess = true;
    if (Settings::GetInstance()->IsVerbose())
        m_Logfile << "TERRAIN: Common texture set successfully" << endl;
    return bSuccess;
}

bool Terrain::SetCommonTexture(const char* szFilename)
{
    bool bSuccess = false;
    char szFullFilename[MAX_FILENAME_LENGTH];
    Settings::GetInstance()->PrependMediaPath(szFilename,szFullFilename);
    if (!Settings::GetInstance()->IsCompilerOnly())
    {
        if (Settings::GetInstance()->IsVerbose())
                m_Logfile << "TERRAIN: Setting common texture to " << szFilename << endl;
        int width,height;
        Uint8* pBuffer;
        LoadImage(szFullFilename,width,height,&pBuffer);
        if (width == 0)
        {
            string msg("Failed to load detail texture image file '");
            msg += szFullFilename;
            msg += "'; This means that the file was not found or it is not an image type that Demeter can read.";
            throw new DemeterException(msg);
        }
        else
        {
            // Test to see if the image is a power of 2 in both width and height.
            if (!IsPowerOf2(width) || !IsPowerOf2(height))
            {
                string msg("The detail texture image file '");
                msg += szFullFilename;
                msg += "' is NOT a power of 2 in both width and height.\nTexture files must be a power of 2 in both width and height.";
                throw new DemeterException(msg);
            }
            m_pCommonTexture = new Texture(pBuffer,width,height,width,0,false,Settings::GetInstance()->IsTextureCompression());
            delete[] pBuffer;
            bSuccess = true;
            if (Settings::GetInstance()->IsVerbose())
                m_Logfile << "TERRAIN: Common texture set successfully" << endl;
        }
    }
    else
    {
        FILE* testFile = fopen(szFullFilename,"rb");
        if (testFile)
        {
            bSuccess = true;
            fclose(testFile);
        }
    }
    return bSuccess;
}

bool Terrain::SetTexture(const Uint8* pBuffer,int width,int height,bool bUseBorders)
{
    if (Settings::GetInstance()->IsHeadless())
        return true;
    bool bSuccess = false;
    // Test to see if the image is a power of 2 in both width and height.
    if (!IsPowerOf2(width) || !IsPowerOf2(height))
    {
        string msg("The texture is NOT a power of 2 in both width and height.\nTextures must be a power of 2 in both width and height.");
        throw new DemeterException(msg);
    }
    ChopTexture(pBuffer,width,height,256,bUseBorders);
    bSuccess = true;
    if (Settings::GetInstance()->IsVerbose())
        m_Logfile << "TERRAIN: Texture set successfully" << endl;
    FlipTexturesForMapping();
    if (Settings::GetInstance()->GetPreloadTextures())
        PreloadTextures();
    return bSuccess;
}

bool Terrain::SetTexture(const char* szFilename,bool bUseBorders)
{
    if (Settings::GetInstance()->IsHeadless())
        return true;
    bool bSuccess = false;
    char szFullFilename[MAX_FILENAME_LENGTH];
	if (strstr(szFilename,"\\") || strstr(szFilename,"/"))
		sprintf(szFullFilename,szFilename);
	else
	    Settings::GetInstance()->PrependMediaPath(szFilename,szFullFilename);

    if (!Settings::GetInstance()->IsCompilerOnly())
    {
        if (Settings::GetInstance()->IsVerbose())
            m_Logfile << "TERRAIN: Setting texture to '" << szFullFilename << "'" << endl;
        int width,height;
        Uint8* pBuffer;
        LoadImage(szFullFilename,width,height,&pBuffer);
        if (width == 0)
        {
            string msg("Failed to load texture image file '");
            msg += szFullFilename;
            msg += "'; This means that the file was not found or it is not an image type that Demeter can read.";
            throw new DemeterException(msg);
        }
        else
        {
			bSuccess = SetTexture(pBuffer,width,height,bUseBorders);
        }
    }
    else
    {
        FILE* fileTest = fopen(szFullFilename,"rb");
        if (fileTest)
        {
            bSuccess = true;
            fclose(fileTest);
        }
    }
    return bSuccess;
}

void Terrain::GenerateTextureCoordinates()
{
	if (m_pTextureMain)
		delete[] m_pTextureMain;
	if (m_pTextureDetail)
		delete[] m_pTextureDetail;
    m_pTextureMain = new float[m_NumberOfVertices * 2];
    m_pTextureDetail = new float[m_NumberOfVertices * 2];
    float u;
    float v = 0.0f;
    float uDetail = 0.0f;
    float vDetail = 0.0f;
    float verticesPerTileWidth = (float)(m_WidthVertices + m_NumberOfTextureTilesWidth - 1) / (float)m_NumberOfTextureTilesWidth;
    float verticesPerTileHeight = (float)(m_HeightVertices + m_NumberOfTextureTilesHeight - 1) / (float)m_NumberOfTextureTilesHeight;
    float deltaU = 1.0f / (verticesPerTileWidth - 1.0f);
    float deltaV = 1.0f / (verticesPerTileHeight - 1.0f);
    float deltaUDetail = deltaU * Settings::GetInstance()->GetDetailTextureRepeats();
    float deltaVDetail = deltaV * Settings::GetInstance()->GetDetailTextureRepeats();
    const float epsilon = 0.0000001f;
    int k = 0;
	int i;
    for (i = 0; i < m_NumberOfVertices; i += m_WidthVertices)
    {
        u = 0.0f;
        uDetail = 0.0f;
        for (int j = i; j < i + m_WidthVertices; j++)
        {
            m_pTextureMain[k] = u;
            m_pTextureMain[k + 1] = v;
            m_pTextureDetail[k] = uDetail;
            m_pTextureDetail[k + 1] = vDetail;
            k += 2;
            u += deltaU;
            uDetail += deltaUDetail;
            if (fabs(u - 1.0f) < epsilon)
            {
                u = 1.0f;
                deltaU *= -1.0f;
            }
            else if (fabs(u) < epsilon)
            {
                u = 0.0f;
                deltaU *= -1.0f;
            }
        }
        v += deltaV;
        vDetail += deltaVDetail;
        if (fabs(v - 1.0f) < epsilon)
        {
            v = 1.0f;
            deltaV *= -1.0f;
        }
        else if (fabs(v) < epsilon)
        {
            v = 0.0f;
            deltaV *= -1.0f;
        }
    }
}

void Terrain::ChopTexture(const Uint8* pImage,int width,int height,int tileSize,bool bUseBorders)
{
    // It is assumed that the image is in a 3-byte per pixel, RGB format, with no padding on the pixel rows
    m_NumberOfTextureTilesWidth = width / tileSize;
    m_NumberOfTextureTilesHeight = height / tileSize;
    m_NumberOfTextureTiles = (width / tileSize) * (height / tileSize);
    float verticesPerTileWidth = (float)(m_WidthVertices + m_NumberOfTextureTilesWidth - 1) / (float)m_NumberOfTextureTilesWidth;
    float verticesPerTileHeight = (float)(m_HeightVertices + m_NumberOfTextureTilesHeight - 1) / (float)m_NumberOfTextureTilesHeight;
    m_TextureTileWidth = (int)((verticesPerTileWidth - 1) * m_VertexSpacing);
    m_TextureTileHeight = (int)((verticesPerTileHeight - 1) * m_VertexSpacing);
    m_TileSize = tileSize;

	GenerateTextureCoordinates();

    if (bUseBorders)
    {
        Uint8* pBorderedImage = new Uint8[(width + 2) * (height + 2) * 3];

        int i;
        // Create a temporary version of the original texture that is surrounded by a 1-pixel border.
        for (i = 0; i < height; i++)
            for (int j = 0; j < width * 3; j++)
                pBorderedImage[(i + 1) * (width + 2) * 3 + j + 3] = pImage[i * width * 3 + j];
        // Create texture tiles by roaming across the bordered image.
        for (i = 0; i < height; i += m_TileSize)
        {
            for (int j = 0; j < width; j += m_TileSize)
            {
                Uint8* pTile = pBorderedImage + i * (width + 2) * 3 + j * 3;
                Texture* pTexture = new Texture(pTile,m_TileSize + 2,m_TileSize + 2,width + 2,1,true,Settings::GetInstance()->IsTextureCompression());
                TextureCell* pCell = new TextureCell();
                pCell->SetTexture(pTexture);
                m_TextureCells.push_back(pCell);
            }
        }
        delete[] pBorderedImage;
    }
    else
    {
        // Create texture tiles by roaming across the image.
        for (int i = 0; i < height; i += tileSize)
        {
            for (int j = 0; j < width; j += tileSize)
            {
                const Uint8* pTile = pImage + i * width * 3 + j * 3;
                Texture* pTexture = new Texture(pTile,tileSize,tileSize,width,0,true,Settings::GetInstance()->IsTextureCompression());
                TextureCell* pCell = new TextureCell();
                pCell->SetTexture(pTexture);
                m_TextureCells.push_back(pCell);
            }
        }
    }
}

void Terrain::SetMaximumVisibleBlockSize(int stride)
{
    m_MaximumVisibleBlockSize = stride;
}

int Terrain::ModelViewMatrixChanged()
{
    int count = Tessellate();
    m_pRootBlock->RepairCracks(this,m_pTriangleFans,&m_CountFans);
    return count;
}

void Terrain::Render()
{
    int i,j;

	m_TerrainPolygonsRendered = 0;

	// Force different rendering contexts to share all terrain textures.
#ifdef _WIN32
	HGLRC currentContext = wglGetCurrentContext();
	bool found = false;
	for (unsigned k  = 0; k < m_SharedContexts.size() && !found; k++)
		found = (m_SharedContexts[k] == currentContext);
	if (!found)
	{
		m_SharedContexts.push_back(currentContext);
		if (1 < m_SharedContexts.size())
			wglShareLists(m_SharedContexts[0],m_SharedContexts[m_SharedContexts.size() - 1]);
	}
#endif

    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glDisableClientState(GL_COLOR_ARRAY );
    glDisableClientState(GL_NORMAL_ARRAY );
    glDisableClientState(GL_TEXTURE_COORD_ARRAY );
    glDisableClientState(GL_EDGE_FLAG_ARRAY);
    glDisableClientState(GL_INDEX_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    for (i = 0; i < m_CountStrips; i++)
        m_pTriangleStrips[i].Setup(this);

    for (i = 0; i < m_CountFans; i++)
        m_pTriangleFans[i].Setup(this);

	if (m_pTextureMain != NULL)
	{
		glClientActiveTextureARB_ptr(GL_TEXTURE0_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2,GL_FLOAT,0,m_pTextureMain);
		glClientActiveTextureARB_ptr(GL_TEXTURE1_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2,GL_FLOAT,0,m_pTextureDetail);
	}
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3,GL_FLOAT,0,m_pVertices);
	if (Settings::GetInstance()->UseNormals())
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT,0,m_pNormals);
		float matDiff[4] = {1.0f,1.0f,1.0f,1.0f};
		glMaterialfv(GL_FRONT,GL_DIFFUSE,matDiff);
	}
	else
		glNormal3f(0.0f,0.0f,1.0f);

	if (glLockArraysEXT_ptr != NULL)
        glLockArraysEXT_ptr(0,m_NumberOfVertices);

    if (0 < m_NumberOfTextureTiles)
    {
        // Prevent state thrashing by rendering one texture at a time...
        for (i = 0; i < m_NumberOfTextureTiles; i++)
        {
            bool anyRendered = false;
            bool firstTime = true;
            TextureCell* pCell;
            if (m_pTextureFactory == NULL)
                pCell = m_TextureCells[i];
            // First pass - overall texture
            for (j = 0; j < m_CountStrips; j++)
            {
                if (m_pTriangleStrips[j].textureId == i)
                {
                    if (firstTime)
                    {
						if (Settings::GetInstance()->UseNormals())
	                       glColor4f(1.0f,1.0f,1.0f,1.0f);
						else
	                       glColor4f(0.0f,0.0f,0.0f,1.0f);
                        glActiveTextureARB_ptr(GL_TEXTURE1_ARB);
                        if (bMultiTextureSupported && m_pCommonTexture != NULL)
                        {
                            glEnable(GL_TEXTURE_2D);
                            glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
                            glTexEnvf(GL_TEXTURE_ENV,GL_COMBINE_RGB_EXT,GL_ADD);
                            GLuint cTexId = m_pCommonTexture->UploadTexture();
                            glBindTexture(GL_TEXTURE_2D,cTexId);
                        }
                        else
                            glDisable(GL_TEXTURE_2D);
                        glActiveTextureARB_ptr(GL_TEXTURE0_ARB);
                        glEnable(GL_TEXTURE_2D);
						if (Settings::GetInstance()->UseNormals())
	                        glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
						else
							glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
                        GLuint texId;
                        if (m_pTextureFactory != NULL)
                        {
                            int tileX = i % (int)m_NumberOfTextureTilesWidth;
                            int tileY = i / (int)m_NumberOfTextureTilesHeight;
                            float originX = tileX * m_TextureTileWidth;
                            float originY = tileY * m_TextureTileHeight;
                            Texture* pTexture = m_pTextureFactory->GetTexture(i,originX,originY,m_TextureTileWidth,m_TextureTileHeight);
                            if (!pTexture->IsBound())
                            {
                                if (tileX % 2 != 0)
                                    pTexture->FlipHorizontal();
                                if (tileY % 2 != 0)
                                    pTexture->FlipVertical();
                            }
                            texId = pTexture->UploadTexture();
                        }
                        else
                            texId = pCell->BindTexture();
                        glBindTexture(GL_TEXTURE_2D,texId);
                        firstTime = false;
                        anyRendered = true;
                    }
                    m_pTriangleStrips[j].Render(this);
                }
            }
            if (m_pTextureFactory == NULL && !firstTime && 0 < pCell->GetNumberOfDetails())
            {
                // Second and subsequent passes - multitexture of masks and details
                glDepthMask(GL_FALSE);
                glDepthFunc(GL_EQUAL);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
                glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
                for (int k = 0; k < pCell->GetNumberOfDetails(); k++)
                {
	                glColor4f(1.0f,1.0f,1.0f,1.0f);
                    glActiveTextureARB_ptr(GL_TEXTURE0_ARB);
                    glBindTexture(GL_TEXTURE_2D,pCell->BindMask(k));
                    glActiveTextureARB_ptr(GL_TEXTURE1_ARB);
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D,pCell->BindDetail(k));
                    for (j = 0; j < m_CountStrips; j++)
                    {
                        if (m_pTriangleStrips[j].textureId == i)
                        {
                            m_pTriangleStrips[j].Render(this);
                        }
                    }
                }
                glDepthMask(GL_TRUE);
                glDepthFunc(GL_LESS);
                glDisable(GL_BLEND);
            }
            firstTime = true;
            for (j = 0; j < m_CountFans; j++)
            {
                if (m_pTriangleFans[j].textureId == i)
                {
                    if (firstTime)
                    {
						if (Settings::GetInstance()->UseNormals())
	                       glColor4f(1.0f,1.0f,1.0f,1.0f);
						else
	                       glColor4f(0.0f,0.0f,0.0f,1.0f);
                        glActiveTextureARB_ptr(GL_TEXTURE1_ARB);
                        if (bMultiTextureSupported && m_pCommonTexture != NULL)
                        {
                            glEnable(GL_TEXTURE_2D);
                            glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
                            glTexEnvf(GL_TEXTURE_ENV,GL_COMBINE_RGB_EXT,GL_ADD);
                            GLuint cTexId = m_pCommonTexture->UploadTexture();
                            glBindTexture(GL_TEXTURE_2D,cTexId);
                        }
                        else
                            glDisable(GL_TEXTURE_2D);
                        glActiveTextureARB_ptr(GL_TEXTURE0_ARB);
                        glEnable(GL_TEXTURE_2D);
						if (Settings::GetInstance()->UseNormals())
	                        glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
						else
							glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
                        GLuint texId;
                        if (m_pTextureFactory != NULL)
                        {
                            int tileX = i % (int)m_NumberOfTextureTilesWidth;
                            int tileY = i / (int)m_NumberOfTextureTilesHeight;
                            float originX = tileX * m_TextureTileWidth;
                            float originY = tileY * m_TextureTileHeight;
                            Texture* pTexture = m_pTextureFactory->GetTexture(i,originX,originY,m_TextureTileWidth,m_TextureTileHeight);
                            if (!pTexture->IsBound())
                            {
                                if (tileX % 2 != 0)
                                    pTexture->FlipHorizontal();
                                if (tileY % 2 != 0)
                                    pTexture->FlipVertical();
                            }
                            texId = pTexture->UploadTexture();
                        }
                        else
                            texId = pCell->BindTexture();
                        glBindTexture(GL_TEXTURE_2D,texId);
                        firstTime = false;
                        anyRendered = true;
                    }
                    m_pTriangleFans[j].Render(this);
                }
            }
            if (m_pTextureFactory == NULL && !firstTime && 0 < pCell->GetNumberOfDetails())
            {
                glDepthMask(GL_FALSE);
                glDepthFunc(GL_EQUAL);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
                glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
                // Second and subsequent passes - multitexture of masks and details
                for (int k = 0; k < pCell->GetNumberOfDetails(); k++)
                {
                    glColor4f(1.0f,1.0f,1.0f,1.0f);
                    glActiveTextureARB_ptr(GL_TEXTURE0_ARB);
                    glBindTexture(GL_TEXTURE_2D,pCell->BindMask(k));
                    glActiveTextureARB_ptr(GL_TEXTURE1_ARB);
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D,pCell->BindDetail(k));
                    for (j = 0; j < m_CountFans; j++)
                    {
                        if (m_pTriangleFans[j].textureId == i)
                        {
                            m_pTriangleFans[j].Render(this);
                        }
                    }
                }
                glDepthMask(GL_TRUE);
                glDepthFunc(GL_LESS);
                glDisable(GL_BLEND);
            }
            if (Settings::GetInstance()->UseDynamicTextures() && !anyRendered)
            {
                if (m_pTextureFactory == NULL)
                    pCell->UnbindAll();
                else
                    m_pTextureFactory->UnloadTexture(i);
            }
        }
    }
    else
    {
        glColor4f(1.0f,1.0f,1.0f,1.0f);
        glActiveTextureARB_ptr(GL_TEXTURE0_ARB);
        glDisable(GL_TEXTURE_2D);
        glActiveTextureARB_ptr(GL_TEXTURE1_ARB);
        glDisable(GL_TEXTURE_2D);
        for (j = 0; j < m_CountStrips; j++)
            m_pTriangleStrips[j].Render(this);
        for (j = 0; j < m_CountFans; j++)
            m_pTriangleFans[j].Render(this);
    }
    
    if (glUnlockArraysEXT_ptr != NULL)
        glUnlockArraysEXT_ptr();
    glDisableClientState(GL_VERTEX_ARRAY);

    // Turn multi-texture back off again so the client application doesn't end up in an unexpected state.
    if (bMultiTextureSupported)
    {
        glClientActiveTextureARB_ptr(GL_TEXTURE1_ARB);
        glActiveTextureARB_ptr(GL_TEXTURE1_ARB);
        glDisable(GL_TEXTURE_2D);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glClientActiveTextureARB_ptr(GL_TEXTURE0_ARB);
        glActiveTextureARB_ptr(GL_TEXTURE0_ARB);
        glDisable(GL_TEXTURE_2D);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    glFrontFace(GL_CCW);

}

TriangleStrip::TriangleStrip()
{
	s_Count++;
}

TriangleStrip::~TriangleStrip()
{
	s_Count--;
}

void TriangleStrip::Setup(Terrain* pTerrain)
{
    if (m_bEnabled)
    {
        minX = pTerrain->m_pVertices[m_pVertices[0]].x;
        minY = pTerrain->m_pVertices[m_pVertices[0]].y;
        for (int i = 1; i < m_NumberOfVertices; i++)
        {
            if (pTerrain->m_pVertices[m_pVertices[i]].x < minX)
                minX = pTerrain->m_pVertices[m_pVertices[i]].x;
            if (pTerrain->m_pVertices[m_pVertices[i]].y < minY)
                minY = pTerrain->m_pVertices[m_pVertices[i]].y;
        }
        int tileY = (int)((minY - pTerrain->m_OffsetY) / pTerrain->GetTextureTileHeight());
        int tileX = (int)((minX - pTerrain->m_OffsetX) / pTerrain->GetTextureTileWidth());
        textureId = tileY * pTerrain->GetNumberOfTextureTilesWidth() + tileX;
    }
}

void TriangleStrip::Render(Terrain* pTerrain)
{

    if (m_bEnabled)
    {
        glDrawElements(GL_TRIANGLE_STRIP,m_NumberOfVertices,GL_UNSIGNED_INT,m_pVertices);
		m_TerrainPolygonsRendered += (m_NumberOfVertices-2);
    }

}

TriangleFan::TriangleFan()
{
	s_Count++;
}

TriangleFan::~TriangleFan()
{
	s_Count--;
}

void TriangleFan::Setup(Terrain* pTerrain)
{
    minX = pTerrain->m_pVertices[m_pVertices[0]].x;
    minY = pTerrain->m_pVertices[m_pVertices[0]].y;
    for (int i = 1; i < m_NumberOfVertices; i++)
    {
        if (pTerrain->m_pVertices[m_pVertices[i]].x < minX)
            minX = pTerrain->m_pVertices[m_pVertices[i]].x;
        if (pTerrain->m_pVertices[m_pVertices[i]].y < minY)
            minY = pTerrain->m_pVertices[m_pVertices[i]].y;
    }
    int tileY = (int)((minY - pTerrain->m_OffsetY) / pTerrain->GetTextureTileHeight());
    int tileX = (int)((minX - pTerrain->m_OffsetX) / pTerrain->GetTextureTileWidth());
    textureId = tileY * pTerrain->GetNumberOfTextureTilesWidth() + tileX;
}

void TriangleFan::Render(Terrain* pTerrain)
{
    glDrawElements(GL_TRIANGLE_FAN,m_NumberOfVertices,GL_UNSIGNED_INT,m_pVertices);
	m_TerrainPolygonsRendered += (m_NumberOfVertices-1);

}

float Terrain::GetTextureTileWidth()
{
    return m_TextureTileWidth;
}

float Terrain::GetTextureTileHeight()
{
    return m_TextureTileHeight;
}

int Terrain::GetNumberOfTextureTilesWidth()
{
    return m_NumberOfTextureTilesWidth;
}

int Terrain::GetNumberOfTextureTilesHeight()
{
    return m_NumberOfTextureTilesHeight;
}

void Terrain::Write(char* szFilename)
{
    char szFullFilename[MAX_FILENAME_LENGTH];
    Settings::GetInstance()->PrependMediaPath(szFilename,szFullFilename);
    FILE* file = fopen(szFullFilename,"wb");
    char szDemeter[] = "Demeter";
    fwrite(szDemeter,1,7,file);

    m_pTextureSet->Write(file,this);

    for (unsigned i = 0; i < m_TextureCells.size(); i++)
        m_TextureCells[i]->Write(file,this);
    fclose(file);
}

void Terrain::Read(char* szFilename)
{   
    char szFullFilename[MAX_FILENAME_LENGTH];
    Settings::GetInstance()->PrependMediaPath(szFilename,szFullFilename);
    FILE* file = fopen(szFullFilename,"rb");
    if (file)
    {
        char szDemeter[8];
        fread(szDemeter,1,7,file);
        szDemeter[7] = '\0';
        if (strcmp(szDemeter,"Demeter") != 0)
        {
            fclose(file);
            throw new DemeterException("The specified file is not a Demeter surface file");
        }
        
        if (m_pTextureSet)
            delete m_pTextureSet;
        m_pTextureSet = new TextureSet;
        m_pTextureSet->Read(file,this);
        for (int i = 0; i < m_NumberOfTextureTiles; i++)
            m_TextureCells[i]->Read(file,this);
        if (Settings::GetInstance()->GetPreloadTextures())
            PreloadTextures();
    }
    else
        throw new DemeterException("The specified file could not be opened");
}

void Terrain::FlipTexturesForMapping()
{
// Every odd texture has to be flipped because our texture coordinates vary from 0 to 1 back to 0 and so forth across the entire terrain. This eliminates seams between texture tiles.
    for (int textureY = 0; textureY < m_NumberOfTextureTilesHeight; textureY++)
    {   
        for (int textureX = 0; textureX < m_NumberOfTextureTilesWidth; textureX++)
        {
            TextureCell* pTextureCell = m_TextureCells[textureY * m_NumberOfTextureTilesWidth + textureX];
            if (textureX % 2 != 0)
                pTextureCell->FlipHorizontal();
            if (textureY % 2 != 0)
                pTextureCell->FlipVertical();
        }
    }
}

void Terrain::PreloadTextures()
{
    for (unsigned i = 0; i < m_TextureCells.size(); i++)
    {
        m_TextureCells[i]->BindTexture();
        for (int j = 0; j < m_TextureCells[i]->GetNumberOfDetails(); j++)
        {
            m_TextureCells[i]->BindMask(j);
            m_TextureCells[i]->BindDetail(j);
        }
    }
}

TextureCell* Terrain::GetTextureCell(int index)
{
    return m_TextureCells[index];
}
 
TextureCell* Terrain::GetTextureCell(int textureCellX,int textureCellY)
{
    return m_TextureCells[textureCellY * m_NumberOfTextureTilesWidth + textureCellX];
}

bool Terrain::Pick(int mouseX,int mouseY,float& pickedX,float& pickedY,float& pickedZ)
{
 // Thanks to Tero Kontkanen for providing this brilliant picking technique.
    bool bPickedTerrain = false;
    
    float depth[1];
    GLdouble modelm[16], projm[16], pos[3];
    int view[4];
    
    glGetDoublev( GL_MODELVIEW_MATRIX, modelm );
    glGetDoublev( GL_PROJECTION_MATRIX, projm );
    glGetIntegerv( GL_VIEWPORT, (GLint*)view );
    
    glReadPixels(mouseX,mouseY,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,depth);
    
    gluUnProject(mouseX,mouseY,depth[0],modelm,projm,(GLint*)view,&pos[0],&pos[1],&pos[2]);
 
    pickedX = static_cast<float>(pos[0]);
    pickedY = static_cast<float>(pos[1]);
    pickedZ = static_cast<float>(pos[2]);
 
    float elev = GetElevation(pickedX,pickedY);
    bPickedTerrain = Settings::GetInstance()->GetPickThreshold() < 0.0f || (pickedZ - elev < Settings::GetInstance()->GetPickThreshold()); // Look for a "fudge factor" difference between the unprojected point and the actual terrain elevation to see if some application object was picked instead of the terrain
    if (!bPickedTerrain)
        m_Logfile << "TERRAIN: Picked point is not on terrain *" << (pickedZ - elev) << ")" << endl;
    return bPickedTerrain;
 }
 
bool Terrain::Pick(int mouseX,int mouseY,float& pickedX,float& pickedY,float& pickedZ,int& textureCellX,int& textureCellY,float& texU,float& texV)
{
    bool bPickedTerrain = Pick(mouseX,mouseY,pickedX,pickedY,pickedZ);
    if (bPickedTerrain)
        GetTextureCoordinates(pickedX,pickedY,textureCellX,textureCellY,texU,texV);
    return bPickedTerrain;
}
 
void Terrain::GetTextureCoordinates(float x,float y,int& textureCellX,int& textureCellY,float& texU,float& texV)
{
    textureCellX = (int)(x / m_TextureTileWidth);
    textureCellY = (int)(y / m_TextureTileHeight);
    texU = static_cast<float>(fmod(x,m_TextureTileWidth) / m_TextureTileWidth);
    texV = static_cast<float>(fmod(y,m_TextureTileHeight) / m_TextureTileHeight);

    if (textureCellX % 2 != 0)
        texU = 1.0f - texU;
    if (textureCellY % 2 != 0)
        texV = 1.0f - texV;
}

void Terrain::ExtractFrustum()
{
// Thanks to Mark Morley for this OpenGL frustum testing code.
// See all of it at http://www.markmorley.com/opengl/frustumculling.html.
    float proj[16];
    float modl[16];
    float clip[16];
    double t;
    /* Get the current PROJECTION matrix from OpenGL */
    glGetFloatv(GL_PROJECTION_MATRIX, proj);
    /* Get the current MODELVIEW matrix from OpenGL */
    glGetFloatv(GL_MODELVIEW_MATRIX, modl);
    /* Combine the two matrices (multiply projection by modelview) */
    clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
    clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
    clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
    clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];
    clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
    clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
    clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
    clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];
    clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
    clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
    clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
    clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];
    clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
    clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
    clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
    clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];
    /* Extract the numbers for the RIGHT plane */
    m_Frustum[0][0] = clip[ 3] - clip[ 0];
    m_Frustum[0][1] = clip[ 7] - clip[ 4];
    m_Frustum[0][2] = clip[11] - clip[ 8];
    m_Frustum[0][3] = clip[15] - clip[12];
    /* Normalize the result */
    t = sqrt(m_Frustum[0][0] * m_Frustum[0][0] + m_Frustum[0][1] * m_Frustum[0][1] + m_Frustum[0][2] * m_Frustum[0][2]);
    m_Frustum[0][0] /= t;
    m_Frustum[0][1] /= t;
    m_Frustum[0][2] /= t;
    m_Frustum[0][3] /= t;
    /* Extract the numbers for the LEFT plane */
    m_Frustum[1][0] = clip[ 3] + clip[ 0];
    m_Frustum[1][1] = clip[ 7] + clip[ 4];
    m_Frustum[1][2] = clip[11] + clip[ 8];
    m_Frustum[1][3] = clip[15] + clip[12];
    /* Normalize the result */
    t = sqrt(m_Frustum[1][0] * m_Frustum[1][0] + m_Frustum[1][1] * m_Frustum[1][1] + m_Frustum[1][2] * m_Frustum[1][2]);
    m_Frustum[1][0] /= t;
    m_Frustum[1][1] /= t;
    m_Frustum[1][2] /= t;
    m_Frustum[1][3] /= t;
    /* Extract the BOTTOM plane */
    m_Frustum[2][0] = clip[ 3] + clip[ 1];
    m_Frustum[2][1] = clip[ 7] + clip[ 5];
    m_Frustum[2][2] = clip[11] + clip[ 9];
    m_Frustum[2][3] = clip[15] + clip[13];
    /* Normalize the result */
    t = sqrt(m_Frustum[2][0] * m_Frustum[2][0] + m_Frustum[2][1] * m_Frustum[2][1] + m_Frustum[2][2] * m_Frustum[2][2]);
    m_Frustum[2][0] /= t;
    m_Frustum[2][1] /= t;
    m_Frustum[2][2] /= t;
    m_Frustum[2][3] /= t;
    /* Extract the TOP plane */
    m_Frustum[3][0] = clip[ 3] - clip[ 1];
    m_Frustum[3][1] = clip[ 7] - clip[ 5];
    m_Frustum[3][2] = clip[11] - clip[ 9];
    m_Frustum[3][3] = clip[15] - clip[13];
    /* Normalize the result */
    t = sqrt(m_Frustum[3][0] * m_Frustum[3][0] + m_Frustum[3][1] * m_Frustum[3][1] + m_Frustum[3][2] * m_Frustum[3][2]);
    m_Frustum[3][0] /= t;
    m_Frustum[3][1] /= t;
    m_Frustum[3][2] /= t;
    m_Frustum[3][3] /= t;
    /* Extract the FAR plane */
    m_Frustum[4][0] = clip[ 3] - clip[ 2];
    m_Frustum[4][1] = clip[ 7] - clip[ 6];
    m_Frustum[4][2] = clip[11] - clip[10];
    m_Frustum[4][3] = clip[15] - clip[14];
    /* Normalize the result */
    t = sqrt(m_Frustum[4][0] * m_Frustum[4][0] + m_Frustum[4][1] * m_Frustum[4][1] + m_Frustum[4][2] * m_Frustum[4][2]);
    m_Frustum[4][0] /= t;
    m_Frustum[4][1] /= t;
    m_Frustum[4][2] /= t;
    m_Frustum[4][3] /= t;
    /* Extract the NEAR plane */
    m_Frustum[5][0] = clip[ 3] + clip[ 2];
    m_Frustum[5][1] = clip[ 7] + clip[ 6];
    m_Frustum[5][2] = clip[11] + clip[10];
    m_Frustum[5][3] = clip[15] + clip[14];
    /* Normalize the result */
    t = sqrt(m_Frustum[5][0] * m_Frustum[5][0] + m_Frustum[5][1] * m_Frustum[5][1] + m_Frustum[5][2] * m_Frustum[5][2]);
    m_Frustum[5][0] /= t;
    m_Frustum[5][1] /= t;
    m_Frustum[5][2] /= t;
    m_Frustum[5][3] /= t;
}

bool Terrain::CuboidInFrustum(const Box& cuboid)
{
// Thanks to Mark Morley for this OpenGL frustum testing code.
// See all of it at http://www.markmorley.com/opengl/frustumculling.html.
    for(int p = 0; p < 6; p++)
    {
        if(m_Frustum[p][0] * cuboid.m_Min.x + m_Frustum[p][1] * cuboid.m_Min.y + m_Frustum[p][2] * cuboid.m_Min.z + m_Frustum[p][3] > 0)
            continue;
        if(m_Frustum[p][0] * cuboid.m_Max.x + m_Frustum[p][1] * cuboid.m_Min.y + m_Frustum[p][2] * cuboid.m_Min.z + m_Frustum[p][3] > 0)
            continue;
        if(m_Frustum[p][0] * cuboid.m_Min.x + m_Frustum[p][1] * cuboid.m_Max.y + m_Frustum[p][2] * cuboid.m_Min.z + m_Frustum[p][3] > 0)
            continue;
        if(m_Frustum[p][0] * cuboid.m_Max.x + m_Frustum[p][1] * cuboid.m_Max.y + m_Frustum[p][2] * cuboid.m_Min.z + m_Frustum[p][3] > 0)
            continue;
        if(m_Frustum[p][0] * cuboid.m_Min.x + m_Frustum[p][1] * cuboid.m_Min.y + m_Frustum[p][2] * cuboid.m_Max.z + m_Frustum[p][3] > 0)
            continue;
        if(m_Frustum[p][0] * cuboid.m_Max.x + m_Frustum[p][1] * cuboid.m_Min.y + m_Frustum[p][2] * cuboid.m_Max.z + m_Frustum[p][3] > 0)
            continue;
        if(m_Frustum[p][0] * cuboid.m_Min.x + m_Frustum[p][1] * cuboid.m_Max.y + m_Frustum[p][2] * cuboid.m_Max.z + m_Frustum[p][3] > 0)
            continue;
        if(m_Frustum[p][0] * cuboid.m_Max.x + m_Frustum[p][1] * cuboid.m_Max.y + m_Frustum[p][2] * cuboid.m_Max.z + m_Frustum[p][3] > 0)
            continue;
        return false;
    }
    return true;
}

#ifdef _USE_RAYTRACING_SUPPORT_
float Terrain::IntersectRay(float startX,float startY,float startZ,float dirX,float dirY,float dirZ,float& intersectX,float& intersectY,float& intersectZ)
{
    Ray ray;
    float distance = DEMETER_INFINITY;
    Vector point;
    point.x = point.y = point.z = -1.0f;

    ray.m_Origin.x = startX;
    ray.m_Origin.y = startY;
    ray.m_Origin.z = startZ;
    ray.m_Direction.x = dirX;
    ray.m_Direction.y = dirY;
    ray.m_Direction.z = dirZ;
    ray.m_Direction.Normalize();
    m_pRootBlock->IntersectRay(ray,point,distance,this);
    intersectX = point.x;
    intersectY = point.y;
    intersectZ = point.z;
    return distance;
}

float Terrain::IntersectRay(float startX,float startY,float startZ,float dirX,float dirY,float dirZ,float& intersectX,float& intersectY,float& intersectZ,int& textureCellX,int& textureCellY,float& texU,float& texV)
{
    float dist = IntersectRay(startX,startY,startZ,dirX,dirY,dirZ,intersectX,intersectY,intersectZ);
    if (0.0f <= intersectX)
        GetTextureCoordinates(intersectX,intersectY,textureCellX,textureCellY,texU,texV);
    return dist;
}
#endif

void Terrain::SetTextureCell(int index,TextureCell* pCell)
{
    m_TextureCells[index] = pCell;
}

TextureSet* Terrain::GetTextureSet()
{
    return m_pTextureSet;
}

void Terrain::SetTextureSet(TextureSet* pSet)
{
    if (m_pTextureSet)
        delete m_pTextureSet;
    m_pTextureSet = pSet;
}

void Terrain::SetLatticePosition(int x,int y)
{
    m_LatticePositionX = x;
    m_LatticePositionY = y;
}

void Terrain::GetLatticePosition(int& x,int& y) const 
{
    x = m_LatticePositionX;
    y = m_LatticePositionY;
}

#ifdef _USE_RAYTRACING_SUPPORT_
void TerrainBlock::IntersectRay(const Ray& ray,Vector& intersectionPoint,float& lowestDistance,const Terrain* pTerrain)
{
// First test ray against this block's bounding box.
    Box boundingBox;
    float width = m_Stride * pTerrain->GetVertexSpacing();
    boundingBox.m_Min.x = pTerrain->m_pVertices[m_HomeIndex].x;
    boundingBox.m_Min.y = pTerrain->m_pVertices[m_HomeIndex].y;
    boundingBox.m_Min.z = m_MinElevation;
    boundingBox.m_Max.x = boundingBox.m_Min.x + width;
    boundingBox.m_Max.y = boundingBox.m_Min.y + width;
    boundingBox.m_Max.z = m_MaxElevation;
    if (RayBoxIntersect(&ray,&boundingBox,NULL,NULL))
    {
        if (2 < m_Stride)
        {
            m_pChildren[0]->IntersectRay(ray,intersectionPoint,lowestDistance,pTerrain);
            m_pChildren[1]->IntersectRay(ray,intersectionPoint,lowestDistance,pTerrain);
            m_pChildren[2]->IntersectRay(ray,intersectionPoint,lowestDistance,pTerrain);
            m_pChildren[3]->IntersectRay(ray,intersectionPoint,lowestDistance,pTerrain);
        }
        else
        {
            float distance;
            Vector point;
            for (int i = 0; i < 8; i++)
            {
                if (RayPlaneIntersect(&ray,m_pTriangles[i].GetPlane(),&point,&distance) == 1)
                {
                    if (i == 0 || i == 2 || i == 4 || i == 6)
                    {
                        if (m_pTriangles[i].GetVertex(0)->x <= point.x &&
                            m_pTriangles[i].GetVertex(0)->y <= point.y &&
                            point.x <= m_pTriangles[i].GetVertex(2)->x &&
                            point.y <= m_pTriangles[i].GetVertex(1)->y &&
                            ((fmod(point.y,pTerrain->m_VertexSpacing) + fmod(point.x,pTerrain->m_VertexSpacing)) <= pTerrain->m_VertexSpacing))
                        {
                            if (distance < lowestDistance)
                            {
                                lowestDistance = distance;
                                intersectionPoint.x = point.x;
                                intersectionPoint.y = point.y;
                                intersectionPoint.z = point.z;
                            }
                        }
                    }
                    else
                    {
                        if (m_pTriangles[i].GetVertex(1)->x <= point.x &&
                            m_pTriangles[i].GetVertex(0)->y <= point.y &&
                            point.x <= m_pTriangles[i].GetVertex(2)->x &&
                            point.y <= m_pTriangles[i].GetVertex(1)->y &&
                            ((fmod(point.y,pTerrain->m_VertexSpacing) + fmod(point.x,pTerrain->m_VertexSpacing)) >= pTerrain->m_VertexSpacing))
                        {
                            if (distance < lowestDistance)
                            {
                                lowestDistance = distance;
                                intersectionPoint.x = point.x;
                                intersectionPoint.y = point.y;
                                intersectionPoint.z = point.z;
                            }
                        }
                    }
                }
            }
        }
    }
}

int RayPlaneIntersect(const Ray *ray,const Plane *plane,Vector* point,float *distance)
{
    float           vd,vo,PnDOTRo,t;

    vd = plane->a * ray->m_Direction.x + plane->b * ray->m_Direction.y + plane->c * ray->m_Direction.z;
    if (vd == 0.0)
        // The plane is parallel to the ray. I've never seen this happen but someday it will . . .
        return -1;
    if (vd > 0.0)
    {
        // The plane is facing away from the ray so no intersection occurs.
        return -2;
    }
    PnDOTRo = plane->a * ray->m_Origin.x + plane->b * ray->m_Origin.y + plane->c * ray->m_Origin.z;
    vo = -1.0f * (PnDOTRo + plane->d);
    t = vo / vd;
    if (t < 0.0f)
        // The intersection occurs behind the ray's origin.
        return -3;
    point->x = ray->m_Origin.x + ray->m_Direction.x * t;
    point->y = ray->m_Origin.y + ray->m_Direction.y * t;
    point->z = ray->m_Origin.z + ray->m_Direction.z * t;
    if (distance != NULL)
        *distance = t;
    return 1;
}

int RayBoxIntersect(const Ray *ray,const Box *box,Vector *point,float *distance)
{
    float       tnear,tfar,t1,t2;

    tnear = -DEMETER_INFINITY;
    tfar = DEMETER_INFINITY;

// Find intersection with x-aligned planes of box.
    // If the ray is parallel to the box and not within the planes of the box it misses.
    if (ray->m_Direction.x == 0.0)
        if ((ray->m_Origin.x < box->m_Min.x) && (ray->m_Origin.x > box->m_Max.x))
            return 0;
    // Calculate intersection distance with the box's planes.
    t1 = (box->m_Min.x - ray->m_Origin.x) / ray->m_Direction.x;
    t2 = (box->m_Max.x - ray->m_Origin.x) / ray->m_Direction.x;
    if (t1 > t2)
    {
        float tmp = t1;
        t1 = t2;
        t2 = tmp;
    }
    if (t1 > tnear)
        tnear = t1;
    if (t2 < tfar)
        tfar = t2;
    if (tnear > tfar)
        return 0;
    if (tfar < 0.0)
        return 0;
// Find intersection with y-aligned planes of box.
    // If the ray is parallel to the box and not within the planes of the box it misses.
    if (ray->m_Direction.y == 0.0)
        if ((ray->m_Origin.y < box->m_Min.y) && (ray->m_Origin.y > box->m_Max.y))
            return 0;
    // Calculate intersection distance with the box's planes.
    t1 = (box->m_Min.y - ray->m_Origin.y) / ray->m_Direction.y;
    t2 = (box->m_Max.y - ray->m_Origin.y) / ray->m_Direction.y;
    if (t1 > t2)
    {
        float tmp = t1;
        t1 = t2;
        t2 = tmp;
    }
    if (t1 > tnear)
        tnear = t1;
    if (t2 < tfar)
        tfar = t2;
    if (tnear > tfar)
        return 0;
    if (tfar < 0.0)
        return 0;
// Find intersection with z-aligned planes of box.
    // If the ray is parallel to the box and not within the planes of the box it misses.
    if (ray->m_Direction.z == 0.0)
        if ((ray->m_Origin.z < box->m_Min.z) && (ray->m_Origin.z > box->m_Max.z))
            return 0;
    // Calculate intersection distance with the box's planes.
    t1 = (box->m_Min.z - ray->m_Origin.z) / ray->m_Direction.z;
    t2 = (box->m_Max.z - ray->m_Origin.z) / ray->m_Direction.z;
    if (t1 > t2)
    {
        float tmp = t1;
        t1 = t2;
        t2 = tmp;
    }
    if (t1 > tnear)
        tnear = t1;
    if (t2 < tfar)
        tfar = t2;
    if (tnear > tfar)
        return 0;
    if (tfar < 0.0)
        return 0;
// If we survived all of the tests, the box is hit.
    if (point != NULL)
    {
        point->x = ray->m_Origin.x + tnear * ray->m_Direction.x;
        point->y = ray->m_Origin.y + tnear * ray->m_Direction.y;
        point->z = ray->m_Origin.z + tnear * ray->m_Direction.z;
        *distance = tnear;
    }
    return 1;
}
#endif

void LoadImage(const char* szShortFilename,int& width,int &height,Uint8** ppBuffer,bool bAlpha)
{
    SDL_Init(SDL_INIT_VIDEO);
    char szFullFilename[MAX_FILENAME_LENGTH];
	if (strstr(szShortFilename,"\\") || strstr(szShortFilename,"/"))
		sprintf(szFullFilename,szShortFilename);
	else
		Settings::GetInstance()->PrependMediaPath(szShortFilename,szFullFilename);

	m_Logfile  << "TERRAIN: Loading texture " << szFullFilename << endl;
	SDL_Surface* pImage = IMG_Load(szFullFilename);
    if (pImage != NULL)
    {
        width = pImage->w;
        height = pImage->h;
        Uint8* pBufferTemp;
        if (bAlpha)
            pBufferTemp = new Uint8[width * height * 4];
        else
            pBufferTemp = new Uint8[width * height * 3];
        int i,j;
        Uint8* pImagePixels = (Uint8*)pImage->pixels;
        for (i = 0,j = 0; i < pImage->h * pImage->pitch; i += pImage->pitch)
        {
            Uint8* pImageRow = pImagePixels + i;
            for (Uint8* pImagePixel = pImageRow; pImagePixel < pImageRow + pImage->w * pImage->format->BytesPerPixel; pImagePixel += pImage->format->BytesPerPixel)
            {
                Uint8 red,green,blue,alpha;
                // Read the pixel into a 32-bit dword for use by SDL_GetRGBA
                Uint8 pPixel[4];
                for (int i = 0; i < pImage->format->BytesPerPixel; i++)
                    pPixel[i] = pImagePixel[i];
                Uint32* pCurrentPixel = (Uint32*)pPixel;
                SDL_GetRGBA(*pCurrentPixel,pImage->format,&red,&green,&blue,&alpha);
                if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
                {
                    Uint8* p = (Uint8*)pCurrentPixel;
                    Uint32 currentPixelConverted = p[0] << 16 | p[1] << 8 | p[2];
                    SDL_GetRGBA(currentPixelConverted, pImage->format, &red, &green, &blue, &alpha);
                }
                
                pBufferTemp[j++] = red;
                pBufferTemp[j++] = green;
                pBufferTemp[j++] = blue;
                if (bAlpha)
                {
                    pBufferTemp[j++] = alpha;
                }
            }
        }
        *ppBuffer = pBufferTemp;
        SDL_FreeSurface(pImage);
    }
    else
    {
        width = 0;
        height = 0;
        *ppBuffer = NULL;
    }
}


void LoadRawImage(const char* szShortFilename,int& width,int &height,Uint8** ppBuffer,bool bAlpha)
{
    char szFullFilename[MAX_FILENAME_LENGTH];
	if (strstr(szShortFilename,"\\") || strstr(szShortFilename,"/"))
		sprintf(szFullFilename,szShortFilename);
	else
		Settings::GetInstance()->PrependMediaPath(szShortFilename,szFullFilename);

	
	int iFileLength, iWidth, iHeight;
	float fWidth;
	int iSuccess;

	long iSourceFileHandle;

	unsigned char * buffer = NULL;

	iSourceFileHandle = _open(szFullFilename, _O_BINARY);
	iFileLength = _filelength(iSourceFileHandle);
	
	fWidth = (float)sqrt((float)iFileLength/2);			// compute terrain width from size of file
	iWidth = (int)fWidth;
	iHeight = (int)fWidth;
    
	buffer = new unsigned char[iFileLength];

	iSuccess = _read(iSourceFileHandle, buffer, iFileLength);
	_close(iSourceFileHandle);
	
/*
	if (pImage != NULL)
    {
        width = pImage->w;
        height = pImage->h;
        Uint8* pBufferTemp;
        if (bAlpha)
            pBufferTemp = new Uint8[width * height * 4];
        else
            pBufferTemp = new Uint8[width * height * 3];
        int i,j;
        Uint8* pImagePixels = (Uint8*)pImage->pixels;
        for (i = 0,j = 0; i < pImage->h * pImage->pitch; i += pImage->pitch)
        {
            Uint8* pImageRow = pImagePixels + i;
            for (Uint8* pImagePixel = pImageRow; pImagePixel < pImageRow + pImage->w * pImage->format->BytesPerPixel; pImagePixel += pImage->format->BytesPerPixel)
            {
                Uint8 red,green,blue,alpha;
                // Read the pixel into a 32-bit dword for use by SDL_GetRGBA
                Uint8 pPixel[4];
                for (int i = 0; i < pImage->format->BytesPerPixel; i++)
                    pPixel[i] = pImagePixel[i];
                Uint32* pCurrentPixel = (Uint32*)pPixel;
                SDL_GetRGBA(*pCurrentPixel,pImage->format,&red,&green,&blue,&alpha);
                if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
                {
                    Uint8* p = (Uint8*)pCurrentPixel;
                    Uint32 currentPixelConverted = p[0] << 16 | p[1] << 8 | p[2];
                    SDL_GetRGBA(currentPixelConverted, pImage->format, &red, &green, &blue, &alpha);
                }
                
                pBufferTemp[j++] = red;
                pBufferTemp[j++] = green;
                pBufferTemp[j++] = blue;
                if (bAlpha)
                {
                    pBufferTemp[j++] = alpha;
                }
            }
        }
        *ppBuffer = pBufferTemp;
        SDL_FreeSurface(pImage);
    }

    else
    {
        width = 0;
        height = 0;
        *ppBuffer = NULL;
    }

      */

	delete [] buffer;

}


GLuint CreateTexture(const Uint8* pTexels,int width,int height,int rowLength,int border,int internalFormat,bool bClamp,bool bAlpha)
{
    GLuint texId;
    glGenTextures(1,&texId);
    glBindTexture(GL_TEXTURE_2D,texId);
    if (bClamp)
    {
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    }
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH,rowLength);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    if (bAlpha)
    {
        glTexImage2D(GL_TEXTURE_2D,0,internalFormat,width,height,border,GL_RGBA,GL_UNSIGNED_BYTE,pTexels);
        gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA,width,height,GL_RGBA,GL_UNSIGNED_BYTE,pTexels);
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D,0,internalFormat,width,height,border,GL_RGB,GL_UNSIGNED_BYTE,pTexels);
        gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGB,width,height,GL_RGB,GL_UNSIGNED_BYTE,pTexels);
    }
    glPixelStorei(GL_UNPACK_ROW_LENGTH,0);
    return texId;
}

bool IsPowerOf2(double number)
{
    const int MAX_POWER = 1024;
    bool isPowerOf2 = false;
    for(int i = 0; (i < MAX_POWER) && (!isPowerOf2); i++)
    {
       if (pow(2,(double)i) == number)
            isPowerOf2 = true;
    }
    return isPowerOf2;
}

Settings::Settings()
{
    m_szMediaPath = NULL;
    m_bVerbose = false;
    m_bIsCompilerOnly = false;
    m_IsHeadless = false;
    m_UseDynamicTextures = false;
    m_IsEditor = false;
    m_TessellateMethod = Settings::TM_NEW;
    m_TessellateZWeight = 200000;
    m_DetailTextureRepeats = 20.0f;
    m_PickThreshold = -1.0f;
	m_bUseNormals = false;
}

Settings::~Settings()
{
    if (m_szMediaPath != NULL)
        delete[] m_szMediaPath;

}

Settings* Settings::GetInstance()
{
    if (pSettingsInstance == NULL)
        pSettingsInstance = new Settings();
    return pSettingsInstance;
}

void Settings::SetUseNormals(bool bUseNormals)
{
	m_bUseNormals = bUseNormals;
}

bool Settings::UseNormals()
{
	return m_bUseNormals;
}

bool Settings::SetProperty(const char* szProperty,const char* szValue)
{
    bool bSuccess = false;

    if (strcmp(szProperty,"verbose") == 0)
    {
        SetVerbose(strcmp(szValue,"true") == 0);
        bSuccess = true;
    }

    return bSuccess;
}

bool Settings::GetProperty(const char* szProperty,char* szValue)
{
    bool bSuccess = false;

    if (strcmp(szProperty,"verbose") == 0)
    {
        sprintf(szValue,m_bVerbose ? "true" : "false");
        bSuccess = true;
    }
    else if (strcmp(szProperty,"glinfo") == 0)
    {
        sprintf(szValue,"OpenGL Vendor: %s; OpenGL Extensions Supported: %s",glGetString(GL_VENDOR),glGetString(GL_EXTENSIONS));
        bSuccess = true;
    }

    return bSuccess;
}

void Settings::SetPreloadTextures(bool bPreload)
{
    m_bPreloadTextures = bPreload;
}

bool Settings::GetPreloadTextures()
{
    return m_bPreloadTextures;
}

void Settings::SetPickThreshold(float threshold)
{
    m_PickThreshold = threshold;
}

float Settings::GetPickThreshold()
{
    return m_PickThreshold;
}

void Settings::SetDetailTextureRepeats(float repeats)
{
    m_DetailTextureRepeats = repeats;
}

float Settings::GetDetailTextureRepeats()
{
    return m_DetailTextureRepeats;
}

void Settings::SetUseDynamicTextures(bool use)
{
    m_UseDynamicTextures = use;
}

bool Settings::UseDynamicTextures()
{
    return m_UseDynamicTextures;
}

void Settings::SetMediaPath(const char* szPath)
{
#ifdef _WIN32
    char separator = '\\';
#else
    char separator = '/';
#endif
    m_bCompressTextures = false;
    if (m_szMediaPath != NULL)
        delete[] m_szMediaPath;
    if (szPath[strlen(szPath) - 1] == separator)
    {
        m_szMediaPath = new char[strlen(szPath) + 1];
        sprintf(m_szMediaPath,szPath);
    }
    else
    {
        m_szMediaPath = new char[strlen(szPath) + 2];
        sprintf(m_szMediaPath,"%s%c",szPath,separator);
    }
}

void Settings::GetMediaPath(char** szPath)
{
    char* szOutput = new char[strlen(m_szMediaPath) + 1];
    sprintf(szOutput,m_szMediaPath);
    *szPath = szOutput;
}

void Settings::PrependMediaPath(const char* szFilename,char* szFullFilename)
{
    sprintf(szFullFilename,"%s%s",m_szMediaPath,szFilename);
}

void Settings::SetHeadless(bool isHeadless)
{
    m_IsHeadless = isHeadless;
}

bool Settings::IsHeadless()
{
    return m_IsHeadless;
}

void Settings::SetVerbose(bool bVerbose)
{
    m_bVerbose = bVerbose;
}

bool Settings::IsVerbose()
{
    return m_bVerbose;
}

void Settings::SetEditor(bool isEditor)
{
    m_IsEditor = isEditor;
}
 
bool Settings::IsEditor()
{
    return m_IsEditor;
}

int Settings::GetScreenWidth()
{
    return m_ScreenWidth;
}

void Settings::SetScreenWidth(int width)
{
    m_ScreenWidth = width;
}

int Settings::GetScreenHeight()
{
    return m_ScreenHeight;
}

void Settings::SetScreenHeight(int height)
{
    m_ScreenHeight = height;
}

bool Settings::IsCompilerOnly()
{
        return m_bIsCompilerOnly;
}

void Settings::SetCompilerOnly(bool bIsCompilerOnly)
{
        m_bIsCompilerOnly = bIsCompilerOnly;
}

void Settings::SetTextureCompression(bool bCompress)
{
    m_bCompressTextures = bCompress;
}

bool Settings::IsTextureCompression()
{
    return m_bCompressTextures;
}

Vector& Vector::operator = (const Vector& v)
{
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}

void Settings::SetTessellateMethod(int method)
{
    m_TessellateMethod = method;
}

int Settings::GetTessellateMethod()
{
    return m_TessellateMethod;
}

void Settings::SetTessellateZWeight(float method)
{
    m_TessellateZWeight = method;
}

float Settings::GetTessellateZWeight()
{
    return m_TessellateZWeight;
}

void Settings::LogStats()
{
	if (Settings::GetInstance()->IsVerbose())
	{
		m_Logfile << "Counts - Blocks: " << TerrainBlock::GetCount() <<
			", Strips: " << TriangleStrip::GetCount() <<
			", Fans: " << TriangleFan::GetCount() << endl;
	}



}




float Vector::Normalize(float tolerance)
{
    float length = GetLength();

    if (length > tolerance)
    {
        float invLength = 1.0f / length;
        x *= invLength;
        y *= invLength;
        z *= invLength;
    }
    else
    {
        length = 0.0;
    }

    return length;
}

float Vector::GetLength()
{
    return static_cast<float>(sqrt(x*x + y*y + z*z));
}

void Vector::RotateZ(float theta)
{
	float newx,newy;

	newx = static_cast<float>(x * cos(theta) - y * sin(theta));
	newy = static_cast<float>(x * sin(theta) + y * cos(theta));
	x = newx;
	y = newy;
}

Plane::Plane(Vector& p1,Vector& p2,Vector& p3)
{
    defineFromPoints(p1,p2,p3);
}

void Plane::defineFromPoints(Vector& p1,Vector& p2,Vector& p3)
{
    Vector v1,v2,normal;
// Find the normal of the polygon defined by the three points(cross product of 2 vertex vectors)
    v1.x = p2.x - p1.x;
    v1.y = p2.y - p1.y;
    v1.z = p2.z - p1.z;

    v2.x = p3.x - p1.x;
    v2.y = p3.y - p1.y;
    v2.z = p3.z - p1.z;

    // why?
    //v1.Normalize();
    //v2.Normalize();
// Find surface normal based on cross product.
    normal.x = v1.y * v2.z - v2.y * v1.z;
    normal.y = v2.x * v1.z - v1.x * v2.z;
    normal.z = v1.x * v2.y - v2.x * v1.y;
    normal.Normalize();
// This surface normal represents the a,b,c components of the plane equation.
    a = normal.x;
    b = normal.y;
    c = normal.z;
// The d component is calculated from Ax + By + Cz + D = 0
    d = -(a * p1.x + b * p1.y + c * p1.z);
}

Triangle::Triangle()
{
}

Triangle::~Triangle()
{
}

void Triangle::DefineFromPoints(Vector& p1,Vector& p2,Vector& p3)
{
    m_pVertices[0].x = p1.x;
    m_pVertices[0].y = p1.y;
    m_pVertices[0].z = p1.z;
    m_pVertices[1].x = p2.x;
    m_pVertices[1].y = p2.y;
    m_pVertices[1].z = p2.z;
    m_pVertices[2].x = p3.x;
    m_pVertices[2].y = p3.y;
    m_pVertices[2].z = p3.z;
#ifdef _USE_RAYTRACING_SUPPORT_
    m_Plane.defineFromPoints(p3,p2,p1);
#endif
}

Vector* Triangle::GetVertex(int index)
{
    return &m_pVertices[index];
}

#ifdef _USE_RAYTRACING_SUPPORT_
Plane* Triangle::GetPlane()
{
    return &m_Plane;
}
#endif

char szElevName[1024];
char szTexName[1024];

TerrainLattice::TerrainLattice(const char* szBaseName,const char* szExtensionElev,const char* szExtensionTex,const char* szDetailTextureName,float vertexSpacing,float elevationScale,int maxNumTriangles,bool bUseBorders,int widthTerrains,int heightTerrains)
{

	if (Settings::GetInstance()->IsVerbose())
	{
		m_Logfile << "Creating TerrainLattice" << std::endl;
	}
    
	
	m_szBaseName = new char[strlen(szBaseName)+1];
    sprintf(m_szBaseName,szBaseName);
    m_szExtensionElev = new char[strlen(szExtensionElev)+1];
    sprintf(m_szExtensionElev,szExtensionElev);
    if (szExtensionTex == NULL)
        m_szExtensionTex = NULL;
    else
    {
        m_szExtensionTex = new char[strlen(szExtensionTex)+1];
        sprintf(m_szExtensionTex,szExtensionTex);
    }
    if (szDetailTextureName == NULL)
        m_szDetailTextureName = NULL;
    else
    {
        m_szDetailTextureName = new char[strlen(szDetailTextureName)+1];
        sprintf(m_szDetailTextureName,szDetailTextureName);
    }
    m_VertexSpacing = vertexSpacing;
    m_ElevationScale = elevationScale;
    m_MaxNumTriangles = maxNumTriangles;
    m_bUseBorders = bUseBorders;
    m_WidthTerrains = widthTerrains;
    m_HeightTerrains = heightTerrains;
    m_pTerrains = new Terrain*[widthTerrains * heightTerrains];
    for (int i = 0; i < widthTerrains * heightTerrains; i++)
        m_pTerrains[i] = NULL;
    m_WidthActiveTerrains = m_HeightActiveTerrains = 1;

    Terrain* pTerrain;
    sprintf(szElevName,"%sElev.%ld-%ld.%s",m_szBaseName,0,0,m_szExtensionElev);

    pTerrain = new Terrain(szElevName,NULL,NULL,m_VertexSpacing,m_ElevationScale,m_MaxNumTriangles,m_bUseBorders);


//    if (m_szExtensionTex == NULL)
//        pTerrain = new Terrain(szElevName,NULL,NULL,m_VertexSpacing,m_ElevationScale,m_MaxNumTriangles,m_bUseBorders);
//    else
//    {
//        sprintf(szTexName,"%sTex.%ld-%ld.%s",m_szBaseName,0,0,m_szExtensionTex);
//        pTerrain = new Terrain(szElevName,szTexName,m_szDetailTextureName,m_VertexSpacing,m_ElevationScale,m_MaxNumTriangles,m_bUseBorders);
//    }
    m_TerrainWidth = pTerrain->GetWidth();
    m_TerrainHeight = pTerrain->GetHeight();
    delete pTerrain;
    m_pTextureFactory = NULL;

	m_OffsetIndexX[Terrain::DIR_CENTER] = 0;
	m_OffsetIndexX[Terrain::DIR_NORTH] = 0;
	m_OffsetIndexX[Terrain::DIR_NORTHEAST] = 1;
	m_OffsetIndexX[Terrain::DIR_EAST] = 1;
	m_OffsetIndexX[Terrain::DIR_SOUTHEAST] = 1;
	m_OffsetIndexX[Terrain::DIR_SOUTH] = 0;
	m_OffsetIndexX[Terrain::DIR_SOUTHWEST] = -1;
	m_OffsetIndexX[Terrain::DIR_WEST] = -1;
	m_OffsetIndexX[Terrain::DIR_NORTHWEST] = -1;

	m_OffsetIndexY[Terrain::DIR_CENTER] = 0;
	m_OffsetIndexY[Terrain::DIR_NORTH] = 1;
	m_OffsetIndexY[Terrain::DIR_NORTHEAST] = 1;
	m_OffsetIndexY[Terrain::DIR_EAST] = 0;
	m_OffsetIndexY[Terrain::DIR_SOUTHEAST] = -1;
	m_OffsetIndexY[Terrain::DIR_SOUTH] = -1;
	m_OffsetIndexY[Terrain::DIR_SOUTHWEST] = -1;
	m_OffsetIndexY[Terrain::DIR_WEST] = 0;
	m_OffsetIndexY[Terrain::DIR_NORTHWEST] = 1;
	
	m_CurrentTerrainIndex[Terrain::DIR_CENTER] = -1;
   m_CurrentTerrainIndex[Terrain::DIR_SOUTH] = -1;
   m_CurrentTerrainIndex[Terrain::DIR_SOUTHEAST] = -1;
	m_CurrentTerrainIndex[Terrain::DIR_SOUTHWEST] = -1 ;
	m_CurrentTerrainIndex[Terrain::DIR_EAST] = -1;
	m_CurrentTerrainIndex[Terrain::DIR_WEST] = -1;
	m_CurrentTerrainIndex[Terrain::DIR_NORTH] = -1;
   m_CurrentTerrainIndex[Terrain::DIR_NORTHEAST] =  -1;
   m_CurrentTerrainIndex[Terrain::DIR_NORTHWEST] = -1;
}

TerrainLattice::~TerrainLattice()
{
	cout << "~TerrainLattice()\n";
	if (Settings::GetInstance()->IsVerbose())
	{
		m_Logfile << "Removing TerrainLattice" << std::endl;
	}
    
	delete[] m_szBaseName;
    delete[] m_szExtensionElev;
    delete[] m_szExtensionTex;
    delete[] m_szDetailTextureName;
	
    for (int i = 0; i < m_WidthTerrains * m_HeightTerrains; i++)
        if (m_pTerrains[i])
			delete m_pTerrains[i];


    delete[] m_pTerrains;
}

void TerrainLattice::SetTextureFactory(TextureFactory* pFactory,int tilesWidth,int tilesHeight)
{
    m_pTextureFactory = pFactory;
    m_FactoryTilesWidth = tilesWidth;
    m_FactoryTilesHeight = tilesHeight;

}

void TerrainLattice::GenerateTextures(TextureSet* pTextureSet,TextureFactory* pTextureFactory,int tilesWidth,int tilesHeight)
{
    m_pTextureFactory = pTextureFactory;
    m_FactoryTilesWidth = tilesWidth;
    m_FactoryTilesHeight = tilesHeight;
    for (int i = 0; i < m_WidthTerrains * m_HeightTerrains; i++)
    {
        LoadTerrain(i);

        int tileIndex = 0;
        for (float y = 0.0f; y < m_pTerrains[i]->GetHeight(); y += m_pTerrains[i]->m_TextureTileHeight)
        {
            for (float x = 0.0f; x < m_pTerrains[i]->GetWidth(); x += m_pTerrains[i]->m_TextureTileWidth)
            {
                pTextureFactory->GenerateTextureCell(m_pTerrains[i],m_pTerrains[i]->GetTextureCell(i),x,y,m_pTerrains[i]->m_TextureTileWidth,m_pTerrains[i]->m_TextureTileHeight);
            }
        }
        int indexX = i % m_WidthTerrains;
        int indexY = i / m_WidthTerrains;
        sprintf(szTexName,"%sDetailTex.%ld-%ld.surface",m_szBaseName,indexX,indexY);
        m_pTerrains[i]->Write(szTexName);
        delete m_pTerrains[i];
        m_pTerrains[i] = NULL;
    }
}

void TerrainLattice::AddTerrain(Terrain* pTerrain,int positionX,int positionY)
{
    m_pTerrains[positionY * m_WidthTerrains + positionX] = pTerrain;
    pTerrain->SetLatticePosition(positionX,positionY);
}

Terrain* TerrainLattice::GetTerrain(int positionX,int positionY)
{
    return m_pTerrains[positionY * m_WidthTerrains + positionX];
}


Terrain* TerrainLattice::GetTerrainRelative(Terrain* pTerrain,Terrain::DIRECTION direction)
{
    int offsetX,offsetY;
    switch (direction)
    {
        case Terrain::DIR_NORTH:
            offsetX = 0;
            offsetY = 1;
            break;
        case Terrain::DIR_NORTHEAST:
            offsetX = 1;
            offsetY = 1;
            break;
        case Terrain::DIR_EAST:
            offsetX = 1;
            offsetY = 0;
            break;
        case Terrain::DIR_SOUTHEAST:
            offsetX = 1;
            offsetY = -1;
            break;
        case Terrain::DIR_SOUTH:
            offsetX = 0;
            offsetY = -1;
            break;
        case Terrain::DIR_SOUTHWEST:
            offsetX = -1;
            offsetY = -1;
            break;
        case Terrain::DIR_WEST:
            offsetX = -1;
            offsetY = 0;
            break;
        case Terrain::DIR_NORTHWEST:
            offsetX = -1;
            offsetY = 1;
            break;
    }
    return GetTerrainRelative(pTerrain,offsetX,offsetY);
}


Terrain* TerrainLattice::GetTerrainRelative(Terrain* pTerrain,int positionX,int positionY)
{
    Terrain* pRequestedTerrain = NULL;
    if (-1 <= positionX && positionX <= 1 && -1 <= positionY && positionY <= 1)
    {
        int x,y;
        pTerrain->GetLatticePosition(x,y);
        x += positionX;
        y += positionY;

        Terrain* pTerrainCenter = m_pTerrains[m_CurrentTerrainIndex[Terrain::DIR_CENTER]];
        int centerX,centerY;
        pTerrainCenter->GetLatticePosition(centerX,centerY);
        if (abs(x - centerX) <= m_WidthActiveTerrains && abs(y - centerY) <= m_HeightActiveTerrains)
        {
            if (0 <= x && x < m_WidthTerrains && 0 <= y && y < m_HeightTerrains)
                pRequestedTerrain = GetTerrain(x,y);
        }
    }
    return pRequestedTerrain;
}

Terrain* TerrainLattice::GetTerrainAtPoint(float x,float y)
{
    int indexX = (int)x / (int)m_TerrainWidth;
    int indexY = (int)y / (int)m_TerrainHeight;
    if (0 <= indexX && indexX < m_WidthTerrains && 0 <= indexY && indexY < m_HeightTerrains)
        return GetTerrain(indexX,indexY);
    else
        return NULL;
}

// Set Camera Position, position is given in global coordinates.
void TerrainLattice::SetCameraPosition(float x,float y,float z)
{
    int indexX = (int)x / (int)m_TerrainWidth;
    int indexY = (int)y / (int)m_TerrainHeight;
    m_CurrentTerrainIndex[Terrain::DIR_CENTER] = indexY * m_WidthTerrains + indexX;
    m_CurrentTerrainIndex[Terrain::DIR_SOUTH] = 0 < indexY ? m_CurrentTerrainIndex[Terrain::DIR_CENTER] - m_WidthTerrains : -1;
    m_CurrentTerrainIndex[Terrain::DIR_SOUTHEAST] = (0 < indexY && indexX < m_WidthTerrains - 1) ? m_CurrentTerrainIndex[Terrain::DIR_CENTER] - m_WidthTerrains + 1 : -1;
    m_CurrentTerrainIndex[Terrain::DIR_SOUTHWEST] = 0 < indexX && 0 < indexY ? m_CurrentTerrainIndex[Terrain::DIR_CENTER] - m_WidthTerrains - 1 : -1;
    m_CurrentTerrainIndex[Terrain::DIR_EAST] = indexX < m_WidthTerrains - 1 ? m_CurrentTerrainIndex[Terrain::DIR_CENTER] + 1 : -1;
    m_CurrentTerrainIndex[Terrain::DIR_WEST] = 0 < indexX ? m_CurrentTerrainIndex[Terrain::DIR_CENTER] - 1 : -1;
    m_CurrentTerrainIndex[Terrain::DIR_NORTH] = indexY < m_HeightTerrains - 1 ? m_CurrentTerrainIndex[Terrain::DIR_CENTER] + m_WidthTerrains : -1;
    m_CurrentTerrainIndex[Terrain::DIR_NORTHEAST] = (indexX < m_WidthTerrains - 1 && indexY < m_HeightTerrains - 1) ? m_CurrentTerrainIndex[Terrain::DIR_CENTER] + m_WidthTerrains + 1 : -1;
    m_CurrentTerrainIndex[Terrain::DIR_NORTHWEST] = (indexY < m_HeightTerrains - 1 && 0 < indexX) ? m_CurrentTerrainIndex[Terrain::DIR_CENTER] + m_WidthTerrains - 1 : -1;

    for (int i = 0; i < m_WidthTerrains * m_HeightTerrains; i++)
    {
        bool active = false;
        for (int dir = 0; dir < 9 && !active; dir++)
            active = (i == m_CurrentTerrainIndex[dir]);
        if (!active && m_pTerrains[i] != NULL)
        {
            vector<TerrainLoadListener*>::iterator iter = m_TerrainLoadListeners.begin();
            while (iter != m_TerrainLoadListeners.end())
            {
                TerrainLoadListener* pListener = *iter;
                pListener->TerrainUnloading(m_pTerrains[i]);
                iter++;
            }
            delete m_pTerrains[i];
            m_pTerrains[i] = NULL;
        }
        else if (active && m_pTerrains[i] == NULL)
        {
            LoadTerrain(i);
            vector<TerrainLoadListener*>::iterator iter = m_TerrainLoadListeners.begin();
            while (iter != m_TerrainLoadListeners.end())
            {
                TerrainLoadListener* pListener = *iter;
                pListener->TerrainLoaded(m_pTerrains[i]);
                iter++;
            }
        }
    }
}

void TerrainLattice::LoadTerrain(int index)
{
    int indexX = index % m_WidthTerrains;
    int indexY = index / m_WidthTerrains;
    sprintf(szElevName,"%sElev.%ld-%ld.%s",m_szBaseName,indexX,indexY,m_szExtensionElev);
    Terrain* pTerrain;
    if (m_pTextureFactory == NULL)
    {
        sprintf(szTexName,"%sTEX.%ld-%ld.%s",m_szBaseName,indexX,indexY,m_szExtensionTex);
        pTerrain = new Terrain(szElevName,szTexName,m_szDetailTextureName,m_VertexSpacing,m_ElevationScale,m_MaxNumTriangles,m_bUseBorders,/* (float)indexX * m_TerrainWidth */ 0, /* (float)indexY * m_TerrainHeight */ 0);
    }
    else
    {
        pTerrain = new Terrain(szElevName,NULL,m_szDetailTextureName,m_VertexSpacing,m_ElevationScale,m_MaxNumTriangles,m_bUseBorders, /* (float)indexX * m_TerrainWidth */ 0, /* (float)indexY * m_TerrainHeight */ 0,m_FactoryTilesWidth,m_FactoryTilesHeight);
		pTerrain->SetTextureFactory(m_pTextureFactory);
    }
    if (m_TerrainWidth == 0.0f)
    {
        m_TerrainWidth = pTerrain->GetWidth();
        m_TerrainHeight = pTerrain->GetHeight();
    }
	if (pTerrain)
		pTerrain->SetDetailThreshold(m_fThreshold);
    AddTerrain(pTerrain,indexX,indexY);
}

void TerrainLattice::SetDetailThreshold(float threshold)
{

	m_fThreshold = threshold;
    for (int i = 0; i < 9; i++)
    {
        if (0 <= m_CurrentTerrainIndex[i])
		{
			if ( m_pTerrains[m_CurrentTerrainIndex[i]] )
				m_pTerrains[m_CurrentTerrainIndex[i]]->SetDetailThreshold(threshold);
		}
    }
}

float TerrainLattice::GetElevation(float x,float y)
{
    Terrain* pTerrain = GetTerrainAtPoint(x,y);
    if (pTerrain != NULL)
	{
		float xLocal = x - m_TerrainWidth*pTerrain->m_LatticePositionX;
		float yLocal = y - m_TerrainHeight*pTerrain->m_LatticePositionY;
        return pTerrain->GetElevation(xLocal,yLocal);
	}
    else
        return 0.0f;
}

void TerrainLattice::GetNormal(float x,float y,float& normalX,float& normalY,float& normalZ )
{
    Terrain* pTerrain = GetTerrainAtPoint(x,y);
    if (pTerrain != NULL)
	{
		float xLocal = x - m_TerrainWidth*pTerrain->m_LatticePositionX;
		float yLocal = y - m_TerrainHeight*pTerrain->m_LatticePositionY;
        pTerrain->GetNormal(xLocal,yLocal, normalX, normalY, normalZ );
	}
    else
	{
		normalX = 0; normalY = 0; normalZ = 1;
	}
}

Terrain::DIRECTION TerrainLattice::GetOppositeDirection(Terrain::DIRECTION direction)
{
    Terrain::DIRECTION oppositeDirection;
    switch (direction)
    {
        case Terrain::DIR_NORTH:
            oppositeDirection = Terrain::DIR_SOUTH;
            break;
        case Terrain::DIR_NORTHEAST:
            oppositeDirection = Terrain::DIR_SOUTHWEST;
            break;
        case Terrain::DIR_EAST:
            oppositeDirection = Terrain::DIR_WEST;
            break;
        case Terrain::DIR_SOUTHEAST:
            oppositeDirection = Terrain::DIR_NORTHWEST;
            break;
        case Terrain::DIR_SOUTH:
            oppositeDirection = Terrain::DIR_NORTH;
            break;
        case Terrain::DIR_SOUTHWEST:
            oppositeDirection = Terrain::DIR_NORTHEAST;
            break;
        case Terrain::DIR_WEST:
            oppositeDirection = Terrain::DIR_EAST;
            break;
        case Terrain::DIR_NORTHWEST:
            oppositeDirection = Terrain::DIR_SOUTHEAST;
            break;
        case Terrain::DIR_CENTER:
            oppositeDirection = Terrain::DIR_CENTER;
            break;
    }
    return oppositeDirection;
}

void TerrainLattice::Tessellate()
{
    int i;
    for (i = 0; i < 9; i++)
    {
        if (m_CurrentTerrainIndex[i] != -1)
		{
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glTranslatef( m_OffsetIndexX[i]*m_TerrainWidth,
				          m_OffsetIndexY[i]*m_TerrainHeight,
						  0);
         
			m_pTerrains[m_CurrentTerrainIndex[i]]->Tessellate();

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		
		}
    }

    for (i = 0; i < 9; i++)
    {
        if (m_CurrentTerrainIndex[i] != -1)
        {
            for (int direction = 0; direction < 8; direction++)
            {
                if (direction != Terrain::DIR_CENTER)
                {
                    Terrain* pTerrain = GetTerrainRelative(m_pTerrains[m_CurrentTerrainIndex[i]],(Terrain::DIRECTION)direction);
                    if (pTerrain != NULL)
                    {
                        m_pTerrains[m_CurrentTerrainIndex[i]]->UpdateNeighbor(pTerrain,(Terrain::DIRECTION)direction);
                        pTerrain->UpdateNeighbor(m_pTerrains[m_CurrentTerrainIndex[i]],GetOppositeDirection((Terrain::DIRECTION)direction));
                    }
                }
            }
        }
    }

    for (i = 0; i < 9; i++)
    {
        if (m_CurrentTerrainIndex[i] != -1)
            m_pTerrains[m_CurrentTerrainIndex[i]]->m_pRootBlock->RepairCracks(m_pTerrains[m_CurrentTerrainIndex[i]],m_pTerrains[m_CurrentTerrainIndex[i]]->m_pTriangleFans,&m_pTerrains[m_CurrentTerrainIndex[i]]->m_CountFans);
    }

}

void TerrainLattice::Render()
{
	m_LatticePolygonsRendered = 0;
    for (int i = 0; i < 9; i++)
    {
        if (m_CurrentTerrainIndex[i] != -1)
		{
			if (m_pTextureFactory)
			{
				m_pTextureFactory->SetTerrain( m_pTerrains[m_CurrentTerrainIndex[i]] );
			}
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glTranslatef( m_OffsetIndexX[i]*m_TerrainWidth,
				          m_OffsetIndexY[i]*m_TerrainHeight,
						  0);

            m_pTerrains[m_CurrentTerrainIndex[i]]->Render();

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();

			m_LatticePolygonsRendered += m_TerrainPolygonsRendered;
		}
    }
}

void TerrainLattice::AddTerrainLoadListener(TerrainLoadListener& listener)
{
    m_TerrainLoadListeners.push_back(&listener);
}

void TerrainLattice::RemoveTerrainLoadListener(TerrainLoadListener& listener)
{
    bool found = false;
    vector<TerrainLoadListener*>::iterator iter = m_TerrainLoadListeners.begin();
    while (iter != m_TerrainLoadListeners.end() && !found)
    {
        TerrainLoadListener* pListener = *iter;
        if (pListener == &listener)
        {
            m_TerrainLoadListeners.erase(iter);
            found = true;
        }
        else
            iter++;
    }
}

float TerrainLattice::GetWidth()
{
    return m_WidthTerrains * m_TerrainWidth;
}

float TerrainLattice::GetHeight()
{
    return m_HeightTerrains * m_TerrainHeight;
}

Texture::Texture(const Uint8* pBuffer,int width,int height,int rowLength,int borderSize,bool bClamp,bool useCompression,bool bAlpha)
{
    int bytesPerPixel = bAlpha ? 4 : 3;
    m_pBuffer = new Uint8[height * width * bytesPerPixel];
    int k = 0;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width * bytesPerPixel; j++,k++)
        {
            m_pBuffer[k] = pBuffer[i * rowLength * bytesPerPixel + j];
        }
    }
    m_BytesPerPixel = bytesPerPixel;
    m_Width = width;
    m_Height = height;
    m_UseCompression = useCompression;
    m_BorderSize = borderSize;
    m_TextureID = 0;
    m_bClamp = bClamp;
    m_RowLength = rowLength;
    m_bAlpha = bAlpha;
    m_SharedIndex = -1;
    m_szFilename = NULL;
    m_bAlphaMaskOnly = false;
}

Texture::Texture()
{
    m_pBuffer = NULL;
    m_Width = m_Height = 0;
    m_UseCompression = m_bClamp = m_bAlpha = false;
    m_RowLength = 0;
    m_TextureID = 0;
    m_BorderSize = 0;
    m_SharedIndex = -1;
    m_szFilename = NULL;
    m_bAlphaMaskOnly = false;
}

Texture::~Texture()
{
    UnloadTexture();
    if (m_pBuffer != NULL)
    {
        delete[] m_pBuffer;
        m_pBuffer = NULL;
    }
    if (m_szFilename != NULL)
        delete[] m_szFilename;
}

void Texture::SetAlphaMaskOnly(bool isMaskOnly)
{
    m_bAlphaMaskOnly = isMaskOnly;
}

GLuint Texture::UploadTexture()
{

    if (m_TextureID == 0)
    {

        
		int textureFormat;
        if (m_bAlpha)
        {
            if (m_bAlphaMaskOnly)
                textureFormat = m_AlphaTextureFormat;
            else
                textureFormat = m_MaskTextureFormat;
        }
        else
            textureFormat = m_UseCompression ? m_CompressedTextureFormat : m_DefaultTextureFormat;
        m_TextureID = CreateTexture(m_pBuffer,m_Width,m_Height,m_Width,m_BorderSize,textureFormat,m_bClamp,m_bAlpha);
		if (m_TextureID < 1)
			m_Logfile << "TERRAIN: Warning - failed to create texture (possibly out of texture memory)" << endl;
        if (!Settings::GetInstance()->IsEditor() && !Settings::GetInstance()->UseDynamicTextures())
        {
            delete[] m_pBuffer;
            m_pBuffer = NULL;
        }

		if (Settings::GetInstance()->IsVerbose())
		{
			m_Logfile << "Texture::UploadTexture() - texID: " << m_TextureID << std::endl;
		}


    }
    return m_TextureID;
}

void Texture::UnloadTexture()
{
    if (m_TextureID != 0)
    {
        glDeleteTextures(1,&m_TextureID);
        m_TextureID = 0;
    }
}

bool Texture::IsBound()
{
    return (m_TextureID != 0);
}

void Texture::FlipHorizontal()
{
    Uint8* pBufferNew = new Uint8[m_Width * m_Height * m_BytesPerPixel];
    for (int y = 0; y < m_Height; y++)
    {
        for (int x = 0; x < m_Width; x++)
        {
            int sourceIndex = y * m_Width * m_BytesPerPixel + x * m_BytesPerPixel;
            int destIndex = y * m_Width * m_BytesPerPixel + ((m_Width - x) - 1) * m_BytesPerPixel;
            for (int i = 0; i < m_BytesPerPixel; i++)
                pBufferNew[destIndex + i] = m_pBuffer[sourceIndex + i];
        }
    }
    delete[] m_pBuffer;
    m_pBuffer = pBufferNew;
}

void Texture::FlipVertical()
{
    Uint8* pBufferNew = new Uint8[m_Width * m_Height * m_BytesPerPixel];
    for (int y = 0; y < m_Height; y++)
    {
        for (int x = 0; x < m_Width; x++)
        {
            int sourceIndex = y * m_Width * m_BytesPerPixel + x * m_BytesPerPixel;
            int destIndex = ((m_Height - y) - 1) * m_Width * m_BytesPerPixel + x * m_BytesPerPixel;
            for (int i = 0; i < m_BytesPerPixel; i++)
                pBufferNew[destIndex + i] = m_pBuffer[sourceIndex + i];
        }
    }
    delete[] m_pBuffer;
    m_pBuffer = pBufferNew;
}

void Texture::SetSharedIndex(int index)
{
    m_SharedIndex = index;
}
 
int Texture::GetSharedIndex()
{
    return m_SharedIndex;
}

Uint8* Texture::GetBuffer()
{
    return m_pBuffer;
}
 
int Texture::GetWidth()
{
    return m_Width;
}
 
int Texture::GetHeight()
{
    return m_Height;
}
 
void Texture::SetFilename(const char* szFilename)
{
    if (m_szFilename)
        delete[] m_szFilename;
    m_szFilename = new char[strlen(szFilename) + 1];
    sprintf(m_szFilename,szFilename);
}
 
void Texture::Write(FILE* file,Terrain* pTerrain)
{
    fwrite(&m_SharedIndex,sizeof(int),1,file);
    if (m_SharedIndex < 0)
    {
        int len,bytesPerPixel;
        if (m_szFilename == NULL)
            len = 0;
        else
            len = strlen(m_szFilename);
 
        fwrite(&len,sizeof(int),1,file);
 
        if (m_szFilename == NULL)
        {
            bytesPerPixel = m_bAlpha ? 4 : 3;
            fwrite(&bytesPerPixel,sizeof(int),1,file);
            fwrite(&m_Width,sizeof(int),1,file);
            fwrite(&m_Height,sizeof(int),1,file);
        }
        else
        {
            fwrite(m_szFilename,sizeof(char),len,file);
        }
        
        fwrite(&m_BorderSize,sizeof(int),1,file);
        Uint8 useCompression = (Uint8)m_UseCompression;
        fwrite(&useCompression,sizeof(Uint8),1,file);
        Uint8 bClamp = (Uint8)m_bClamp;
        fwrite(&bClamp,sizeof(Uint8),1,file);
        Uint8 bAlpha = (Uint8)m_bAlpha;
        fwrite(&bAlpha,sizeof(Uint8),1,file);
        if (m_szFilename == NULL)
            fwrite(m_pBuffer,m_Height * m_Width * bytesPerPixel,1,file);
    }
}
 
void Texture::Read(FILE* file,Terrain* pTerrain)
{
    int len;
    fread(&len,sizeof(int),1,file);
    if (len == 0)
    {
        fread(&m_BytesPerPixel,sizeof(int),1,file);
        fread(&m_Width,sizeof(int),1,file);
        fread(&m_Height,sizeof(int),1,file);
        m_RowLength = m_Width;
    }
    else
    {
        m_szFilename = new char[len + 1];
        fread(m_szFilename,sizeof(char),len,file);
        m_szFilename[len] = '\0';
    }
    
    fread(&m_BorderSize,sizeof(int),1,file);
    Uint8 useCompression;
    fread(&useCompression,sizeof(Uint8),1,file);
	m_UseCompression = useCompression != 0;
    Uint8 bClamp;
    fread(&bClamp,sizeof(Uint8),1,file);
    m_bClamp = bClamp != 0;
    Uint8 bAlpha;
    fread(&bAlpha,sizeof(Uint8),1,file);
    m_bAlpha = bAlpha != 0;
    if (m_szFilename == NULL)
    {
        m_pBuffer = new Uint8[m_Height * m_Width * m_BytesPerPixel];
        fread(m_pBuffer,m_Height * m_Width * m_BytesPerPixel,1,file);
    }
    else
    {
        LoadImage(m_szFilename,m_Width,m_Height,&m_pBuffer,m_bAlpha);
    }
}
 
const char* Texture::GetFilename()
{
    return m_szFilename;
}

void Texture::SetDefaultTextureFormat(int format)
{
    m_DefaultTextureFormat = format;
}

void Texture::SetAlphaTextureFormat(int format)
{
    m_AlphaTextureFormat = format;
}

void Texture::SetMaskTextureFormat(int format)
{
    m_MaskTextureFormat = format;
}

void Texture::SetCompressedTextureFormat(int format)
{
    m_CompressedTextureFormat = format;
}

DetailTexture::DetailTexture(Texture* pTexture)
{
    m_pMask = NULL;
    m_pTexture = NULL;
    if (pTexture)
    {
    	int bytes = DETAIL_TEXTURE_SIZE*DETAIL_TEXTURE_SIZE*4;
        Uint8* pBuffer = new Uint8[bytes];
        memset(pBuffer,255,bytes);
        for (int i = 3; i < bytes; i += 4)
            pBuffer[i] = 0;
        Texture* pMask = new Texture(pBuffer,DETAIL_TEXTURE_SIZE,DETAIL_TEXTURE_SIZE,DETAIL_TEXTURE_SIZE,0,true,false,true);
        delete[] pBuffer;
        DetailTexture* pDetail = new DetailTexture;
        SetMask(pMask);
        SetTexture(pTexture);
    }
}
 
DetailTexture::~DetailTexture()
{
    if (m_pMask)
        delete m_pMask;
}

void DetailTexture::FlipHorizontal()
{
    m_pMask->FlipHorizontal();
    m_pTexture->FlipHorizontal();
}

void DetailTexture::FlipVertical()
{
    m_pMask->FlipVertical();
    m_pTexture->FlipVertical();
}
 
void DetailTexture::SetMask(Texture* pTexture)
{
    m_pMask = pTexture;
}
 
Texture* DetailTexture::GetMask()
{
    return m_pMask;
}
 
void DetailTexture::SetTexture(Texture* pTexture)
{
    m_pTexture = pTexture;
}
 
Texture* DetailTexture::GetTexture()
{
    return m_pTexture;
}
 
GLuint DetailTexture::BindMask()
{
    return m_pMask->UploadTexture();
}
 
GLuint DetailTexture::BindTexture()
{
    return m_pTexture->UploadTexture();
}
 
void DetailTexture::Unbind()
{
    m_pMask->UnloadTexture();
    m_pTexture->UnloadTexture();
}
 
void DetailTexture::Write(FILE* file,Terrain* pTerrain)
{
    m_pMask->Write(file,pTerrain);
    m_pTexture->Write(file,pTerrain);
}
 
void DetailTexture::Read(FILE* file,Terrain* pTerrain)
{
    int sharedIndex;
    fread(&sharedIndex,sizeof(int),1,file);
    if (sharedIndex < 0)
    {
        m_pMask = new Texture;
        m_pMask->Read(file,pTerrain);
    }
    else
        m_pMask = pTerrain->GetTextureSet()->GetTexture(sharedIndex);
    fread(&sharedIndex,sizeof(int),1,file);
    if (sharedIndex < 0)
    {
        m_pTexture = new Texture;
        m_pTexture->Read(file,pTerrain);
    }
    else
        m_pTexture = pTerrain->GetTextureSet()->GetTexture(sharedIndex);
}
 
TextureCell::TextureCell()
{
    m_pTexture = NULL;
}
 
TextureCell::~TextureCell()
{
    if (m_pTexture)
        delete m_pTexture;
    while (!m_DetailTextures.empty())
    {
        vector<DetailTexture*>::iterator iter = m_DetailTextures.begin();
        DetailTexture* pDetailTexture = *iter;
        m_DetailTextures.erase(iter);
        delete pDetailTexture;
    }
}

void TextureCell::FlipHorizontal()
{
    m_pTexture->FlipHorizontal();
    for (unsigned int i = 0; i < m_DetailTextures.size(); i++)
        m_DetailTextures[i]->FlipHorizontal();
}

void TextureCell::FlipVertical()
{
    m_pTexture->FlipVertical();
    for (unsigned int i = 0; i < m_DetailTextures.size(); i++)
        m_DetailTextures[i]->FlipVertical();
}
 
void TextureCell::Write(FILE* file,Terrain* pTerrain)
{
    unsigned int numDetails = m_DetailTextures.size();
    fwrite(&numDetails,sizeof(int),1,file);
    for (unsigned int i = 0; i < numDetails; i++)
        m_DetailTextures[i]->Write(file,pTerrain);
}
 
void TextureCell::Read(FILE* file,Terrain* pTerrain)
{
    int numDetails;
    fread(&numDetails,sizeof(int),1,file);
    for (int i = 0; i < numDetails; i++)
    {
        DetailTexture* pDet = new DetailTexture;
        pDet->Read(file,pTerrain);
        m_DetailTextures.push_back(pDet);
    }
}
 
void TextureCell::SetTexture(Texture* pTexture)
{
    m_pTexture = pTexture;
}
 
DetailTexture* TextureCell::GetDetail(int index)
{
    return m_DetailTextures[index];
}
 
DetailTexture* TextureCell::GetDetail(Texture* pTexFind)
{
    DetailTexture* pDetailFound = NULL;
    for (unsigned int i = 0; i < m_DetailTextures.size() && pDetailFound == NULL; i++)
    {
        if (m_DetailTextures[i]->GetTexture() == pTexFind)
            pDetailFound = m_DetailTextures[i];
    }
    return pDetailFound;
}
 
void TextureCell::AddDetail(DetailTexture* pDetail)
{
    m_DetailTextures.push_back(pDetail);
}
 
int TextureCell::GetNumberOfDetails()
{
    return m_DetailTextures.size();
}
 
GLuint TextureCell::BindTexture()
{
    return m_pTexture->UploadTexture();
}
 
GLuint TextureCell::BindMask(int index)
{
    return m_DetailTextures[index]->BindMask();
}
 
GLuint TextureCell::BindDetail(int index)
{
    return m_DetailTextures[index]->BindTexture();
}
 
void TextureCell::UnbindAll()
{
    m_pTexture->UnloadTexture();
    for (unsigned int i = 0; i < m_DetailTextures.size(); i++)
        m_DetailTextures[i]->Unbind();
}
 
TextureSet::TextureSet()
{
}
 
TextureSet::~TextureSet()
{
     while (!m_Textures.empty())
     {
         vector<Texture*>::iterator iter = m_Textures.begin();
         Texture* pTexture = *iter;
         m_Textures.erase(iter);
         delete pTexture;
     }
}
 
void TextureSet::AddTexture(Texture* pTex)
{
    m_Textures.push_back(pTex);
    pTex->SetSharedIndex(m_Textures.size() - 1);
}
 
Texture* TextureSet::GetTexture(int index)
{
    return m_Textures[index];
}
 
int TextureSet::GetNumTextures()
{
    return m_Textures.size();
}
 
void TextureSet::Write(FILE* file,Terrain* pTerrain)
{
    int size = m_Textures.size();
    fwrite(&size,sizeof(int),1,file);
    for (unsigned int i = 0; i < m_Textures.size(); i++)
    {
        int index = m_Textures[i]->GetSharedIndex();
        m_Textures[i]->SetSharedIndex(-1);
        m_Textures[i]->Write(file,pTerrain);
        m_Textures[i]->SetSharedIndex(index);
    }
}
 
void TextureSet::Read(FILE* file,Terrain* pTerrain)
{
    int size;
    fread(&size,sizeof(int),1,file);
    for (int i = 0; i < size; i++)
    {
        int index;
        fread(&index,sizeof(int),1,file);
        Texture* pTex = new Texture;
        pTex->Read(file,pTerrain);
        AddTexture(pTex);
    }
}

void DimensionPowerOf2(int origX,int origY,int& newX,int& newY)
{
    newX = newY = 0;
    for (int numPows = 20; newX == 0 || newY == 0; numPows *= 2)
    {
        int* powers = new int[numPows];
        int i;
        for (i = 0; i < numPows; i++)
            powers[i] = (int)pow(2,(double)i);
        newX = 0;
        for (i = 0; i < numPows && newX == 0; i++)
        {
            if (origX <= powers[i])
                newX = powers[i];
        }
        newY = 0;
        for (i = 0; i < numPows && newY == 0; i++)
        {
            if (origY <= powers[i])
                newY = powers[i];
        }
        delete[] powers;
    }
}

void LoadGLExtensions()
{
#ifdef _WIN32
    glMultiTexCoord2fARB_ptr = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
    glActiveTextureARB_ptr = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
    glLockArraysEXT_ptr = (PFNGLLOCKARRAYSEXTPROC)wglGetProcAddress("glLockArraysEXT");
    glUnlockArraysEXT_ptr = (PFNGLUNLOCKARRAYSEXTPROC)wglGetProcAddress("glUnlockArraysEXT");
    glClientActiveTextureARB_ptr = (PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB");
#else
    glMultiTexCoord2fARB_ptr = glMultiTexCoord2fARB;
    glActiveTextureARB_ptr = glActiveTextureARB;
    glLockArraysEXT_ptr = glLockArraysEXT;
    glUnlockArraysEXT_ptr = glUnlockArraysEXT;
    glClientActiveTextureARB_ptr = glClientActiveTextureARB;
#endif
    if(!glClientActiveTextureARB_ptr || !glActiveTextureARB_ptr || !glMultiTexCoord2fARB_ptr)
    {
        throw new DemeterException("TERRAIN: ERROR: Multitexture extensions not supported by this OpenGL vendor!");
        bMultiTextureSupported = false;
    }
}

void TextureFactory::GenerateTextureCell(Terrain* pTerrain,TextureCell* pCell,float originX,float originY,float width,float height)
{
}

void TextureFactory::SetTerrain(Terrain * pTerrain)
{
}
