// EcoSystem.cpp: Implementierung der Klasse CEcoSystem.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "math.h"
#include "EcoSystem.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CEcoSystem::CEcoSystem()
{

}

CEcoSystem::~CEcoSystem()
{

}

float CEcoSystem::CalcEcoWeight(float fElevation, float fRelativeElevation, float fSlope)
{
	float fProbElevation, fProbRelElevation, fProbSlope, fX, fAVS;
	float fRangeElevation, fRangeRelElevation, fRangeSlope;
	float fResult;


	// probability due to elevation
/*	fRangeElevation = eco_param.fElevationMax - eco_param.fElevationMin;
	fAVS = eco_param.fElevationMax + eco_param.fElevationMin;
	fX = fabs((2.0 * (fElevation - fAVS)) / fRangeElevation);
	fProbElevation = pow(0.5, pow(fX, eco_param.fElevationSharpness));
*/
	fRangeElevation = (eco_param.fElevationMax - eco_param.fElevationMin)/2;
	fAVS = (eco_param.fElevationMax + eco_param.fElevationMin) / 2;
	fX = fabs((fElevation - fAVS) / fRangeElevation);
	fProbElevation = pow(0.5, pow(fX, eco_param.fElevationSharpness));

	// probability due to relative elevation
/*	fRangeRelElevation = eco_param.fRelativeElevationMax - eco_param.fRelativeElevationMin;
	fAVS = (eco_param.fRelativeElevationMax + eco_param.fRelativeElevationMin);
	fX = fabs((2.0 * (fRelativeElevation - fAVS)) / fRangeRelElevation);
	fProbRelElevation = pow(0.5, pow(fX, eco_param.fRelativeElevationSharpness));
*/
	fRangeRelElevation = (eco_param.fRelativeElevationMax - eco_param.fRelativeElevationMin)/2;
	fAVS = (eco_param.fRelativeElevationMax + eco_param.fRelativeElevationMin)/2;
	fX = fabs((fRelativeElevation - fAVS) / fRangeRelElevation);
	fProbRelElevation = pow(0.5, pow(fX, eco_param.fRelativeElevationSharpness));

	// probability due to slope
/*	fRangeSlope = eco_param.fSlopeMax - eco_param.fSlopeMin;
	fAVS = (eco_param.fSlopeMax + eco_param.fSlopeMin);
	fX = fabs((2.0 * (fSlope - fAVS)) / fRangeSlope);
	fProbSlope = pow(0.5, pow(fX, eco_param.fSlopeSharpness));
*/
	// probability due to slope
	fRangeSlope = (eco_param.fSlopeMax - eco_param.fSlopeMin)/2;
	fAVS = (eco_param.fSlopeMax + eco_param.fSlopeMin)/2;
	fX = fabs((fSlope - fAVS) / fRangeSlope);
	fProbSlope = pow(0.5, pow(fX, eco_param.fSlopeSharpness));


	fResult = fProbElevation * fProbRelElevation * fProbSlope;
	return fResult;
}

