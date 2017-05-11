#pragma once
#include "afxpropertygridctrl.h"
#include "Object3D.h"
#include "objcontainerDoc.h"
typedef bool (*Set)(CObject3D* pObj, const _variant_t& var);
typedef bool (*Get)(const CObject3D* pObj, _variant_t& var);
typedef struct _Pipe
{
	Set set;
	Get get;
} Pipe;

struct IUIProperty
{
	virtual bool Update(CObject3D* pObj, bool bObj2Prop) = 0;
	virtual CobjcontainerDoc::OP GetCode() = 0;
};

template<typename TThis>
class IUIPropertyImpl : public IUIProperty
{
private:
	virtual bool Update(CObject3D* pObj, bool bObj2Prop)
	{
		TThis* pThis = static_cast<TThis*>(this);
		_variant_t v;
		bool done = true;
		if (bObj2Prop)
		{
			done = m_pipe.get(pObj, v);
			if (done)
				pThis->SetValue(v);
		}
		else
		{
			v = pThis->GetValue();
			done = m_pipe.set(pObj, v);
		}
		return done;
	}
	virtual CobjcontainerDoc::OP GetCode()
	{
		return m_code;
	}
protected:
	Pipe m_pipe;
	CobjcontainerDoc::OP m_code;
};

class CPropertyItem :
	public CMFCPropertyGridProperty
	, public IUIPropertyImpl<CPropertyItem>
{
	DECLARE_DYNAMIC(CPropertyItem)
public:
	CPropertyItem(const CString& strName, const _variant_t& v,  Pipe pipe, LPCTSTR lpszDescr = NULL, CobjcontainerDoc::OP code = CobjcontainerDoc::OP_PROPCH);
	virtual ~CPropertyItem(void);
};

class CPropertyItemFile :
	public CMFCPropertyGridFileProperty
	, public IUIPropertyImpl<CPropertyItemFile>
{
	DECLARE_DYNAMIC(CPropertyItemFile)
public:
	CPropertyItemFile(const CString& strName
					, const _variant_t& v
					,  Pipe pipe
					, LPCTSTR lpszDescr = NULL
					, CobjcontainerDoc::OP code = CobjcontainerDoc::OP_PROPCH
					, BOOL bOpenFileDialog = TRUE
					, const CString& strFileName = _T("")
					, LPCTSTR lpszDefExt = NULL
					, DWORD dwFlags=OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT
					, LPCTSTR lpszFilter=NULL
					, DWORD_PTR dwData=0);
	virtual ~CPropertyItemFile(void);
};

