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
//	7/9/2001	created file  - zzed
//  9/15/2001	added function fileopen - zzed
//
//
//
///////////////////////////////////////////////////////////////////////////

#if !defined(GLOBAL_H)
#define GLOBAL_H

//#include <stdio.h>
#include <string>
//#include <vector>
#include <deque>

using namespace std;

typedef unsigned int	uint;
typedef unsigned char	uchar;
typedef unsigned short	ushort;


FILE* fileopen(const char *cFilename, const char *cMode);

void ConvertStringToUpper(string & str);

class StringTokenizer : public deque<string>
{
	public:
		StringTokenizer(const string &rStr, const string &rDelimiters = " ,\n");
};



#endif

