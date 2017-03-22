#ifndef _TOPENGL_VIEW
#define _TOPENGL_VIEW
#pragma once
#include <GL/glew.h>
#include <GL/wglew.h>

template<typename TThis>
class TOpenGLView
{
protected:
	TOpenGLView() : m_hrc(NULL)
	{
	}
	~TOpenGLView()
	{
	}
private:
	virtual int OnGLCreate() = 0;
	virtual void OnGLDraw() = 0;
	virtual void OnUpdateGLData() = 0;
	virtual void OnGLSize(int cx, int cy) = 0;
protected:
	int OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		TThis* pThis = static_cast<TThis *>(this);
		PIXELFORMATDESCRIPTOR pfd;
		memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
		pfd.nSize  = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion   = 1;
		pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 32;
		pfd.iLayerType = PFD_MAIN_PLANE;

		CDC *pDC = pThis->GetDC();
		HDC hdc = pDC->m_hDC;

		int nPixelFormat = ChoosePixelFormat(hdc, &pfd);
		if (nPixelFormat != 0)
		{
			BOOL bResult = SetPixelFormat (hdc, nPixelFormat, &pfd);
			if (bResult)
			{
				HGLRC tempContext = wglCreateContext(hdc);
				wglMakeCurrent(hdc, tempContext);

				GLenum GlewInitResult;
				glewExperimental = GL_TRUE;
				GlewInitResult = glewInit();

				if (GlewInitResult != GLEW_OK)
				{
					AfxMessageBox(_T("GLEW is not initialized!"));
				}

				int attribs[] =
				{
					WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
					WGL_CONTEXT_MINOR_VERSION_ARB, 3,
					WGL_CONTEXT_FLAGS_ARB, 0,
					0
				};

				if (wglewIsSupported("WGL_ARB_create_context") == 1)
				{
					m_hrc = wglCreateContextAttribsARB(hdc, 0, attribs);
					wglMakeCurrent(NULL, NULL);
					wglDeleteContext(tempContext);
					wglMakeCurrent(hdc, m_hrc);
				}
				else
				{
					//It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
					m_hrc = tempContext;
				}

			}
		}

		int result = OnGLCreate();

		wglMakeCurrent(NULL, NULL);
		pThis->ReleaseDC(pDC);
		CString str;
		const GLubyte *version = glGetString(GL_VERSION);
		str.Format(_T("OpenGL version: %s\n"), version ? (CString)version : _T("can't get the OpengGL version"));
		TRACE(str);

		return result;
	}
	void OnDestroy()
	{
		wglMakeCurrent(NULL, NULL);
		if (m_hrc)
		{
			wglDeleteContext(m_hrc);
			m_hrc = NULL;
		}
	}
	void OnDraw(CDC *pDC)
	{
		if (NULL != m_hrc)
		{
			HDC hDC = pDC->m_hDC;
			if (!wglMakeCurrent(hDC, m_hrc))
			{
				AfxMessageBox(_T("failed to make current"));
				return;
			}

			OnGLDraw();

			glFlush();
			SwapBuffers(hDC);

			wglMakeCurrent(NULL, NULL);
		}
	}
	void UpdateGLData()
	{
		TThis* pThis = static_cast<TThis *>(this);
		CDC *pDC = pThis->GetDC();
		HDC hDC = pDC->m_hDC;
		wglMakeCurrent(hDC, m_hrc);
		OnUpdateGLData();
		wglMakeCurrent(NULL, NULL);
		pThis->ReleaseDC(pDC);
	}
	BOOL PreCreateWindow(CREATESTRUCT& cs)
	{
		cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_OWNDC, NULL, (HBRUSH)GetStockObject(WHITE_BRUSH), NULL); //default background colour
		cs.style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		return TRUE;
	}

	void OnSize(int cx, int cy)
	{
		TThis* pThis = static_cast<TThis *>(this);
		CDC *pDC = pThis->GetDC();
		HDC hDC = pDC->m_hDC;
		wglMakeCurrent(hDC, m_hrc);

		if (cx > 0 && cy > 0)
		{
			glViewport(0, 0, cx, cy);
			OnGLSize(cx, cy);
		}
		wglMakeCurrent(NULL, NULL);
		pThis->ReleaseDC(pDC);
	}
private:
	HGLRC m_hrc;
};
#endif