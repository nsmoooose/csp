///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   declaration of class CConfig
//   reads and provides configuration data
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	9/26/2001	created file and implemented functionality- zzed
//
//
//
///////////////////////////////////////////////////////////////////////////

#if !defined(CONFIG_H)
#define CONFIG_H

#pragma warning( disable : 4786 )	// disable warning about truncated characters in debug-info

#include <map>
#include <string>

typedef std::map<std::string, std::string>	ConfigData;



/**
 * class CConfig - Describe me!
 *
 * @author unknown
 */
class CConfig
{
public:
	CConfig();
	void ReadConfigFile(const char* cFilename);
	void GetString(char* cValue, const char* cParam);
	int GetInteger(const char* cParam);
	float GetFloat(const char* cParam);

private:
	ConfigData	m_oConfigData;

};

extern CConfig Config;

#endif
