// Definition of ECO_PARAM
// contains parameters an ecosystem is made of

typedef struct
{
	char cName[256];
	float fElevationMin;
	float fElevationMax;
	float fElevationSharpness;
	float fRelativeElevationMax;
	float fRelativeElevationMin;
	float fRelativeElevationSharpness;
	float fSlopeMin;
	float fSlopeMax;
	float fSlopeSharpness;
} ECO_PARAM;

