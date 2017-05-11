#pragma once
#include <afxpropertygridctrl.h>
#include "Object3D.h"
#include "PropertyItem.h"

const int c_idxNULL = -1;
enum PT {Group, ItemEdit, ItemEditSpin, ItemFile};
struct _Initializer
{
	PT pt;
	int idxFirstChild;
	int idxNextSibbling;
	CobjcontainerDoc::OP code;
	CMFCPropertyGridProperty* propPane;
};



struct _IInitializer : public _Initializer
{
	LPCTSTR name;
	LPCTSTR descr;
	Pipe pipe;
	_IInitializer(int a_firstChild, int a_nextSibbling, LPCTSTR a_name, LPCTSTR a_descr, Set set, Get get, PT a_pt, CobjcontainerDoc::OP a_code = CobjcontainerDoc::OP_PROPCH)
	{
		pt = a_pt;
		idxFirstChild = a_firstChild;
		idxNextSibbling = a_nextSibbling;
		name = a_name;
		descr = a_descr;
		pipe.set = set;
		pipe.get = get;
		propPane = NULL;
		code = a_code;
	}
};



struct _GInitializer : public _Initializer
{
	LPCTSTR name;
	BOOL bIsValueList;
	_GInitializer(int a_firstChild, int a_nextSibbling, LPCTSTR a_name, BOOL a_bIsValueList, CobjcontainerDoc::OP a_code = CobjcontainerDoc::OP_PROPCH)
	{
		pt = Group;
		idxFirstChild = a_firstChild;
		idxNextSibbling = a_nextSibbling;
		name = a_name;
		bIsValueList = a_bIsValueList;
		propPane = NULL;
		code = a_code;
	}
};

class CPropertyGridGroup;

class CPropertyGridGroup :
	public CMFCPropertyGridProperty
	, public IUIPropertyImpl<CPropertyGridGroup>
{
public:
	DECLARE_DYNCREATE(CPropertyGridGroup)
	CPropertyGridGroup(void);
	CPropertyGridGroup(const CString& strGroupName, BOOL bIsValueList = FALSE, CobjcontainerDoc::OP code = CobjcontainerDoc::OP_PROPCH);
	virtual ~CPropertyGridGroup(void);
	virtual void Init(const CObject3D* pObj);
	virtual bool Update(CObject3D* pObj, bool bObj2Prop);
protected:
	void Initiate(const CObject3D* pObj, _Initializer** params);
};

