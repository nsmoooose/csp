///////////////////////////////////////////////////////////////////////////
//
//   TerrainLab  - CSP - http://csp.homeip.net
//
//   coded by Stormbringer and		zzed
//   email:   storm_bringer@gmx.li	zzed@gmx.li
//
///////////////////////////////////////////////////////////////////////////
//
//   Header of Class CTerrain
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	09/25/2001	file created		-	Stormbringer
//	09/27/2001  Iterate modified	-	Stormbringer
//
///////////////////////////////////////////////////////////////////////////

#if !defined(TERRAIN_H)
#define TERRAIN_H

#include <gl/gl.h>														// Header File For The OpenGL32 Library
#include <gl/glu.h>														// Header File For The GLu32 Library

#include "BaseCode.h"
#include "TerrainData.h"
#include "OGLText.h"

class CTerrain
{
public:
	CTerrain();
	~CTerrain();

	void LoadDefaultMesh();
	void LoadRAWData();
	void LoadRAWData(char *filename);
	void Iterate(float fFactorD);
	void BuildVertexArray(int MeshWidth);
	void CalcTriangleNormal(float *afNormal, float afVec1[], float afVec2[], float afVec3[]);
	void RenderSquares();
	void RenderTriangles();
	void RenderShaded();
};

#endif

