// TerrainManager.h: interface for the TerrainManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERRAINMANAGER_H__3078B607_0720_487B_BAB6_718D3F1DF205__INCLUDED_)
#define AFX_TERRAINMANAGER_H__3078B607_0720_487B_BAB6_718D3F1DF205__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <std.h>
#include <Material.h>
#include <Modifier.h>
#include <VertexFormats.h>
#include <ParticleStructure.h>

using namespace Jet;

class TerrainManager : 
	public ParticleStructure, 
	public Modifier  
{
public:
	TerrainManager();
	virtual ~TerrainManager();

	bool Init(RenderKernel *renderKernel);

	virtual void JETAPI ResetBoundingVolume(); /**/
	virtual Modifier::Result JETAPI Update(float dt); /**/
	virtual void JETAPI Cull(ViewPort &view, const Matrix4 &trans); /**/
	virtual void JETAPI Render(Display *display, ViewPort &viewport, Matrix4 &transformation); /**/

private:
	RenderKernel  *thisRenderKernel;
	PackedVertexP vertexarray[3];	//hier brauchen wir zum testen erstmal nur 3 vertices
	Color		  color;
	Material	 *material;

};

#endif // !defined(AFX_TERRAINMANAGER_H__3078B607_0720_487B_BAB6_718D3F1DF205__INCLUDED_)
