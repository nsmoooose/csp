// F4Map2CSP.h : Haupt-Header-Datei für die Anwendung F4MAP2CSP
//

#if !defined(AFX_F4MAP2CSP_H__D7F598A3_F51F_4392_9988_762993C9AE4C__INCLUDED_)
#define AFX_F4MAP2CSP_H__D7F598A3_F51F_4392_9988_762993C9AE4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CF4Map2CSPApp:
// Siehe F4Map2CSP.cpp für die Implementierung dieser Klasse
//

class CF4Map2CSPApp : public CWinApp
{
public:
	CF4Map2CSPApp();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CF4Map2CSPApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CF4Map2CSPApp)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_F4MAP2CSP_H__D7F598A3_F51F_4392_9988_762993C9AE4C__INCLUDED_)
