#pragma once
#define IDX_NULL -1
template<typename TThis>
class CTreeNodePersist
{
public:
	CTreeNodePersist(void)
	{
		memset(m_idx, IDX_NULL, total * sizeof(int));
	}
	virtual ~CTreeNodePersist(void)
	{

	}

	void SaveTreeNode(CObArray& arr)
	{
		m_idx[self] = arr.GetCount();
		arr.Add(static_cast<TThis*>(this));
	}

	void RestoreTreeNode(CObArray& arr)
	{
		TThis* pThis = static_cast<TThis*>(this);
		if (IDX_NULL != m_idx[parent])
			pThis->m_parent = static_cast<TThis*>(arr.GetAt(m_idx[parent]));
		if (IDX_NULL != m_idx[firstchild])
			pThis->m_firstChild = static_cast<TThis*>(arr.GetAt(m_idx[firstchild]));
		if (IDX_NULL != m_idx[nextsibbling])
			pThis->m_nextSibbling = static_cast<TThis*>(arr.GetAt(m_idx[nextsibbling]));
	}
	void Serialize(CArchive& ar)
	{
		if (ar.IsStoring())
		{
			TThis* pThis = static_cast<TThis*>(this);
			if (NULL != pThis->m_parent)
				m_idx[parent] = pThis->m_parent->m_idx[self];
			if (NULL != pThis->m_firstChild)
				m_idx[firstchild] = pThis->m_firstChild->m_idx[self];
			if (NULL != pThis->m_nextSibbling)
				m_idx[nextsibbling] = pThis->m_nextSibbling->m_idx[self];
			for (int i = 0; i < total; i ++)
				ar << m_idx[i];
#ifdef TEST_SIERALIZATION
			ar << m_id;
#endif
		}
		else
		{
			for (int i = 0; i < total; i ++)
				ar >> m_idx[i];
#ifdef TEST_SIERALIZATION
			ar >> m_id;
#endif
		}
	}
private:
	enum INDEX {self = 0, parent, firstchild, nextsibbling, total};
	int m_idx[total];

#ifdef TEST_SIERALIZATION
public:
	void setNodeId(int id)
	{
		m_id = id;
	}
	int getNodeId() const
	{
		return m_id;
	}
private:
	int m_id;
#endif
};

