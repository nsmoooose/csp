// TerrianEdView.h : interface of the CTerrianEdView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERRIANEDVIEW_H__2F4D217E_8264_4173_BEB1_3A75E58EEFB2__INCLUDED_)
#define AFX_TERRIANEDVIEW_H__2F4D217E_8264_4173_BEB1_3A75E58EEFB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CTerrianEdView : public CView
{
protected: // create from serialization only
	CTerrianEdView();
	DECLARE_DYNCREATE(CTerrianEdView)

// Attributes
public:
	CTerrianEdDoc* GetDocument();


	HGLRC	m_hRC;
	HDC		m_hDC;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerrianEdView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTerrianEdView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTerrianEdView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnFileOpen();
	afx_msg void OnViewMapinfo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in TerrianEdView.cpp
inline CTerrianEdDoc* CTerrianEdView::GetDocument()
   { return (CTerrianEdDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERRIANEDVIEW_H__2F4D217E_8264_4173_BEB1_3A75E58EEFB2__INCLUDED_)
