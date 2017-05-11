#pragma once
#include "PropertyGridGroup.h"
class CPropertyGridWF :
	public CPropertyGridGroup
{
public:
	DECLARE_DYNCREATE(CPropertyGridWF)
	CPropertyGridWF(void);
	~CPropertyGridWF(void);
	virtual void Init(const CObject3D* pObj);
};

