#pragma once
#include <afxpropertygridctrl.h>
#include "Object3D.h"
class CPropertyGridGroup :
	public CMFCPropertyGridProperty
{
public:
	DECLARE_DYNCREATE(CPropertyGridGroup)
	CPropertyGridGroup(void);
	virtual ~CPropertyGridGroup(void);
	virtual void Cnn(const CObject3D* pObj);
};

