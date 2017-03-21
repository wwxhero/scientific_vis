
// objcontainerView.cpp : implementation of the CobjcontainerView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "objcontainer.h"
#endif

#include "objcontainerDoc.h"
#include "objcontainerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CobjcontainerView

IMPLEMENT_DYNCREATE(CobjcontainerView, CView)

BEGIN_MESSAGE_MAP(CobjcontainerView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CobjcontainerView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CobjcontainerView construction/destruction

CobjcontainerView::CobjcontainerView()
{
	// TODO: add construction code here

}

CobjcontainerView::~CobjcontainerView()
{
}

BOOL CobjcontainerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	return CView::PreCreateWindow(cs)
		&& TOpenGLView::PreCreateWindow(cs);
}

// CobjcontainerView drawing
void CobjcontainerView::OnGLDraw()
{
	TRACE(_T("CobjcontainerView::OnGLDraw\n"));
}

void CobjcontainerView::OnDraw(CDC* pDC)
{
	TOpenGLView::OnDraw(pDC);
}

void CobjcontainerView::OnUpdateGLData()
{
	TRACE(_T("OnUpdateGLData\n"));
}



void CobjcontainerView::OnDestroy()
{
	CView::OnDestroy();
	TOpenGLView::OnDestroy();
}

int CobjcontainerView::OnGLCreate()
{
	TRACE(_T("CobjcontainerView::OnGLCreate\n"));
	return 0;
}

int CobjcontainerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	int result = CView::OnCreate(lpCreateStruct);
	if (0 == result)
		result = TOpenGLView::OnCreate(lpCreateStruct);
	return result;
}

BOOL CobjcontainerView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

// CobjcontainerView printing


void CobjcontainerView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CobjcontainerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CobjcontainerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CobjcontainerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CobjcontainerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CobjcontainerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CobjcontainerView diagnostics

#ifdef _DEBUG
void CobjcontainerView::AssertValid() const
{
	CView::AssertValid();
}

void CobjcontainerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CobjcontainerDoc* CobjcontainerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CobjcontainerDoc)));
	return (CobjcontainerDoc*)m_pDocument;
}
#endif //_DEBUG


// CobjcontainerView message handlers
