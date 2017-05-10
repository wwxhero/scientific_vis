#pragma once
#include <afxpropertygridctrl.h>
#include "Object3D.h"
#include "PropertyItem.h"

const int c_idxNULL = -1;
enum PT {Group, ItemEdit, ItemEditSpin};
struct _Initializer
{
	PT pt;
	int idxFirstChild;
	int idxNextSibbling;
	CMFCPropertyGridProperty* propPane;
};



struct _IInitializer : public _Initializer
{
	LPCTSTR name;
	LPCTSTR descr;
	Pipe pipe;
	_IInitializer(int a_firstChild, int a_nextSibbling, LPCTSTR a_name, LPCTSTR a_descr, Set set, Get get, PT a_pt)
	{
		pt = a_pt;
		idxFirstChild = a_firstChild;
		idxNextSibbling = a_nextSibbling;
		name = a_name;
		descr = a_descr;
		pipe.set = set;
		pipe.get = get;
		propPane = NULL;
	}
};



struct _GInitializer : public _Initializer
{
	LPCTSTR name;
	BOOL bIsValueList;
	_GInitializer(int a_firstChild, int a_nextSibbling, LPCTSTR a_name, BOOL a_bIsValueList)
	{
		pt = Group;
		idxFirstChild = a_firstChild;
		idxNextSibbling = a_nextSibbling;
		name = a_name;
		bIsValueList = a_bIsValueList;
		propPane = NULL;
	}
};

class CPropertyGridGroup;

class CPropertyGridGroup :
	public CMFCPropertyGridProperty
{
public:
	DECLARE_DYNCREATE(CPropertyGridGroup)
	CPropertyGridGroup(void);
	CPropertyGridGroup(const CString& strGroupName, BOOL bIsValueList = FALSE);
	virtual ~CPropertyGridGroup(void);
	virtual void Init(const CObject3D* pObj);
	bool Update(CObject3D* pObj, bool bObj2Prop);
protected:
	void Initiate(const CObject3D* pObj, _Initializer** params);
};

