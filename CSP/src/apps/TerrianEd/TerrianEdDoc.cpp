// TerrianEdDoc.cpp : implementation of the CTerrianEdDoc class
//

#include "stdafx.h"
#include "TerrianEd.h"

#include "TerrianEdDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTerrianEdDoc

IMPLEMENT_DYNCREATE(CTerrianEdDoc, CDocument)

BEGIN_MESSAGE_MAP(CTerrianEdDoc, CDocument)
	//{{AFX_MSG_MAP(CTerrianEdDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerrianEdDoc construction/destruction

CTerrianEdDoc::CTerrianEdDoc()
{
	// TODO: add one-time construction code here

}

CTerrianEdDoc::~CTerrianEdDoc()
{
}

BOOL CTerrianEdDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CTerrianEdDoc serialization

void CTerrianEdDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTerrianEdDoc diagnostics

#ifdef _DEBUG
void CTerrianEdDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTerrianEdDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTerrianEdDoc commands
