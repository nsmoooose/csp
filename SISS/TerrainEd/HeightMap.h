// HeightMap.h: Schnittstelle für die Klasse HeightMap.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HEIGHTMAP_H__3530C5E9_9FEF_4DD3_A899_7CA00DB9183A__INCLUDED_)
#define AFX_HEIGHTMAP_H__3530C5E9_9FEF_4DD3_A899_7CA00DB9183A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class HeightMap  
{
public:
	HeightMap();
	float*	data;
	int		width;
	int		height;
	HeightMap(char* map_filename, int i_width, int i_height);
	virtual ~HeightMap();
};

#endif // !defined(AFX_HEIGHTMAP_H__3530C5E9_9FEF_4DD3_A899_7CA00DB9183A__INCLUDED_)
