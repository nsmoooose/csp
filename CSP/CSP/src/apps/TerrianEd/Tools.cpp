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
#include "Tools.h"

#include <math.h>




void CTools::NormalizeVector(float afVector[3])
{
	float length;

	length = sqrt(afVector[0]*afVector[0]+afVector[1]*afVector[1]+afVector[2]*afVector[2]);
	for (int i=0; i<3; i++) afVector[i] /= length;
}

void CTools::OutputProgressBar(int iProgress)
{
	for (int i=0; i<12; i++) printf("\b");
	printf("[");
	for (i=0; i<iProgress; i++) printf("#");
	for (i=iProgress; i<10; i++) printf(" ");
	printf("]");
}
