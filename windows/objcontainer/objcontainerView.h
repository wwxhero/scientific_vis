
// objcontainerView.h : interface of the CobjcontainerView class
//

#pragma once
#include "TOpenGLView.h"

class CobjcontainerView : public CView
						, protected TOpenGLView<CobjcontainerView>
{
protected: // create from serialization only
	CobjcontainerView();
	DECLARE_DYNCREATE(CobjcontainerView)

// Attributes
public:
	CobjcontainerDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDestroy();
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CobjcontainerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Overides TOpenGLFuncs
protected:
	virtual int OnGLCreate();
	virtual void OnGLDraw();
	virtual void OnUpdateGLData();

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in objcontainerView.cpp
inline CobjcontainerDoc* CobjcontainerView::GetDocument() const
   { return reinterpret_cast<CobjcontainerDoc*>(m_pDocument); }
#endif

