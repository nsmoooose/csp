// Log.cpp: implementation of the CLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Log.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CLog construction/destruction

CLog::CLog ( char *szPath ) 
{
	// if nothing was put use the default
	if ( szPath == '\0' )
		m_szPath = "logs\\log.txt";
	else
		m_szPath = szPath;

	m_fLog = fopen ( m_szPath, "w+" ); // destroy anything existing
	if ( m_fLog ) {
		fprintf ( m_fLog, "Logging System Initialized \n" );
		fclose ( m_fLog );
	} else
		MessageBox( NULL,"Error Starting Logging System","ERROR",MB_OK );
		
	m_fLog = '\0';
}


CLog::~CLog() 
{
	Log ( "\nShutting Down Logging System" );
	Close ();
} 


/////////////////////////////////////////////////////////////////////////////
// CLog Open

void CLog::Open() 
{
	// if it isnt already opened, open the file for adding on to

	if ( !m_fLog ) {
		m_fLog = fopen ( m_szPath, "a" );
	}
} 


/////////////////////////////////////////////////////////////////////////////
// CLog Close

void CLog::Close() 
{
	// if the file is open, close it
	if ( m_fLog ) {
		fclose ( m_fLog );
		m_fLog = '\0';
	}
} 


/////////////////////////////////////////////////////////////////////////////
// CLog Log

void CLog::Log(char *szText, ...) 
{
	// open the file

	Open();

	if ( !m_fLog )							// error somewhere
		return;

	va_list arglist;
	char Text [ 1024 ];						// more than 1024 causes a buffer overrun
	memset ( Text, '\0', sizeof ( Text ) ); // clear out the memory

	va_start ( arglist, szText );
	vsprintf ( Text, szText, arglist );
	va_end ( arglist );
	fprintf ( m_fLog, "\n%s",Text );		// copy to the file
	Close ();								// and close the file
} 

