///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   declaration of CRenderer
//   class controls the rendering process (one layer above quads) and
//   updates terraindata dynamically (texture-generation AND quad loading)
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	7/30/2001	updated documentation - zzed
//  9/19/2001	added code to change textures' resolution dynamically in
//				a background process - zzed
//	10/10/2001	added sectorlist for dynamic loading of quads - zzed
//
//
///////////////////////////////////////////////////////////////////////////

/**
 * Main class for the terrain renderer.
 */

#if !defined(MIPMAPRENDERER_H)
#define MIPMAPRENDERER_H

#include "MipMapQuad.h"
#include "TerrainData.h"	
#include "Frustum.h"
#include "Sector.h"


class CRenderer  
{
public:
	/**
	 * saves sum of rendered triangles for debug output
	 */
	int m_iTriangleCount;

	/// background-thread status
	enum EBProcStatus { bpStatStop=0, bpStatIdle, bpStatRun };

	/// starts/stops/idles background-thread
	void SetBackgroundProc(EBProcStatus eStatus);

	/// background-thread to load and generate terrain dynamically; only internal functions call it!
	void BackgroundProcess();

	/**
	 * Returns the distance of the far clipping plane (Attention: don't forget to call SetCurrentPosition() before!)
	 */
	float GetClippingDistance();
	/**
	 * Renders the complete terrain. Attention: you MUST call glPerspective using getClippingDistance() and SetCurrentPosition() before!
	 * @param bLines should wireframe or filled polygons be displayed?
	 */

#ifdef WIN32	
	static unsigned long __stdcall CatchThread(void *lpPar);
#else
	static unsigned long CatchThread(void *lpPar);
#endif

	void Render();
	/**
	 * Constructor to initialize the engine.
	 * @param pTerrainData provides the necessary terrain-data
	 */
	CRenderer(CTerrainData *pTerrainData);
	virtual ~CRenderer();

private:
	CTerrainData *m_pData;
	CFrustum m_pFrustum;
	
	// visible sectors { ymin, ymax, xmin, xmax }
	uint	m_uiVisSectors[4];
	uint	m_uiSleep;
	CSector **m_pSectorList;
	EBProcStatus m_eBProcStatus;

	void UpdateTerrain();
	void RemoveSectors(const uint uiStartIndex, const uint uiDelta, const uint uiEndIndex, const int iUpdateRefIndex, const uint uiUpdateRefDirection);
	inline void ClampValue(int &value, const int min, const int max) { if (value<min) value = min; else if (value>max) value = max; }
};

#endif 
