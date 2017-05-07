
#pragma once

#include "ViewTree.h"
#include "Resource.h"
#include "objcontainerDoc.h"
#include "ViewPane.h"

class CClassToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CViewObjHierarchy : public CViewPane
{
public:
	CViewObjHierarchy();
	virtual ~CViewObjHierarchy();

	void AdjustLayout();
	void OnChangeVisualStyle();

	enum {ID_TREEVIEW = 1, ID_TOOLBAR = IDR_SORT};
	enum TREEITEM {ID_ITEM_SCENE = 0, ID_ITEM_WAVEFRONTOBJ, ID_ITEM_UNKNOWN, ID_ITEM_BOX, ID_ITEM_TOTAL};
	void FillObjHierarchy();

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnUpdate(CWnd* pSender, CobjcontainerDoc::OP op, CObject3D* pObj);
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnClassAddMemberFunction();
	afx_msg void OnClassAddMemberVariable();
	afx_msg void OnClassDefinition();
	afx_msg void OnClassProperties();
	afx_msg void OnNewObj(UINT nID);
	afx_msg void OnDeleteObj();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnChangeActiveTab(WPARAM, LPARAM);
	afx_msg void OnSort(UINT id);
	afx_msg void OnUpdateSort(CCmdUI* pCmdUI);
	afx_msg LRESULT OnInitialUpdate(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()

private:
	typedef struct _Item
	{
		HTREEITEM hItem;
		CObject3D* pItem;
	} Item;

	void GetSelectedItem(Item& item);
	void Reload(Item& item);

protected:
	CClassToolBar m_wndToolBar;
	CViewTree m_wndObjsTreeView;
	CImageList m_ClassViewImages;
	UINT m_nCurrSort;



	typedef struct _FuncObjTreeItem
	{
		LPCSTR x;
		TREEITEM y;
	} FuncObjTreeItem; //invertable func

	static FuncObjTreeItem s_funcObjTreeItem[];
	static const LPCSTR Object(TREEITEM item);
	static const TREEITEM TreeItem(LPCSTR name);

	static LPCSTR s_funcCmdNewObj[];
	static const LPCSTR ClsName(UINT id);
};

