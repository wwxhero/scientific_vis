#pragma once
#include "afxpropertygridctrl.h"
#include "Object3D.h"
typedef bool (*Set)(CObject3D* pObj, const _variant_t& var);
typedef bool (*Get)(const CObject3D* pObj, _variant_t& var);
typedef struct _Pipe
{
	Set set;
	Get get;
} Pipe;
class CPropertyItem :
	public CMFCPropertyGridProperty
{
public:
	CPropertyItem(const CString& strName, const _variant_t& v,  Pipe pipe, LPCTSTR lpszDescr = NULL);
	virtual ~CPropertyItem(void);
	bool Update(CObject3D* pObj, bool bObj2Prop);
private:
	Pipe m_pipe;
};

