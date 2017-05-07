#pragma once
#include "object3d.h"
class CObjectWaveFront :
	public CObject3D
{
public:
	DECLARE_SERIAL( CObjectWaveFront )
	CObjectWaveFront(void);
	virtual ~CObjectWaveFront(void);
	virtual void GetName(CString& strName) const
	{
		strName = _T("WaveFront Object");
	}
};

