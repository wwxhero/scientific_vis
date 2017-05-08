// PropertyGridCtrlObject3D.cpp : implementation file
//

#include "stdafx.h"
#include <list>
#include "objcontainer.h"
#include "PropertyGridCtrlObject3D.h"
#include "PropertyGridObject3D.h"
#include "PropertyGridScene.h"
#include "Scene.h"
#include "PropertyGridBox.h"
#include "Box.h"
#include "PropertyGridWF.h"
#include "ObjectWaveFront.h"



// CPropertyGridCtrlObject3D

IMPLEMENT_DYNAMIC(CPropertyGridCtrlObject3D, CMFCPropertyGridCtrl)

CPropertyGridCtrlObject3D::CPropertyGridCtrlObject3D()
{
	c_mapObj2Prop[RUNTIME_CLASS(CObject3D)] = RUNTIME_CLASS(CPropertyGridObject3D);
	c_mapObj2Prop[RUNTIME_CLASS(CScene)] = RUNTIME_CLASS(CPropertyGridScene);
	c_mapObj2Prop[RUNTIME_CLASS(CBox)] = RUNTIME_CLASS(CPropertyGridBox);
	c_mapObj2Prop[RUNTIME_CLASS(CObjectWaveFront)] = RUNTIME_CLASS(CPropertyGridWF);
}

CPropertyGridCtrlObject3D::~CPropertyGridCtrlObject3D()
{
}


BEGIN_MESSAGE_MAP(CPropertyGridCtrlObject3D, CMFCPropertyGridCtrl)
END_MESSAGE_MAP()

void CPropertyGridCtrlObject3D::Init(CObject3D* pObj)
{
	EnableHeaderCtrl(FALSE);
	EnableDescriptionArea();
	SetVSDotNetLook();
	MarkModifiedProperties();
	RemoveAll();

	std::list<CRuntimeClass*> lstHieracky;
	CRuntimeClass* pRtcObj = pObj->GetRuntimeClass();
	CRuntimeClass* pRtcObjRoot = CRuntimeClass::FromName("CObject3D");
	bool hitRoot = false;
	while(!hitRoot)
	{
		lstHieracky.push_front(pRtcObj);
		hitRoot = (pRtcObj == pRtcObjRoot);
#ifdef _AFXDLL
        pRtcObj = pRtcObj->m_pfnGetBaseClass();
#else
        pRtcObj = pRtcObj->m_pBaseClass;
#endif
	}

	CString strNameProp;
	for (std::list<CRuntimeClass*>::iterator it = lstHieracky.begin(); it != lstHieracky.end(); it ++)
	{
		pRtcObj = *it;
		CRuntimeClass* pRtcPropG = c_mapObj2Prop[pRtcObj];
		ATLASSERT(NULL != pRtcPropG);
		CPropertyGridGroup* pProperty = static_cast<CPropertyGridGroup*>(pRtcPropG->CreateObject());
		strNameProp.Format(_T("%s"), CA2T(pRtcObj->m_lpszClassName));
		pProperty->SetName(strNameProp);
		pProperty->Init(pObj);
		AddProperty(pProperty, FALSE, FALSE);
	}
	AdjustLayout();
	Update(pObj);
}
bool CPropertyGridCtrlObject3D::Update(CObject3D* pObj, bool bObj2Property)
{
	POSITION pos = m_lstProps.GetHeadPosition();
	bool updated = true;
	while(pos && updated)
	{
		CPropertyGridGroup* pg = static_cast<CPropertyGridGroup*>(m_lstProps.GetNext(pos));
		updated = pg->Update(pObj, bObj2Property);
	}
	return updated;
}


// CPropertyGridCtrlObject3D message handlers


