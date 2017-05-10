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
	, m_strName(_T("General 3D Object"))
	, m_pos(0)
	, m_rot(0)
	, m_scale(1)
	, m_programID(0)
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
	BOOL storing = ar.IsStoring();
	Vector3* v3s[] = {&m_pos, &m_rot, &m_scale};
	if (storing)
	{
		ar << m_strName;
		for (int i = 0; i < sizeof(v3s)/sizeof(Vector3*); i ++)
		{
			for (int j = 0; j < 3; j ++)
				ar << (*v3s[i])[j];
		}
	}
	else
	{
		ar >> m_strName;
		for (int i = 0; i < sizeof(v3s)/sizeof(Vector3*); i ++)
		{
			for (int j = 0; j < 3; j ++)
				ar >> (*v3s[i])[j];
		}
	}
	CTreeNodePersist::Serialize(ar);
}


void CObject3D::glDraw(const Matrix4x4& w2v, const Matrix4x4& v2c)
{

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
		CObject3D* sm = NULL;
		while(s)
		{
			if (s == this)
			{
				if (NULL == sm)
				{
					p->m_firstChild = s->m_nextSibbling;
				}
				else
				{
					sm->m_nextSibbling = s->m_nextSibbling;
				}
				break;
			}
			sm = s;
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


bool CObject3D::SetName(CObject3D* pThis, const _variant_t& vName)
{
	pThis->m_strName = vName;
	return true;
}
bool CObject3D::GetName(const CObject3D* pThis, _variant_t& vName)
{
	vName = pThis->m_strName;
	return true;
}

bool CObject3D::SetPosX(CObject3D* pThis, const _variant_t& posX)
{
	pThis->m_pos.x = posX;
	return true;
}
bool CObject3D::GetPosX(const CObject3D* pThis, _variant_t& posX)
{
	posX = (int)pThis->m_pos.x;
	return true;
}
bool CObject3D::SetPosY(CObject3D* pThis, const _variant_t& posY)
{
	pThis->m_pos.y = posY;
	return true;
}
bool CObject3D::GetPosY(const CObject3D* pThis, _variant_t& posY)
{
	posY = (int)pThis->m_pos.y;
	return true;
}
bool CObject3D::SetPosZ(CObject3D* pThis, const _variant_t& posZ)
{
	pThis->m_pos.z = posZ;
	return true;
}
bool CObject3D::GetPosZ(const CObject3D* pThis, _variant_t& posZ)
{
	posZ = (int)pThis->m_pos.z;
	return true;
}
bool CObject3D::SetRotX(CObject3D* pThis, const _variant_t& rotX)
{
	pThis->m_rot.x = rotX;
	return true;
}
bool CObject3D::GetRotX(const CObject3D* pThis, _variant_t& rotX)
{
	rotX = (int)pThis->m_rot.x;
	return true;
}
bool CObject3D::SetRotY(CObject3D* pThis, const _variant_t& rotY)
{
	pThis->m_rot.y = rotY;
	return true;
}
bool CObject3D::GetRotY(const CObject3D* pThis, _variant_t& rotY)
{
	rotY = (int)pThis->m_rot.y;
	return true;
}
bool CObject3D::SetRotZ(CObject3D* pThis, const _variant_t& rotZ)
{
	pThis->m_rot.z = rotZ;
	return true;
}
bool CObject3D::GetRotZ(const CObject3D* pThis, _variant_t& rotZ)
{
	rotZ = (int)pThis->m_rot.z;
	return true;
}
bool CObject3D::SetScaleX(CObject3D* pThis, const _variant_t& scaleX)
{
	pThis->m_scale.x = scaleX;
	return true;
}
bool CObject3D::GetScaleX(const CObject3D* pThis, _variant_t& scaleX)
{
	scaleX = (int)pThis->m_scale.x;
	return true;
}
bool CObject3D::SetScaleY(CObject3D* pThis, const _variant_t& scaleY)
{
	pThis->m_scale.y = scaleY;
	return true;
}
bool CObject3D::GetScaleY(const CObject3D* pThis, _variant_t& scaleY)
{
	scaleY = (int)pThis->m_scale.y;
	return true;
}
bool CObject3D::SetScaleZ(CObject3D* pThis, const _variant_t& scaleZ)
{
	pThis->m_scale.z = scaleZ;
	return true;
}
bool CObject3D::GetScaleZ(const CObject3D* pThis, _variant_t& scaleZ)
{
	scaleZ = (int)pThis->m_scale.z;
	return true;
}

// CObject3D member functions
