///////////////////////////////////////////////////////////////////////////
//
//   Falcon 4.0 Terrain Level Class
//	 for the CSP Terrain Editor
//
//   coded Stormbringer
//   email: storm_bringer@gmx.li 
//
//   11.04.2001
//
///////////////////////////////////////////////////////////////////////////

#if !defined(F4TERRAINLEVEL_H)
#define F4TERRAINLEVEL_H

class CF4TerrainLevel
{
public:
	bool Setup(int iLevel, int iWidth, int iHeight, CString MapFileName);
	void Cleanup();
	CF4TerrainLevel();
	virtual ~CF4TerrainLevel();

private:

};

#endif