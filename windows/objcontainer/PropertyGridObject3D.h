#pragma once
#include "PropertyGridGroup.h"
class CPropertyGridObject3D :
	public CPropertyGridGroup
{
public:
	DECLARE_DYNCREATE(CPropertyGridObject3D)
	CPropertyGridObject3D(void);
	virtual ~CPropertyGridObject3D(void);
};

