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
#include "EngineManager.h"


int main(int argc, char* argv[])
{
	EngineManager *em = new EngineManager();
	em->StartEngine(argc, argv);
	delete(em);
	
	return 0;
}
