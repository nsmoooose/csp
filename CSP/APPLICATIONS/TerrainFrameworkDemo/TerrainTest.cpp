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

#ifdef WIN32
#include "stdafx.h"
#endif

#include "EngineManager.h"


int main(int argc, char* argv[])
{
        printf("Creating EngineManager\n");
	EngineManager *em = new EngineManager();
	printf("Starting EngineManager\n");
	em->StartEngine(argc, argv);
	delete(em);
	
	return 0;
}
