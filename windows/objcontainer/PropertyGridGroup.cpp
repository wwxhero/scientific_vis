#include "StdAfx.h"
#include <stack>
#include <queue>
#include "PropertyGridGroup.h"
#include "PropertyItem.h"
#define SPIN_MIN -1000
#define SPIN_MAX +1000

IMPLEMENT_DYNCREATE(CPropertyGridGroup, CMFCPropertyGridProperty)
CPropertyGridGroup::CPropertyGridGroup(void)
	: CMFCPropertyGridProperty(_T(""))
{
}

CPropertyGridGroup::CPropertyGridGroup(const CString& strGroupName, BOOL bIsValueList)
	: CMFCPropertyGridProperty(strGroupName, 0, bIsValueList)
{

}

CPropertyGridGroup::~CPropertyGridGroup(void)
{
}

void CPropertyGridGroup::Init(const CObject3D* pObj)
{
	//TODO: over write this function in derived classes to set up connection from property item to data
}

bool CPropertyGridGroup::Update(CObject3D* pObj, bool bObj2Prop)
{
	POSITION pos = m_lstSubItems.GetHeadPosition();
	bool updated = true;
	while(pos && updated)
	{
		CMFCPropertyGridProperty* propPage = m_lstSubItems.GetNext(pos);
		if (propPage->IsKindOf(RUNTIME_CLASS(CPropertyItem)))
		{
			CPropertyItem *item = static_cast<CPropertyItem*>(propPage);
			updated = item->Update(pObj, bObj2Prop);
		}
	}
	return updated;
}

void CPropertyGridGroup::Initiate(const CObject3D* pObj, _Initializer** params)
{
	std::queue<_Initializer*> tq;
	_Initializer* c = params[0];

	do
	{
		if (Group == c->pt)
		{
			_GInitializer* g = (_GInitializer*)c;
			c->propPane = new CPropertyGridGroup(g->name, g->bIsValueList);
		}
		else if(ItemEdit == c->pt)
		{
			_IInitializer* i = (_IInitializer*)c;
			_variant_t v;
			i->pipe.get(pObj, v);
			c->propPane = new CPropertyItem(i->name, v, i->pipe, i->descr);
		}
		else if(ItemEditSpin == c->pt)
		{
			_IInitializer* i = (_IInitializer*)c;
			_variant_t v;
			i->pipe.get(pObj, v);
			c->propPane = new CPropertyItem(i->name, v, i->pipe, i->descr);
			c->propPane->EnableSpinControl(TRUE, SPIN_MIN, SPIN_MAX);
		}


		AddSubItem(c->propPane);

		tq.push(c);
		c = (c_idxNULL == c->idxNextSibbling) ? NULL : params[c->idxNextSibbling];
	}while(NULL != c);

	while(!tq.empty())
	{
		_Initializer* n = tq.front();

		_Initializer* c = (c_idxNULL ==  n->idxFirstChild) ? NULL : params[n->idxFirstChild];
		while (NULL != c)
		{
			ASSERT(n->pt == Group);
			if (Group == c->pt)
			{
				_GInitializer* g = (_GInitializer*)c;
				c->propPane = new CPropertyGridGroup(g->name, g->bIsValueList);
			}
			else if(ItemEdit == c->pt)
			{
				_IInitializer* i = (_IInitializer*)c;
				_variant_t v;
				i->pipe.get(pObj, v);
				c->propPane = new CPropertyItem(i->name, v, i->pipe, i->descr);
			}
			else if(ItemEditSpin == c->pt)
			{
				_IInitializer* i = (_IInitializer*)c;
				_variant_t v;
				i->pipe.get(pObj, v);
				c->propPane = new CPropertyItem(i->name, v, i->pipe, i->descr);
				c->propPane->EnableSpinControl(TRUE, SPIN_MIN, SPIN_MAX);
			}
			n->propPane->AddSubItem(c->propPane);

			tq.push(c);
			c = (c_idxNULL == c->idxNextSibbling) ? NULL : params[c->idxNextSibbling];
		}
		tq.pop();
	}

	/*for (int i = 0; i < sizeof(parms)/sizeof(_Initializer); i ++)
	{

		_variant_t v;
		parms[i].pipe.get(pObj, v);
		AddSubItem(new CPropertyItem(parms[i].name, v, parms[i].pipe, parms[i].descr));
	}*/


	//CPropertyItem(const CString& strName, const _variant_t& v,  Pipe pipe, LPCTSTR lpszDescr = NULL);
	//pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("3D Look"), (_variant_t) false, _T("Specifies the window's font will be non-bold and controls will have a 3D border")));
}
