///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   definition of class CConfig
//   reads and provides configuration data
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	9/26/2001	created file and implemented functionality - zzed
//
//
//
///////////////////////////////////////////////////////////////////////////

#include "Config.h"

#include <assert.h>

#include "global.h"

using namespace std;


CConfig Config;

CConfig::CConfig()
{
}

void CConfig::ReadConfigFile(const char* cFilename)
{
	const int	iMaxFileLen			= 10240;		// cfg-file's size limited to 10kb
	char		sConfigData[iMaxFileLen+1]; 
	FILE		*pFile;

	m_oConfigData.clear();

	int i;

	// read cfg-file into buffer
	for (i=0; i<iMaxFileLen+1; i++) sConfigData[i] = 0;
	pFile = fileopen(cFilename, "r");
	int iBytesRead = fread(sConfigData, iMaxFileLen, 1, pFile);
	fclose(pFile);
	if (iBytesRead==iMaxFileLen)
	{
		printf("\nERROR: config-file length exceeds maximum (%d bytes)", iMaxFileLen);
		exit(-1);
	}

	// analyze buffer and store parameters in map
	i = 0;
	uint k;
	char cParamName[255];
	char cParamValue[255];
	
	while (sConfigData[i] != 0)
	{
		if (sConfigData[i]=='<')					// read the string between < and >
		{
			i++;
			k = 0;
			while (sConfigData[i] != '>')
			{
				cParamName[k] = sConfigData[i];
				i++;
				k++;
			}
			cParamName[k] = 0;							// terminate the string with 0x00
			if (!strcmp(cParamName, "EOF"))	return;		// End of File reached?
			// ok, new parameter found, extract value
			k = 0;
			i += 2;									// skip the CR + LF bytes
			while (sConfigData[i]!='\n')			// read the value, terminated by a LF
			{
				cParamValue[k] = sConfigData[i];
				i++;
				k++;
			}
			cParamValue[k] = 0;						// terminate the string with 0x00
			// store data in map
			string sName = cParamName;
			string sValue = cParamValue;
			m_oConfigData.insert(ConfigData::value_type(sName, sValue));
		}
		i++;
	};
}

void CConfig::GetString(char* cValue, const char* cParam)
{
	assert(m_oConfigData.find(cParam)!=m_oConfigData.end() && "specified parameter not found!");
	//const char *text = m_oConfigData[cParam].c_str();
	strcpy(cValue, m_oConfigData[cParam].c_str());
}

int CConfig::GetInteger(const char* cParam)
{
	return atoi(m_oConfigData[cParam].c_str());
}

float CConfig::GetFloat(const char* cParam)
{
	return (float)atof(m_oConfigData[cParam].c_str());
}
