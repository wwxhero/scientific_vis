#include "StdAfx.h"
#include "PropertyGridWF.h"
#include "ObjectWaveFront.h"

IMPLEMENT_DYNCREATE(CPropertyGridWF, CPropertyGridGroup)
CPropertyGridWF::CPropertyGridWF(void)
{
}


CPropertyGridWF::~CPropertyGridWF(void)
{
}

void CPropertyGridWF::Init(const CObject3D* pObj)
{
	ATLASSERT(this->m_lstSubItems.IsEmpty());

	_Initializer* params[] = {
		  new _IInitializer(-1, -1, _T("File"), _T("File path of the three-D object"), CObjectWaveFront::SetFilePath, CObjectWaveFront::GetFilePath, ItemFile, CobjcontainerDoc::OP_UPDATEVBF) //0: Name
	};

	Initiate(pObj, params);

	const int len = sizeof(params)/sizeof(_Initializer*);
	for (int i = 0; i < len; i ++)
		delete params[i];
}