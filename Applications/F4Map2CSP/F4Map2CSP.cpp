// F4Map2CSP.cpp : Legt das Klassenverhalten für die Anwendung fest.
//

#include "stdafx.h"
#include "F4Map2CSP.h"
#include "F4Map2CSPDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCommandLineInfo cmdInfo;

/////////////////////////////////////////////////////////////////////////////
// CF4Map2CSPApp

BEGIN_MESSAGE_MAP(CF4Map2CSPApp, CWinApp)
	//{{AFX_MSG_MAP(CF4Map2CSPApp)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CF4Map2CSPApp Konstruktion

CF4Map2CSPApp::CF4Map2CSPApp()
{
	// ZU ERLEDIGEN: Hier Code zur Konstruktion einfügen
	// Alle wichtigen Initialisierungen in InitInstance platzieren
}

/////////////////////////////////////////////////////////////////////////////
// Das einzige CF4Map2CSPApp-Objekt

CF4Map2CSPApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CF4Map2CSPApp Initialisierung

BOOL CF4Map2CSPApp::InitInstance()
{

#ifdef _AFXDLL
	Enable3dControls();			// Diese Funktion bei Verwendung von MFC in gemeinsam genutzten DLLs aufrufen
#else
	Enable3dControlsStatic();	// Diese Funktion bei statischen MFC-Anbindungen aufrufen
#endif

	CF4Map2CSPDlg dlg;
	m_pMainWnd = &dlg;

	ParseCommandLine(cmdInfo);

	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// ZU ERLEDIGEN: Fügen Sie hier Code ein, um ein Schließen des
		//  Dialogfelds über OK zu steuern
	}
	else if (nResponse == IDCANCEL)
	{
		// ZU ERLEDIGEN: Fügen Sie hier Code ein, um ein Schließen des
		//  Dialogfelds über "Abbrechen" zu steuern
	}

	// Da das Dialogfeld geschlossen wurde, FALSE zurückliefern, so dass wir die
	//  Anwendung verlassen, anstatt das Nachrichtensystem der Anwendung zu starten.
	return FALSE;
}
