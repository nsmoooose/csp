// Log.h: interface for the CLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOG_H__34DEB2C0_A378_11D4_A3B0_0050DA731472__INCLUDED_)
#define AFX_LOG_H__34DEB2C0_A378_11D4_A3B0_0050DA731472__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h> // printf and va commands
#include <stdarg.h> // va commands
#include <string.h> // memset()
#include <windows.h> // MessageBox()


class CLog
{
public:
	CLog ( char *szPath ); // creates the file if it doesnt exist
	~CLog (); // if the file is open it closes it
	void Open ();
	void Close ();
	void Log ( char *szText, ... );

	FILE* m_fLog;
	char* m_szPath;
};

#endif // !defined(AFX_LOG_H__34DEB2C0_A378_11D4_A3B0_0050DA731472__INCLUDED_)
