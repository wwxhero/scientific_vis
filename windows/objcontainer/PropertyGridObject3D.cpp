#include "StdAfx.h"
#include "PropertyGridObject3D.h"
#include "PropertyItem.h"
#include "Object3D.h"
IMPLEMENT_DYNCREATE(CPropertyGridObject3D, CPropertyGridGroup)
CPropertyGridObject3D::CPropertyGridObject3D(void)
{
}


CPropertyGridObject3D::~CPropertyGridObject3D(void)
{
}

void CPropertyGridObject3D::Init(const CObject3D* pObj)
{
	ATLASSERT(this->m_lstSubItems.IsEmpty());
	_variant_t vName;
	CObject3D::GetName(pObj, vName);
	Pipe pName = {CObject3D::SetName, CObject3D::GetName};

	struct _Initializer
	{
		LPCTSTR name;
		LPCTSTR descr;
		Pipe pipe;
	} parms[] = {
		{_T("Name"), _T("Name of the three-D object"), {CObject3D::SetName, CObject3D::GetName}}
	};

	for (int i = 0; i < sizeof(parms)/sizeof(_Initializer); i ++)
	{
		_variant_t v;
		parms[i].pipe.get(pObj, v);
		AddSubItem(new CPropertyItem(parms[i].name, v, parms[i].pipe, parms[i].descr));
	}


	//CPropertyItem(const CString& strName, const _variant_t& v,  Pipe pipe, LPCTSTR lpszDescr = NULL);
	//pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("3D Look"), (_variant_t) false, _T("Specifies the window's font will be non-bold and controls will have a 3D border")));
}
