///////////////////////////////////////////////////////////////////////////
//
//   test of a terrain-engine based on GeoMipMapping
//
//   coded by zzed and Stormbringer
//   email:   zzed@gmx.li    storm_bringer@gmx.li 
//
//   7.2.2001
//
///////////////////////////////////////////////////////////////////////////

#if !defined(MIPMAPRENDERER_H)
#define MIPMAPRENDERER_H

#include "MipMapQuad.h"
#include "TerrainData.h"	
#include "Frustum.h"


class CMipMapRenderer  
{
public:
	int m_iTriangleCount;
	float m_fDeltaZ;
	bool m_temp;
	/**
	 * selfexplaining; must be called before Render() and GetClippingDistance()
	 */
	void SetPosition(float fX, float fY, float fZ);
	/**
	 * selfexplaining; not used yet, but later for optimization issues
	 */
	void SetRotation(float fRotHor, float fRotVert);
	/**
	 * Returns the distance of the far clipping plane (Attention: don't forget to call SetCurrentPosition() before!)
	 */
	float GetClippingDistance();
	/**
	 * Renders the complete terrain. Attention: you MUST call glPerspective using getClippingDistance() and SetCurrentPosition() before!
	 * @param bLines should wireframe or filled polygons be displayed?
	 */
	void Render(bool bLines);
	/**
	 * Constructor to initialize the engine.
	 * @param pTerrainData provides the necessary terrain-data
	 */
	CMipMapRenderer(CTerrainData *pTerrainData);
	virtual ~CMipMapRenderer();

	void UpdateSettings();

	float GetDeltaZ()				{ return m_fDeltaZ; }
	int	  GetQuadLength()			{ return m_iQuadLength; }
	float GetDetailLevel()			{ return m_fDetailLevel; }
	float GetRenderDistance()		{ return m_fClippingDistance; }
	float GetMinRenderDistance()	{ return m_fMinClippingDistance; }
	float GetMaxRenderDistance()	{ return m_fMaxClippingDistance; }
	int	  GetTerrainWidth()			{ return m_iTerrainWidth; }
	int   GetTerrainHeight()		{ return m_iTerrainHeight; }
	
	void SetDeltaZ( float value )				{ m_fDeltaZ = value; }
	void SetQuadLength( int value )				{ m_iQuadLength = value; }
	void SetDetailLevel( float value )			{ m_fDetailLevel = value; }
	void SetRenderDistance( float value )		{ m_fClippingDistance = value; }
	void SetMinRenderDistance( float value )	{ m_fMinClippingDistance = value; }
	void SetMaxRenderDistance( float value )	{ m_fMaxClippingDistance = value; }


private:
	void PreprocessTerrain();
	/// current position of the eye (x-value)
	float m_fPosX;
	/// current position of the eye (y-value)
	float m_fPosY;
	/// current position of the eye (z-value)
	float m_fPosZ;
	/// current viewing direction of the eye (horizontal)
	float m_fRotHor;
	/// current viewing direction of the eye (vertical)
	float m_fRotVer;
	float m_fAverageTerrainHeight;
	/// current clipping distance ratio of quads
	float m_fRatioClippingDistance;
	/// current clipping distance of quads
	float m_fClippingDistance;
	/// minimal clipping distance
	float m_fMinClippingDistance;
	/// maximum clipping distance
	float m_fMaxClippingDistance;
	float m_fDetailLevel;
	int m_iQuadLength;
	/// how many quads broad is the whole terrain (X-axis)?
	int m_iTerrainWidth;
	/// how many quads long is the whole terrain (Y-axis)?
	int m_iTerrainHeight;
	CMipMapQuad **m_MipmapQuads;
	void InitQuads();
	CTerrainData *m_pTerrainData;
	CFrustum m_cFrustum;
};

#endif 
