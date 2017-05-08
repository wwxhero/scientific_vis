#include "StdAfx.h"
#include "PropertyGridGroup.h"

IMPLEMENT_DYNCREATE(CPropertyGridGroup, CMFCPropertyGridProperty)
CPropertyGridGroup::CPropertyGridGroup(void) : CMFCPropertyGridProperty(_T(""))
{
}


CPropertyGridGroup::~CPropertyGridGroup(void)
{
}

void CPropertyGridGroup::Cnn(const CObject3D* pObj)
{
	//TODO: over write this function in derived classes to set up connection from property item to data
}