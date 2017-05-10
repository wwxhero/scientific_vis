
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
	ON_COMMAND(ID_BTNLEFT, &CobjcontainerView::OnLeftTurn)
	ON_COMMAND(ID_BTNRIGHT, &CobjcontainerView::OnRightTurn)
	ON_COMMAND(ID_EDIT_SNAPSHOT, &CobjcontainerView::OnSnapShot)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CobjcontainerView construction/destruction

CobjcontainerView::CobjcontainerView() : m_objModel(NULL)
									   , m_degRotY(0)
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

		//n->glDraw(world2view, view2clip);// this causes the shader program to switch frequently, performance therefore sacrifies.
		n->glDraw(m_view, m_projection);

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

void CobjcontainerView::OnDraw(CDC* pDC)
{
	TOpenGLView::OnDraw(pDC);
}

void CobjcontainerView::OnUpdateGLData()
{
	TRACE(_T("OnUpdateGLData\n"));
	if (NULL != m_objModel)
	{
		glmDelete(m_objModel);
		m_objModel = NULL;
	}
	CobjcontainerDoc *pDoc = GetDocument();
	LPCTSTR filePath = pDoc->GetPathName();
	static LPCTSTR s_filePath = NULL;
	if (filePath != s_filePath
		&& *filePath != _T('\0'))
	{
		m_objModel = glmReadOBJ(const_cast<char*>(filePath));
		// Normilize vertices
		glmUnitize(m_objModel);
		// Compute facet normals
		glmFacetNormals(m_objModel);
		// Comput vertex normals
		glmVertexNormals(m_objModel, 90.0);
		// Load the m_model (vertices and normals) into a vertex buffer
		glmLoadInVBO(m_objModel);
		m_degRotY = 0;
		s_filePath = filePath;
	}
}



void CobjcontainerView::OnDestroy()
{
	CView::OnDestroy();
	TOpenGLView::OnDestroy();
	if (m_objModel)
	{
		glmDelete(m_objModel);
		m_objModel = NULL;
	}
}
void CobjcontainerView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CView::OnUpdate(pSender, lHint, pHint);
	//UpdateGLData();
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

	// TODO: Make sure that this is trully unnecessary
	//GLuint VertexArrayID;
	//glGenVertexArrays(1, &VertexArrayID);
	//glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	CString strPath;
	theApp.GetModuleDirPath(strPath);

	m_programID = LoadShaders( strPath + _T("vertShader.glsl"), strPath + _T("fragShader.glsl" ));

	// Get a handle for our m_model, m_view and projection uniforms
	m_modelID = glGetUniformLocation(m_programID, "model2world");
	m_viewID = glGetUniformLocation(m_programID, "world2view");
	m_projectionID = glGetUniformLocation(m_programID, "view2clip");

	glm::vec4 light_ambient = glm::vec4( 0.1, 0.1, 0.1, 0.5 );
	glm::vec4 light_diffuse = glm::vec4 ( 0.8, 1.0, 1.0, 1.0 );
	glm::vec4 light_specular = glm::vec4( 0.8, 1.0, 1.0, 1.0 );

	glUseProgram(m_programID);
	glUniform4fv( glGetUniformLocation(m_programID, "light_ambient"), 1, &light_ambient[0]);
	glUniform4fv( glGetUniformLocation(m_programID, "light_diffuse"), 1, &light_diffuse[0]);
	glUniform4fv( glGetUniformLocation(m_programID, "light_specular"), 1, &light_specular[0]);

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	m_projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	m_view = glm::lookAt( glm::vec3(0, 0, 3), // Camera position in World Space
	                    glm::vec3(0, 0, 0), // and looks at the origin
	                    glm::vec3(0, 1, 0) // Head is up (set to 0,-1,0 to look upside-down)
	                  );
	// Model matrix : an identity matrix (m_model will be at the origin)
	m_model      = glm::mat4(1.0f);

	// Initialize a light
	glm::vec4 lightPosition = glm::vec4(-20, -10, 0, 0);
	glUniform4fv( glGetUniformLocation(m_programID, "lightPos"), 1, &lightPosition[0]);
	return 0;
}

void CobjcontainerView::OnGLSize(int cx, int cy)
{
	GLfloat aspectRatio = GLfloat(cx)/cy;
	m_projection = glm::perspective(45.0f, aspectRatio, 0.1f, 100.0f);
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

void CobjcontainerView::OnLeftTurn()
{
	m_degRotY -= 0.3146f;
	Invalidate();
}

void CobjcontainerView::OnRightTurn()
{
	m_degRotY += 0.3146f;
	Invalidate();
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
