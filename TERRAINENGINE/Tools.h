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

#if !defined(TOOLS_H)
#define TOOLS_H



class CTools  
{
private:
	CTools() {}
public:
	static void OutputProgressBar(int iProgress);
	static void NormalizeVector(float afVector[3]);
};

#endif 
