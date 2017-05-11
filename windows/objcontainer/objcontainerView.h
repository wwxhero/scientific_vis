
// objcontainerView.h : interface of the CobjcontainerView class
//

#pragma once
#include "TOpenGLView.h"
#include "libobj.h"

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
	virtual void OnDraw(CDC* pDC);  // overridden to draw this m_view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnInitialUpdate(); // called first time after construct

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
	virtual void OnUpdateGLData(CObject3D* pObj);
	virtual void OnGLSize(int cx, int cy);
	virtual void OnGLDestroy(CObject3D* pObj = NULL);
	virtual void OnGLInitialUpdate();

private:
	void DrawScene(CScene* pScene);

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnSnapShot();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in objcontainerView.cpp
inline CobjcontainerDoc* CobjcontainerView::GetDocument() const
   { return reinterpret_cast<CobjcontainerDoc*>(m_pDocument); }
#endif

