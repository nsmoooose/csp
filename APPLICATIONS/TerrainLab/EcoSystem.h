// EcoSystem.h: Schnittstelle für die Klasse CEcoSystem.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ECOSYSTEM_H__107FDFF3_252B_42B3_9FFE_3645EEF70200__INCLUDED_)
#define AFX_ECOSYSTEM_H__107FDFF3_252B_42B3_9FFE_3645EEF70200__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


typedef struct
{
	char *cName;
	char *cTextureFileName;
	float fElevationMin;
	float fElevationMax;
	float fElevationSharpness;
	float fRelativeElevationMax;
	float fRelativeElevationMin;
	float fRelativeElevationSharpness;
	float fSlopeMin;
	float fSlopeMax;
	float fSlopeSharpness;
	float fEcosystemColour[3];
} ECO_PARAM;

class CEcoSystem  
{
public:
	CEcoSystem();
	virtual ~CEcoSystem();
	ECO_PARAM eco_param;
	float CalcEcoWeight(float fElevation, float fRelativeElevation, float fSlope);

protected:
	
};

#endif // !defined(AFX_ECOSYSTEM_H__107FDFF3_252B_42B3_9FFE_3645EEF70200__INCLUDED_)
