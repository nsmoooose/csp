///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//  type definitions
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	9/15/2001	created file  - zzed
//  9/15/2001	added function fileopen - zzed
//
//
//
///////////////////////////////////////////////////////////////////////////

#include "global.h"

#include <stdlib.h>


// catches non existent files to open, displays an error and exits the program
FILE* fileopen(const char *cFilename, const char *cMode)
{
	FILE *f = fopen(cFilename, cMode);
	
	if (f==0)
	{ 
		printf("\n\n\nerror while opening file '%s'! Exiting program ....\n\n", cFilename);
		exit(-1);
	}

	return f;
}
