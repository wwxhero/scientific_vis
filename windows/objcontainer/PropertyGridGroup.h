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
	virtual void Init(const CObject3D* pObj);
	bool Update(CObject3D* pObj, bool bObj2Prop);
};

