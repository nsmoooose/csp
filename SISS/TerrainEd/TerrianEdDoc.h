// TerrianEdDoc.h : interface of the CTerrianEdDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERRIANEDDOC_H__5B2484DC_BE4A_435A_8479_E3A4F407BD4C__INCLUDED_)
#define AFX_TERRIANEDDOC_H__5B2484DC_BE4A_435A_8479_E3A4F407BD4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CTerrianEdDoc : public CDocument
{
protected: // create from serialization only
	CTerrianEdDoc();
	DECLARE_DYNCREATE(CTerrianEdDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerrianEdDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTerrianEdDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTerrianEdDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERRIANEDDOC_H__5B2484DC_BE4A_435A_8479_E3A4F407BD4C__INCLUDED_)
