///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//  type definitions
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	9/15/2001	created file  - zzed
//  9/15/2001	added function fileopen - zzed
//
//
//
///////////////////////////////////////////////////////////////////////////

#include<algorithm>

#include "LogStream.h"

#include "global.h"

// catches non existent files to open, displays an error and exits the program
FILE* fileopen(const char *cFilename, const char *cMode)
{

	//CSP_LOG(CSP_TERRAIN, CSP_TRACE, "fileopen() - " << cFilename);

	FILE *f = fopen(cFilename, cMode);
	
	if (f==0)
	{ 
		printf("\n\n\nerror while opening file '%s'! Exiting program ....\n\n", cFilename);
		exit(-1);
	}

	return f;
}

void ConvertStringToUpper(string & str)
{
    transform(str.begin(), str.end(), str.begin(), ::toupper);

}

StringTokenizer::StringTokenizer(const string &rStr, const string &rDelimiters)
{
	string::size_type lastPos(rStr.find_first_not_of(rDelimiters, 0));
	string::size_type pos(rStr.find_first_of(rDelimiters, lastPos));
	while (string::npos != pos || string::npos != lastPos)
	{
		push_back(rStr.substr(lastPos, pos - lastPos));
		lastPos = rStr.find_first_not_of(rDelimiters, pos);
		pos = rStr.find_first_of(rDelimiters, lastPos);
	}
}
