#include "StdAfx.h"
#include "Scene.h"

IMPLEMENT_SERIAL(CScene, CObject3D, 1)
CScene::CScene(void)
{
	 m_strName = _T("Scene");
}


CScene::~CScene(void)
{
}

void CScene::glDraw(const Matrix4x4& w2v, const Matrix4x4& v2c)
{
	glClearColor(0, 1, 0, 0);
	glClearDepth(1);
	// Clear the screne

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
