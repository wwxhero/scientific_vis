
// objcontainerView.cpp : implementation of the CobjcontainerView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "objcontainer.h"
#endif
#include <queue>
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
	ON_COMMAND(ID_EDIT_SNAPSHOT, &CobjcontainerView::OnSnapShot)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CobjcontainerView construction/destruction

CobjcontainerView::CobjcontainerView()
{

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

void DumpMatrix4x4(const glm::mat4& m)
{
	for (int i = 0; i < 4; i ++)
	{
		glm::vec4 v = m[i];
		TRACE(_T("\t\n%f %f %f %f"), v[0], v[1], v[2], v[3]);
	}
}

void DumpMatrix3x3(const glm::mat3& m)
{
	for (int i = 0; i < 3; i ++)
	{
		glm::vec3 v = m[i];
		TRACE(_T("\t\n%f %f %f"), v[0], v[1], v[2]);
	}
}

void CobjcontainerView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	CRect rc;
	GetClientRect(&rc);
	OnGLSize(rc.Width(), rc.Height());
}

void CobjcontainerView::DrawScene(CScene* pScene)
{
	CCamera* pCamera = pScene->GetCamera();
	const Matrix4x4& world2view = pCamera->World2View();
	const Matrix4x4& view2clip = pCamera->View2Clip();
	CObject3D* root = pScene;
	std::queue<CObject3D*> tq;
	tq.push(root);
	while(!tq.empty())
	{
		CObject3D* n = tq.front();
		tq.pop();

		n->glDraw(world2view, view2clip);// this causes the shader program to switch frequently, performance therefore sacrifies.
		//n->glDraw(m_view, m_projection);

		CObject3D* c = n->GetFirstChild();
		while(NULL != c)
		{
			tq.push(c);
			c = c->GetNextSibbling();
		}
	}
}

// CobjcontainerView drawing
void CobjcontainerView::OnGLDraw()
{
	CobjcontainerDoc* pDoc = GetDocument();
	CObject3D* pObj =  pDoc->RootObj();
	if (NULL != pObj)
	{
		ATLASSERT(pObj->IsKindOf(RUNTIME_CLASS(CScene)));
		DrawScene(static_cast<CScene*>(pObj));
	}
}

void CobjcontainerView::OnGLDestroy(CObject3D* pObj)
{
	CObject3D* root = (NULL == pObj ? GetDocument()->RootObj() : pObj);
	std::queue<CObject3D*> tq;
	tq.push(root);
	while(!tq.empty())
	{
		CObject3D* n = tq.front();
		tq.pop();

		n->glDestroy();
		//n->glDraw(m_view, m_projection);

		CObject3D* c = n->GetFirstChild();
		while(NULL != c)
		{
			tq.push(c);
			c = c->GetNextSibbling();
		}
	}
}

void CobjcontainerView::OnDraw(CDC* pDC)
{
	TOpenGLView::OnDraw(pDC);
}

void CobjcontainerView::OnUpdateGLData(CObject3D* pObj)
{
	pObj->glUpdate();
}

void CobjcontainerView::OnDestroy()
{
	CView::OnDestroy();
	TOpenGLView::OnDestroy();
}

void CobjcontainerView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CView::OnUpdate(pSender, lHint, pHint);
	if (NULL != pHint
		&& (pHint->IsKindOf(RUNTIME_CLASS(CObject3D))))
	{
		if (CobjcontainerDoc::OP_NEW == lHint)
			UpdateGLData(static_cast<CObject3D*>(pHint));
		else if (CobjcontainerDoc::OP_DEL == lHint)
			DestroyGLData(static_cast<CObject3D*>(pHint));
	}


}

int CobjcontainerView::OnGLCreate()
{
	TRACE(_T("CobjcontainerView::OnGLCreate\n"));

	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	return 0;
}

void CobjcontainerView::OnGLSize(int cx, int cy)
{
	CScene* scene = static_cast<CScene*>(GetDocument()->RootObj());
	if (scene)
	{
		CCamera* camera = scene->GetCamera();
		camera->OnGLSize(cx, cy);
	}
}


void CobjcontainerView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	TOpenGLView::OnSize(cx, cy);
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

void CobjcontainerView::OnSnapShot()
{
	DumpSnapShot();
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
