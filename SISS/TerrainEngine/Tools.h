///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   <file-description>
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//
//
//
//
///////////////////////////////////////////////////////////////////////////

#if !defined(TOOLS_H)
#define TOOLS_H

#include "Vector.h"

class CTools  
{
private:
	CTools() {}
public:
	static void OutputProgressBar(int iProgress);
	//static void NormalizeVector(float afVector[3]);
	static void CalcTriangleNormal(CVector &vNormal, CVector vVec1, CVector vVec2, CVector vVec3);
	static bool GetConfigData(char *sSearchString, char *sConfigData, char *sResult);
};

#endif 
