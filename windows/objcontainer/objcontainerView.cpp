
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

// glm::vec3, glm::vec4, glm::ivec4, glm::mat4
#include <glm/glm.hpp>
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

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
	ON_WM_SIZE()
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

// CobjcontainerView drawing
void CobjcontainerView::OnGLDraw()
{
	TRACE(_T("CobjcontainerView::OnGLDraw\n"));
	// glClearColor(0, 1, 0, 0);
	glClearColor(0, 1, 0, 0);
	glClearDepth(1);
	// Clear the screne

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	model = glm::mat4(1.0f);
	glm::mat4 xRotMat = glm::rotate(glm::mat4(1.0f), 0.0f, glm::normalize(glm::vec3(glm::inverse(model) * glm::vec4(1, 0, 0, 1))) );
	model = model * xRotMat;
	glm::mat4 yRotMat = glm::rotate(glm::mat4(1.0f), 0.0f, glm::normalize(glm::vec3(glm::inverse(model) * glm::vec4(0, 1, 0, 1))) );
	model = model * yRotMat;

	glm::mat4 modelViewMatrix = view * model;
	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(modelViewMatrix)); // Normal Matrix

	// Use our shader
	glUseProgram(programID);
	// Send the model, view and projection matrices to the shader
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix3fv( glGetUniformLocation(programID, "normalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	glmDrawVBO(objModel, programID);

	TRACE(_T("programID:%d, modelID:%d, projectionID:%d\n"), programID, modelID, projectionID);

	TRACE(_T("\nmodel matrix\n"));
	DumpMatrix4x4(model);
	TRACE(_T("\nview matrix\n"));
	DumpMatrix4x4(view);
	TRACE(_T("\nProjection matrx\n"));
	DumpMatrix4x4(projection);
	TRACE(_T("\nnormalMatrix\n"));
	DumpMatrix3x3(normalMatrix);

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
	glmDelete(objModel);
}

int CobjcontainerView::OnGLCreate()
{
	TRACE(_T("CobjcontainerView::OnGLCreate\n"));
	//objModel = glmReadOBJ("E:\\Users\\wanxwang\\scientific_vis\\course_project\\objview\\data\\al.obj");
	objModel = glmReadOBJ("al.obj");
	if (!objModel) return 1;

	// Normilize vertices
	glmUnitize(objModel);
	// Compute facet normals
	glmFacetNormals(objModel);
	// Comput vertex normals
	glmVertexNormals(objModel, 90.0);
	// Load the model (vertices and normals) into a vertex buffer
	glmLoadInVBO(objModel);

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
	programID = LoadShaders( "../shaders/vertShader.glsl", "../shaders/fragShader.glsl" );

	// Get a handle for our model, view and projection uniforms
	modelID = glGetUniformLocation(programID, "model");
	viewID = glGetUniformLocation(programID, "view");
	projectionID = glGetUniformLocation(programID, "projection");

	glm::vec4 light_ambient = glm::vec4( 0.1, 0.1, 0.1, 0.5 );
	glm::vec4 light_diffuse = glm::vec4 ( 0.8, 1.0, 1.0, 1.0 );
	glm::vec4 light_specular = glm::vec4( 0.8, 1.0, 1.0, 1.0 );

	glUseProgram(programID);
	glUniform4fv( glGetUniformLocation(programID, "light_ambient"), 1, &light_ambient[0]);
	glUniform4fv( glGetUniformLocation(programID, "light_diffuse"), 1, &light_diffuse[0]);
	glUniform4fv( glGetUniformLocation(programID, "light_specular"), 1, &light_specular[0]);

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	view = glm::lookAt( glm::vec3(0, 0, 3), // Camera position in World Space
	                    glm::vec3(0, 0, 0), // and looks at the origin
	                    glm::vec3(0, 1, 0) // Head is up (set to 0,-1,0 to look upside-down)
	                  );
	// Model matrix : an identity matrix (model will be at the origin)
	model      = glm::mat4(1.0f);

	// Initialize a light
	glm::vec4 lightPosition = glm::vec4(-20, -10, 0, 0);
	glUniform4fv( glGetUniformLocation(programID, "lightPos"), 1, &lightPosition[0]);
	return 0;
}

void CobjcontainerView::OnGLSize(int cx, int cy)
{
	GLfloat aspectRatio = GLfloat(cx)/cy;
	projection = glm::perspective(45.0f, aspectRatio, 0.1f, 100.0f);
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
