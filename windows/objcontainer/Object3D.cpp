// Object3D.cpp : implementation file
//

#include "stdafx.h"
#include <queue>
#include "objcontainer.h"
#include "Object3D.h"
#include "Scene.h"



// CObject3D
IMPLEMENT_SERIAL(CObject3D, CObject, 1)
CObject3D::CObject3D()
	: m_local2parent(1.0f)
	, m_parent2local(1.0f)
	, m_parent(NULL)
	, m_firstChild(NULL)
	, m_nextSibbling(NULL)
{
#ifdef TEST_SIERALIZATION
	for (int c = 0; c < 4; c ++)
	{
		Vector4& v = m_local2parent[c];
		for (int r = 0; r < 4; r ++)
		{
			v[r] = rand()%100;
		}
	}
#endif
}

CObject3D::~CObject3D()
{
}

void CObject3D::Serialize(CArchive& ar)
{
	bool storing = ar.IsStoring();
	for (int c = 0; c < 4; c ++)
	{
		Vector4& v = m_local2parent[c];
		for (int r = 0; r < 4; r ++)
		{
			Real& e = v[r];
			if (storing)
				ar << e;
			else
				ar >> e;
		}
	}
	CTreeNodePersist::Serialize(ar);
}

void CObject3D::DumpData()
{
	Vector4 v0 = m_local2parent[0];
	Vector4 v1 = m_local2parent[1];
	Vector4 v2 = m_local2parent[2];
	Vector4 v3 = m_local2parent[3];
	for (int r = 0; r < 4; r ++)
	{
		TRACE(_T("%2.0f %2.0f %2.0f %2.0f\n"), v0[r], v1[r], v2[r], v3[r]);
	}
}

void CObject3D::AddChild(CObject3D* child)
{
	child->m_nextSibbling = m_firstChild;
	m_firstChild = child;
	child->m_parent = this;
}

void CObject3D::RemoveSelf()
{
	std::queue<CObject3D*> tq;
	CObject3D* c = GetFirstChild();
	while(c)
	{
		tq.push(c);
		c = c->GetNextSibbling();
	}
	while(!tq.empty())
	{
		CObject3D* n = tq.front();
		tq.pop();
		c = n->GetFirstChild();
		while(c)
		{
			tq.push(c);
			c = c->GetNextSibbling();
		}
		delete n;
	}
	CObject3D* p = m_parent;
	if (p)
	{
		CObject3D* s = p->GetFirstChild();
		CObject3D* sp = NULL;
		while(s)
		{
			if (s == this)
			{
				if (NULL == sp)
				{
					p->m_firstChild = s->m_nextSibbling;
				}
				else
				{
					sp->m_nextSibbling = s->m_nextSibbling;
				}
				break;
			}
			sp = s;
			s = s->GetNextSibbling();
		}
	}
	delete this;
}

bool CObject3D::Connect(CObject3D* parent, CObject3D* child)
{
	ASSERT(!child->IsKindOf(RUNTIME_CLASS(CScene)));
	CObject3D* ancestor = parent;
	bool cycle = false;
	while(!cycle
		&& NULL != ancestor)
	{
		cycle = (ancestor == child);
		ancestor = ancestor->m_parent;
	}
	if (!cycle)
	{
		if (NULL != child->m_parent)
		{
			CObject3D** cnn = &child->m_parent->m_firstChild;
			while(NULL != *cnn)
			{
				if (*cnn == child)
				{
					*cnn = child->m_nextSibbling;
					child->m_nextSibbling = NULL;
					break;
				}
				cnn = &(*cnn)->m_nextSibbling;
			}
		}
		parent->AddChild(child);
	}
	return !cycle;
}

// CObject3D member functions
