// F4Map2CSPDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "F4Map2CSP.h"
#include "F4Map2CSPDlg.h"
#include "F4Map.h"
#include "F4TerrainBlock.h"
#include "F4TerrainLevel.h"
#include "TextOutputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CCommandLineInfo cmdInfo;
CF4Map *m_pF4Map;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg-Dialogfeld für Anwendungsbefehl "Info"

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogfelddaten
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// Keine Nachrichten-Handler
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CF4Map2CSPDlg Dialogfeld

CF4Map2CSPDlg::CF4Map2CSPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CF4Map2CSPDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CF4Map2CSPDlg)
	m_cFileNameSrc = _T("");
	m_cFileNameDest = _T("");
	m_iLevel = -1;
	//}}AFX_DATA_INIT
	// Beachten Sie, dass LoadIcon unter Win32 keinen nachfolgenden DestroyIcon-Aufruf benötigt
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CF4Map2CSPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CF4Map2CSPDlg)
	DDX_Text(pDX, IDC_EDIT_FILE_SRC, m_cFileNameSrc);
	DDX_Text(pDX, IDC_EDIT_FILE_DEST, m_cFileNameDest);
	DDX_Radio(pDX, IDC_RADIO_L0, m_iLevel);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CF4Map2CSPDlg, CDialog)
	//{{AFX_MSG_MAP(CF4Map2CSPDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_BROWSE_SRC, OnBrowseSrc)
	ON_BN_CLICKED(ID_BROWSE_DEST, OnBrowseDest)
	ON_BN_CLICKED(ID_CONVERT, OnConvert)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CF4Map2CSPDlg Nachrichten-Handler

BOOL CF4Map2CSPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Hinzufügen des Menübefehls "Info..." zum Systemmenü.

	// IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{	
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	// Quick and dirty hack: check if there's a map file name
	// specified as a command line parameter 
	// if yes, extract the l2 map to the same directory and quit

	if(!cmdInfo.m_strFileName.IsEmpty())
	{ 

		m_cFileNameDest = cmdInfo.m_strFileName;
		m_cFileNameSrc = m_cFileNameDest;
		m_cFileNameDest.Replace("map","raw");
		m_iLevel = 2;

		m_pF4Map = new CF4Map();
		m_pF4Map->Setup(m_cFileNameSrc);
		m_pF4Map->ConvertMAPtoRAW(m_cFileNameDest, m_iLevel);

		if(m_pF4Map)
		{
			m_pF4Map->Cleanup();
			delete(m_pF4Map);
		}
		
		PostMessage(WM_QUIT);
	}
	
	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

void CF4Map2CSPDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// Wollen Sie Ihrem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch für Sie erledigt.

void CF4Map2CSPDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext für Zeichnen

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Symbol in Client-Rechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// Die Systemaufrufe fragen den Cursorform ab, die angezeigt werden soll, während der Benutzer
//  das zum Symbol verkleinerte Fenster mit der Maus zieht.
HCURSOR CF4Map2CSPDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CF4Map2CSPDlg::OnBrowseSrc() 
{
	CFileDialog	FileDialog(TRUE, NULL, NULL,
						   OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
						   "MAP - Falcon 4.0 Terrain Map|*.map|");

	FileDialog.m_ofn.lpstrTitle="Select Falcon4 Map file";
	if (FileDialog.DoModal() == IDOK)
	{
		m_cFileNameSrc=FileDialog.GetPathName();
		UpdateData(FALSE);
	}	
}

void CF4Map2CSPDlg::OnBrowseDest() 
{
	CFileDialog	FileDialog(TRUE, NULL, NULL,
						   OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
						   "RAW - 16bit RAW file|*.raw|");

	FileDialog.m_ofn.lpstrTitle="Select CSP Scenary file";
	if (FileDialog.DoModal() == IDOK)
	{
		m_cFileNameDest=FileDialog.GetPathName();
		m_cFileNameDest.MakeLower();
		if(m_cFileNameDest.Find(".raw") == -1)
		   m_cFileNameDest += ".raw";
		UpdateData(FALSE);
	}	
}

BOOL CF4Map2CSPDlg::OnConvert() 
{
	CDialog::OnOK();

	if(m_cFileNameSrc.IsEmpty()) 
	{
		MessageBox("Please select a source f4-map file to convert", NULL, MB_OK);
		return FALSE;
	}

	if(m_cFileNameDest.IsEmpty()) 
	{
		MessageBox("Please select a destination file", NULL, MB_OK);
		return FALSE;
	}

	m_pF4Map = new CF4Map();
	m_pF4Map->Setup(m_cFileNameSrc);
	m_pF4Map->ConvertMAPtoRAW(m_cFileNameDest, m_iLevel);

	if(m_pF4Map)
	{
		m_pF4Map->Cleanup();
		delete(m_pF4Map);
	}

	MessageBox("Conversion successful!", NULL, MB_OK);
	return TRUE;
}
