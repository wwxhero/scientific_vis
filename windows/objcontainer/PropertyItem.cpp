#include "StdAfx.h"
#include "PropertyItem.h"

IMPLEMENT_DYNAMIC(CPropertyItem, CMFCPropertyGridProperty)

CPropertyItem::CPropertyItem(const CString& strName, const _variant_t& v, Pipe pipe, LPCTSTR lpszDescr)
	: CMFCPropertyGridProperty(strName, v, lpszDescr)
	, m_pipe(pipe)
{
}


CPropertyItem::~CPropertyItem(void)
{
}

bool CPropertyItem::Update(CObject3D* pObj, bool bObj2Prop)
{
	_variant_t v;
	bool done = true;
	if (bObj2Prop)
	{
		done = m_pipe.get(pObj, v);
		if (done)
			SetValue(v);
	}
	else
	{
		v = GetValue();
		done = m_pipe.set(pObj, v);
	}
	return done;
}
