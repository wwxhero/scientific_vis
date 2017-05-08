#include "StdAfx.h"
#include "PropertyGridGroup.h"
#include "PropertyItem.h"

IMPLEMENT_DYNCREATE(CPropertyGridGroup, CMFCPropertyGridProperty)
CPropertyGridGroup::CPropertyGridGroup(void) : CMFCPropertyGridProperty(_T(""))
{
}


CPropertyGridGroup::~CPropertyGridGroup(void)
{
}

void CPropertyGridGroup::Init(const CObject3D* pObj)
{
	//TODO: over write this function in derived classes to set up connection from property item to data
}

bool CPropertyGridGroup::Update(CObject3D* pObj, bool bObj2Prop)
{
	POSITION pos = m_lstSubItems.GetHeadPosition();
	bool updated = true;
	while(pos && updated)
	{
		CPropertyItem *item = static_cast<CPropertyItem*>(m_lstSubItems.GetNext(pos));
		updated = item->Update(pObj, bObj2Prop);
	}
	return updated;
}