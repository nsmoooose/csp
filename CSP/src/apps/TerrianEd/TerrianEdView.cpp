// TerrianEdView.cpp : implementation of the CTerrianEdView class
//

#include "stdafx.h"
#include "TerrianEd.h"

#include "TerrianEdDoc.h"
#include "TerrianEdView.h"
#include "InitEngineDlg.h"

#include "EngineStatus.h"
#include "glcode.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CEngineStatus	EngineStatus; 

/////////////////////////////////////////////////////////////////////////////
// CTerrianEdView

IMPLEMENT_DYNCREATE(CTerrianEdView, CView)

BEGIN_MESSAGE_MAP(CTerrianEdView, CView)
	//{{AFX_MSG_MAP(CTerrianEdView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerrianEdView construction/destruction

CTerrianEdView::CTerrianEdView()
{
	// TODO: add construction code here

}

CTerrianEdView::~CTerrianEdView()
{
}

BOOL CTerrianEdView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CS_OWNDC);

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CTerrianEdView drawing

void CTerrianEdView::OnDraw(CDC* pDC)
{
	/*CTerrianEdDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);*/
	// TODO: add draw code for native data here
/*	SetCamera(); */

	wglMakeCurrent(m_hDC, m_hRC);

	DrawGLScene();
	
	SwapBuffers(m_hDC);

	wglMakeCurrent(NULL, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CTerrianEdView diagnostics

#ifdef _DEBUG
void CTerrianEdView::AssertValid() const
{
	CView::AssertValid();
}

void CTerrianEdView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTerrianEdDoc* CTerrianEdView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTerrianEdDoc)));
	return (CTerrianEdDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTerrianEdView message handlers

int CTerrianEdView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CInitEngineDlg InitEngineDlg;

	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	int nPixelFormat;
	m_hDC = ::GetDC(m_hWnd);

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW |
			PFD_SUPPORT_OPENGL |
			PFD_DOUBLEBUFFER |
			PFD_TYPE_RGBA,
			32,
			0,0,0,0,0,0,
			0,0,
			0,0,0,0,0,
			16,
			0,
			0,
			PFD_MAIN_PLANE,
			0,
			0,0,0, };

	nPixelFormat = ChoosePixelFormat(m_hDC, &pfd);

	VERIFY(SetPixelFormat(m_hDC, nPixelFormat, &pfd));

	m_hRC = wglCreateContext(m_hDC);

	VERIFY(wglMakeCurrent(m_hDC, m_hRC));
	InitGL();
	InitEngineDlg.DoModal();
	LoadScene();
	wglMakeCurrent(NULL, NULL);
	SetTimer(101,500,NULL); // 2 times a second ...are enough... :-) 
	
	return 0;
}

void CTerrianEdView::OnDestroy() 
{
	wglDeleteContext(m_hRC);
	::ReleaseDC(m_hWnd, m_hDC);
	// Kill the timer
	KillTimer(101);

	KillGL();

	CView::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CTerrianEdView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	VERIFY(wglMakeCurrent(m_hDC, m_hRC));
	ReSizeGLScene(cx, cy);
	VERIFY(wglMakeCurrent(NULL, NULL));
}

BOOL CTerrianEdView::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
}

void CTerrianEdView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
/*	Invalidate(FALSE);  */

	Invalidate(FALSE);
	CView::OnTimer(nIDEvent);
} 

void CTerrianEdView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CView::OnKeyDown(nChar, nRepCnt, nFlags);

	if (nChar==VK_F3)
	{
		EngineStatus.m_fSpeed += 0.3f;
	}
	if (nChar==VK_F4)
	{
		EngineStatus.m_fSpeed -= 0.3f;
	}
	if (nChar==VK_F5)
	{
		EngineStatus.m_fSpeed = 0.0f;
	}
	if (nChar==VK_UP)
	{
		EngineStatus.m_roty -= 3.0f;
	}
	if (nChar==VK_DOWN)
	{
		EngineStatus.m_roty += 3.0f;
	}
	if (nChar==VK_LEFT)
	{
		EngineStatus.m_rotx += 3.0f;
	}
	if (nChar==VK_RIGHT)
	{
		EngineStatus.m_rotx -= 3.0f;
	}
	if (nChar==VK_SHIFT)
	{
		SetCamera();
	} 
	if (nChar==VK_TAB)													// easy way to move backwards
	{
		EngineStatus.m_fSpeed *= -1;
		SetCamera();
		EngineStatus.m_fSpeed *= -1;
	} 

	if (EngineStatus.m_rotx<0) EngineStatus.m_rotx += 360;				// assure that camera rot. doesn't
	else if (EngineStatus.m_rotx>360) EngineStatus.m_rotx -=360;		// get <0 and >360°
	if (EngineStatus.m_roty<0) EngineStatus.m_roty += 360;
	else if (EngineStatus.m_roty>360) EngineStatus.m_roty -= 360;

	Invalidate(FALSE);
}

void CTerrianEdView::OnFileOpen() 
{
	CInitEngineDlg InitEngineDlg;
	InitEngineDlg.DoModal();
	LoadScene();
}
