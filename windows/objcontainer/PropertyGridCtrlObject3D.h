#pragma once
#include "Object3D.h"
#include <map>
// CPropertyGridCtrlObject3D

class CPropertyGridCtrlObject3D : public CMFCPropertyGridCtrl
{
	DECLARE_DYNAMIC(CPropertyGridCtrlObject3D)

public:
	CPropertyGridCtrlObject3D();
	virtual ~CPropertyGridCtrlObject3D();
	void Init(CObject3D* pObj);
	bool Update(CObject3D* pObj, bool bObj2property = true);
protected:

	DECLARE_MESSAGE_MAP()

	std::map<CRuntimeClass*, CRuntimeClass*> c_mapObj2Prop;
};


