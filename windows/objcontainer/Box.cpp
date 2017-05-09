#include "StdAfx.h"
#include "Box.h"

IMPLEMENT_SERIAL(CBox, CObject3D, 1)

CBox::CBox(void)
{
	 m_strName = _T("Box");
}


CBox::~CBox(void)
{
}
