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

#include "stdafx.h"
#include "Frustum.h"

#include <gl/gl.h>
#include <math.h>



CFrustum::CFrustum()
{

}

CFrustum::~CFrustum()
{

}


// returns true if point is positioned inside frustum
bool CFrustum::PointInFrustum(float fPosX, float fPosY, float fPosZ)
{
	for (int iCurPlane = 0; iCurPlane<6; iCurPlane++)
	{
		if (m_fFrustum[iCurPlane][0] * fPosX + m_fFrustum[iCurPlane][1] * 
			fPosY + m_fFrustum[iCurPlane][2] * fPosZ + m_fFrustum[iCurPlane][3] <= 0)
		{
			return false;
		}
	}
	return true;
}



// Updates internal planes to adjust them according to actual camera position
void CFrustum::Update()
{
	float fProj[16];	// For grabbing the projection matrix
	float fView[16];	// For grabbing the modelview matrix
	float fClip[16];	// Holds the result of projection * modelview
	float fT;
	
	// Get the current projection matrix from OpenGL
	glGetFloatv(GL_PROJECTION_MATRIX, fProj);
	
	// Get the current modelview matrix from OpenGL
	glGetFloatv(GL_MODELVIEW_MATRIX, fView);
	
	// Concenate the two matrices
	fClip[ 0] = fView[ 0] * fProj[ 0] + fView[ 1] * fProj[ 4] + fView[ 2] * fProj[ 8] + fView[ 3] * fProj[12];
	fClip[ 1] = fView[ 0] * fProj[ 1] + fView[ 1] * fProj[ 5] + fView[ 2] * fProj[ 9] + fView[ 3] * fProj[13];
	fClip[ 2] = fView[ 0] * fProj[ 2] + fView[ 1] * fProj[ 6] + fView[ 2] * fProj[10] + fView[ 3] * fProj[14];
	fClip[ 3] = fView[ 0] * fProj[ 3] + fView[ 1] * fProj[ 7] + fView[ 2] * fProj[11] + fView[ 3] * fProj[15];
	
	fClip[ 4] = fView[ 4] * fProj[ 0] + fView[ 5] * fProj[ 4] + fView[ 6] * fProj[ 8] + fView[ 7] * fProj[12];
	fClip[ 5] = fView[ 4] * fProj[ 1] + fView[ 5] * fProj[ 5] + fView[ 6] * fProj[ 9] + fView[ 7] * fProj[13];
	fClip[ 6] = fView[ 4] * fProj[ 2] + fView[ 5] * fProj[ 6] + fView[ 6] * fProj[10] + fView[ 7] * fProj[14];
	fClip[ 7] = fView[ 4] * fProj[ 3] + fView[ 5] * fProj[ 7] + fView[ 6] * fProj[11] + fView[ 7] * fProj[15];
	
	fClip[ 8] = fView[ 8] * fProj[ 0] + fView[ 9] * fProj[ 4] + fView[10] * fProj[ 8] + fView[11] * fProj[12];
	fClip[ 9] = fView[ 8] * fProj[ 1] + fView[ 9] * fProj[ 5] + fView[10] * fProj[ 9] + fView[11] * fProj[13];
	fClip[10] = fView[ 8] * fProj[ 2] + fView[ 9] * fProj[ 6] + fView[10] * fProj[10] + fView[11] * fProj[14];
	fClip[11] = fView[ 8] * fProj[ 3] + fView[ 9] * fProj[ 7] + fView[10] * fProj[11] + fView[11] * fProj[15];
	
	fClip[12] = fView[12] * fProj[ 0] + fView[13] * fProj[ 4] + fView[14] * fProj[ 8] + fView[15] * fProj[12];
	fClip[13] = fView[12] * fProj[ 1] + fView[13] * fProj[ 5] + fView[14] * fProj[ 9] + fView[15] * fProj[13];
	fClip[14] = fView[12] * fProj[ 2] + fView[13] * fProj[ 6] + fView[14] * fProj[10] + fView[15] * fProj[14];
	fClip[15] = fView[12] * fProj[ 3] + fView[13] * fProj[ 7] + fView[14] * fProj[11] + fView[15] * fProj[15];
	
	// Extract the right plane
	m_fFrustum[0][0] = fClip[ 3] - fClip[ 0];
	m_fFrustum[0][1] = fClip[ 7] - fClip[ 4];
	m_fFrustum[0][2] = fClip[11] - fClip[ 8];
	m_fFrustum[0][3] = fClip[15] - fClip[12];
	
	// Normalize the result
	fT = (float) sqrt(m_fFrustum[0][0] * m_fFrustum[0][0] + m_fFrustum[0][1] * m_fFrustum[0][1] + 
		m_fFrustum[0][2] * m_fFrustum[0][2]);
	m_fFrustum[0][0] /= fT;
	m_fFrustum[0][1] /= fT;
	m_fFrustum[0][2] /= fT;
	m_fFrustum[0][3] /= fT;
	
	// Extract the left plane
	m_fFrustum[1][0] = fClip[ 3] + fClip[ 0];
	m_fFrustum[1][1] = fClip[ 7] + fClip[ 4];
	m_fFrustum[1][2] = fClip[11] + fClip[ 8];
	m_fFrustum[1][3] = fClip[15] + fClip[12];
	
	// Normalize the result
	fT = (float) sqrt(m_fFrustum[1][0] * m_fFrustum[1][0] + m_fFrustum[1][1] * m_fFrustum[1][1] + 
		m_fFrustum[1][2] * m_fFrustum[1][2]);
	m_fFrustum[1][0] /= fT;
	m_fFrustum[1][1] /= fT;
	m_fFrustum[1][2] /= fT;
	m_fFrustum[1][3] /= fT;
	
	// Extract the bottom plane
	m_fFrustum[2][0] = fClip[ 3] + fClip[ 1];
	m_fFrustum[2][1] = fClip[ 7] + fClip[ 5];
	m_fFrustum[2][2] = fClip[11] + fClip[ 9];
	m_fFrustum[2][3] = fClip[15] + fClip[13];
	
	// Normalize the result
	fT = (float) sqrt(m_fFrustum[2][0] * m_fFrustum[2][0] + m_fFrustum[2][1] * m_fFrustum[2][1] + 
		m_fFrustum[2][2] * m_fFrustum[2][2]);
	m_fFrustum[2][0] /= fT;
	m_fFrustum[2][1] /= fT;
	m_fFrustum[2][2] /= fT;
	m_fFrustum[2][3] /= fT;
	
	// Extract the top plane
	m_fFrustum[3][0] = fClip[ 3] - fClip[ 1];
	m_fFrustum[3][1] = fClip[ 7] - fClip[ 5];
	m_fFrustum[3][2] = fClip[11] - fClip[ 9];
	m_fFrustum[3][3] = fClip[15] - fClip[13];
	
	// Normalize the result
	fT = (float) sqrt(m_fFrustum[3][0] * m_fFrustum[3][0] + m_fFrustum[3][1] * m_fFrustum[3][1] + 
		m_fFrustum[3][2] * m_fFrustum[3][2]);
	m_fFrustum[3][0] /= fT;
	m_fFrustum[3][1] /= fT;
	m_fFrustum[3][2] /= fT;
	m_fFrustum[3][3] /= fT;
	
	// Extract the far plane
	m_fFrustum[4][0] = fClip[ 3] - fClip[ 2];
	m_fFrustum[4][1] = fClip[ 7] - fClip[ 6];
	m_fFrustum[4][2] = fClip[11] - fClip[10];
	m_fFrustum[4][3] = fClip[15] - fClip[14];
	
	// Normalize the result
	fT = (float) sqrt(m_fFrustum[4][0] * m_fFrustum[4][0] + m_fFrustum[4][1] * m_fFrustum[4][1] + 
		m_fFrustum[4][2] * m_fFrustum[4][2]);
	m_fFrustum[4][0] /= fT;
	m_fFrustum[4][1] /= fT;
	m_fFrustum[4][2] /= fT;
	m_fFrustum[4][3] /= fT;
	
	// Extract the near plane
	m_fFrustum[5][0] = fClip[ 3] + fClip[ 2];
	m_fFrustum[5][1] = fClip[ 7] + fClip[ 6];
	m_fFrustum[5][2] = fClip[11] + fClip[10];
	m_fFrustum[5][3] = fClip[15] + fClip[14];
	
	// Normalize the result
	fT = (float) sqrt(m_fFrustum[5][0] * m_fFrustum[5][0] + m_fFrustum[5][1] * m_fFrustum[5][1] + 
		m_fFrustum[5][2] * m_fFrustum[5][2]);
	m_fFrustum[5][0] /= fT;
	m_fFrustum[5][1] /= fT;
	m_fFrustum[5][2] /= fT;
	m_fFrustum[5][3] /= fT;
}


// returns true when heightmap-quad is in frustum
bool CFrustum::QuadInFrustum(float fPosX, float fPosY, float fPosZ, float fSize, float fHeight)
{
	for (int iCurPlane = 0; iCurPlane<6; iCurPlane++)
	{
		//if (iCurPlane==2 || iCurPlane==3) fLength = fHeight;
		//	else fLength = fSize;
		if (m_fFrustum[iCurPlane][0] * (fPosX - fSize) + m_fFrustum[iCurPlane][1] *
			(fPosY - fSize) + m_fFrustum[iCurPlane][2] * (fPosZ - fHeight) + 
			m_fFrustum[iCurPlane][3] > 0)
			continue;
		if (m_fFrustum[iCurPlane][0] * (fPosX + fSize) + m_fFrustum[iCurPlane][1] * 
			(fPosY - fSize) + m_fFrustum[iCurPlane][2] * (fPosZ - fHeight) + 
			m_fFrustum[iCurPlane][3] > 0)
			continue;
		if (m_fFrustum[iCurPlane][0] * (fPosX - fSize) + m_fFrustum[iCurPlane][1] * 
			(fPosY + fSize) + m_fFrustum[iCurPlane][2] * (fPosZ - fHeight) +
			m_fFrustum[iCurPlane][3] > 0)
			continue;
		if (m_fFrustum[iCurPlane][0] * (fPosX + fSize) + m_fFrustum[iCurPlane][1] * 
			(fPosY + fSize) + m_fFrustum[iCurPlane][2] * (fPosZ - fHeight) + 
			m_fFrustum[iCurPlane][3] > 0)
			continue;
		if (m_fFrustum[iCurPlane][0] * (fPosX - fSize) + m_fFrustum[iCurPlane][1] * 
			(fPosY - fSize) + m_fFrustum[iCurPlane][2] * (fPosZ + fHeight) + 
			m_fFrustum[iCurPlane][3] > 0)
			continue;
		if (m_fFrustum[iCurPlane][0] * (fPosX + fSize) + m_fFrustum[iCurPlane][1] * 
			(fPosY - fSize) + m_fFrustum[iCurPlane][2] * (fPosZ + fHeight) + 
			m_fFrustum[iCurPlane][3] > 0)
			continue;
		if (m_fFrustum[iCurPlane][0] * (fPosX - fSize) + m_fFrustum[iCurPlane][1] * 
			(fPosY + fSize) + m_fFrustum[iCurPlane][2] * (fPosZ + fHeight) + 
			m_fFrustum[iCurPlane][3] > 0)
			continue;
		if (m_fFrustum[iCurPlane][0] * (fPosX + fSize) + m_fFrustum[iCurPlane][1] * 
			(fPosY + fSize) + m_fFrustum[iCurPlane][2] * (fPosZ + fHeight) + 
			m_fFrustum[iCurPlane][3] > 0)
			continue;

		return false;
	}

	return true;
}