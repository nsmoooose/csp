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

#include "Tools.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <io.h>
#endif

#include <fcntl.h>

/*void CTools::NormalizeVector(float afVector[3])
{
	float length;

	length = sqrt(afVector[0]*afVector[0]+afVector[1]*afVector[1]+afVector[2]*afVector[2]);
	for (int i=0; i<3; i++) afVector[i] /= length;
}*/

void CTools::OutputProgressBar(int iProgress)
{
	if (iProgress>100) return;
	for (int i=0; i<7; i++) printf("\b");
	printf(" (%3d%%)", iProgress);
	/*for (i=0; i<iProgress; i++) printf("#");
	for (i=iProgress; i<10; i++) printf(" ");
	printf("]");*/
}

void CTools::CalcTriangleNormal(CVector &vNormal, CVector vVec1, CVector vVec2, CVector vVec3)
{
	CVector vDiff1, vDiff2;

	vDiff1 = vVec2 - vVec1;
	vDiff2 = vVec3 - vVec1;

	vNormal.x = vDiff1.y*vDiff2.z-vDiff1.z*vDiff2.y;
	vNormal.y = -vDiff1.z*vDiff2.x+vDiff1.x*vDiff2.z;
	vNormal.z = vDiff1.x*vDiff2.y-vDiff1.y*vDiff2.x;

	vNormal.normalize();
}

bool CTools::GetConfigData(char *sSearchString, char *sConfigData, char *sResult)
{
	unsigned int i=0, k, l;
	char buffer[256];

	while(sConfigData[i] != -1)
	{
		if(sConfigData[i] == '<')					// read the string between < and >
		{
			i++;
			k=0;

			while(sConfigData[i] != '>')
			{
				buffer[k] = sConfigData[i];
				i++;
				k++;
			}
			buffer[k] = 0;							// terminate the string with 0x00

			if(!strcmp(buffer, "EOF"))				// End of File reached?
			{
				return false;
			}

			l = strcmp(buffer, sSearchString);		// does it match to our search string? 
			k=0;
			
			i+=2;									// skip the CR + LF bytes

			if(l == 0)								// yes, the strings are equal
			{
				while(sConfigData[i] != '\n'/*0x0d*/)		// read the value, terminated by a LF
				{
					buffer[k] = sConfigData[i];
					i++;
					k++;
				}
				buffer[k] = 0;						// terminate the string with 0x00
				strcpy(sResult, buffer);				// copy result string from buffer
				return true;
			}
		}
		i++;
	};

	return false;
}
