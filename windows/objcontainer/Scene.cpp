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
