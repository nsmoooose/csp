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

#include "stdinc.h"

#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string>


void CTools::OutputProgressBar(int iProgress)
{
	if (iProgress>100) return;
	for (int i=0; i<7; i++) printf("\b");
	printf(" (%3d%%)", iProgress);
	/*for (i=0; i<iProgress; i++) printf("#");
	for (i=iProgress; i<10; i++) printf(" ");
	printf("]");*/
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
