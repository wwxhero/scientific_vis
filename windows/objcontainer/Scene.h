#pragma once
#include "object3d.h"
class CScene :
	public CObject3D
{
public:
	DECLARE_SERIAL( CScene )
	CScene(void);
	virtual ~CScene(void);
	virtual void GetName(CString& strName) const
	{
		strName = _T("Scene");
	}
	void Reset()
	{
		m_firstChild = NULL;
		m_nextSibbling = NULL;
		m_parent = NULL;
		m_local2parent = Matrix4x4(1);
		m_parent2local = Matrix4x4(1);
	}

	void Reset(const CScene* src)
	{
		m_firstChild = src->m_firstChild;
		m_local2parent = src->m_local2parent;
		m_nextSibbling = src->m_nextSibbling;
		m_parent = src->m_parent;
		m_parent2local = src->m_parent2local;
	}

	virtual void RemoveSelf()
	{
	}
};

