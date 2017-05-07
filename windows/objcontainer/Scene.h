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


	virtual void RemoveSelf()
	{
	}
};

