#include "StdAfx.h"
#include "ObjectWaveFront.h"

IMPLEMENT_SERIAL(CObjectWaveFront, CObject3D, 1)

CObjectWaveFront::CObjectWaveFront(void)
{
	m_strName = _T("Wavefront");
}


CObjectWaveFront::~CObjectWaveFront(void)
{
}
