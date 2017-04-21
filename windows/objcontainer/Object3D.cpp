// Object3D.cpp : implementation file
//

#include "stdafx.h"
#include "objcontainer.h"
#include "Object3D.h"


// CObject3D
IMPLEMENT_SERIAL(CObject3D, CObject, 1)
CObject3D::CObject3D()
	: m_local2parent(1.0f)
	, m_parent2local(1.0f)
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

#ifdef TEST_SIERALIZATION
	if (storing)
		TRACE(_T("Storing...\n"));
	else
		TRACE(_T("Loading...\n"));
	DumpData();
#endif
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

// CObject3D member functions
