#include "StdAfx.h"
#include "PropertyGridObject3D.h"
#include "PropertyItem.h"
#include "Object3D.h"
#include <stack>
IMPLEMENT_DYNCREATE(CPropertyGridObject3D, CPropertyGridGroup)
CPropertyGridObject3D::CPropertyGridObject3D(void)
{
}


CPropertyGridObject3D::~CPropertyGridObject3D(void)
{
}

void CPropertyGridObject3D::Init(const CObject3D* pObj)
{
	ATLASSERT(this->m_lstSubItems.IsEmpty());
	_variant_t vName;
	CObject3D::GetName(pObj, vName);
	Pipe pName = {CObject3D::SetName, CObject3D::GetName};

	_Initializer* params[] = {
		  new _IInitializer(-1, 1, _T("Name"), _T("Name of the three-D object"), CObject3D::SetName, CObject3D::GetName, ItemEdit) //0: Name
		, new _GInitializer(2, -1, _T("Transform"), FALSE) //1: Transform
			, new _GInitializer(3,  6, _T("Position"), TRUE) //2: Transform->Position
				, new _IInitializer(-1, 4, _T("X:"), _T("Position of the tree-D object"), CObject3D::SetPosX, CObject3D::GetPosX, ItemEditSpin) //3: Transform->Position->X
				, new _IInitializer(-1, 5, _T("Y:"), _T("Position of the tree-D object"), CObject3D::SetPosY, CObject3D::GetPosY, ItemEditSpin) //4: Transform->Position->Y
				, new _IInitializer(-1, -1, _T("Z:"), _T("Position of the tree-D object"), CObject3D::SetPosZ, CObject3D::GetPosZ, ItemEditSpin) //5: Transform->Position->Z
			, new _GInitializer(7, 10, _T("Rotation"), TRUE) //6: Transform->Rotation
				, new _IInitializer(-1, 8, _T("X:"), _T("Rotation around X of the tree-D object"), CObject3D::SetRotX, CObject3D::GetRotX, ItemEditSpin) //7: Transform->Rotation->X
				, new _IInitializer(-1, 9, _T("Y:"), _T("Rotation around Y of the tree-D object"), CObject3D::SetRotY, CObject3D::GetRotY, ItemEditSpin) //8: Transform->Rotation->Y
				, new _IInitializer(-1, -1, _T("Z:"), _T("Rotation around Z of the tree-D object"), CObject3D::SetRotZ, CObject3D::GetRotZ, ItemEditSpin) //9: Transform->Rotation->Z
			, new _GInitializer(11, -1, _T("Scale"), TRUE) //10: Transform->Scale
				, new _IInitializer(-1, 12, _T("X:"), _T("Scale on X of the tree-D object"), CObject3D::SetScaleX, CObject3D::GetScaleX, ItemEditSpin) //11: Transform->Scale->X
				, new _IInitializer(-1, 13, _T("Y:"), _T("Scale on Y of the tree-D object"), CObject3D::SetScaleY, CObject3D::GetScaleY, ItemEditSpin) //12: Transform->Scale->Y
				, new _IInitializer(-1, -1, _T("Z:"), _T("Scale on Z of the tree-D object"), CObject3D::SetScaleZ, CObject3D::GetScaleZ, ItemEditSpin) //13: Transform->Scale->Z
	};

	Initiate(pObj, params);

	const int len = sizeof(params)/sizeof(_Initializer*);
	for (int i = 0; i < len; i ++)
		delete params[i];
}
