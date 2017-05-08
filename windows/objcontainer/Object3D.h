#pragma once
#include "TreeNodePersist.h"
// CObject3D command target
class CObject3D : public CObject
				, public CTreeNodePersist<CObject3D>
{
	friend class CTreeNodePersist<CObject3D>;
	friend class CScene;
public:
	DECLARE_SERIAL( CObject3D )
	CObject3D();
	virtual ~CObject3D();
	void DumpData();

	void AddChild(CObject3D*);
	CObject3D* GetFirstChild() const
	{
		return m_firstChild;
	}
	CObject3D* GetNextSibbling() const
	{
		return m_nextSibbling;
	}
	void GetName(CString& strName) const
	{
		strName = m_strName;
	}
	virtual void RemoveSelf();

	static bool Connect(CObject3D* parent, CObject3D* child);

	static bool SetName(CObject3D* pThis, const _variant_t& vName);
	static bool GetName(const CObject3D* pThis, _variant_t& vName);

private:
	virtual void Serialize(CArchive& ar);

	Matrix4x4 m_local2parent;
	Matrix4x4 m_parent2local;

	CObject3D* m_parent;
	CObject3D* m_firstChild;
	CObject3D* m_nextSibbling;
protected:
	CString m_strName;
};


