
#include "stdafx.h"
#include "MainFrm.h"
#include "Resource.h"
#include "ViewObjHierarchy.h"
#include "objcontainer.h"
#include "objcontainerDoc.h"
#include <queue>
#include "Box.h"

class CClassViewMenuButton : public CMFCToolBarMenuButton
{
	friend class CViewObjHierarchy;

	DECLARE_SERIAL(CClassViewMenuButton)

public:
	CClassViewMenuButton(HMENU hMenu = NULL) : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
	{
	}

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE,
		BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages();

		CAfxDrawState ds;
		pImages->PrepareDrawImage(ds);

		CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

		pImages->EndDrawImage(ds);
	}
};

IMPLEMENT_SERIAL(CClassViewMenuButton, CMFCToolBarMenuButton, 1)


CViewObjHierarchy::FuncObjTreeItem CViewObjHierarchy::s_funcObjTreeItem[] = {
	{"CScene", ID_ITEM_SCENE}
	, {"CObjectWaveFront", ID_ITEM_WAVEFRONTOBJ}
	, {"CBox", ID_ITEM_BOX}
};

const LPCSTR CViewObjHierarchy::Object(TREEITEM item)
{
	LPCSTR name = NULL;
	for (int i = 0
		; NULL == name && i < sizeof(s_funcObjTreeItem)/sizeof(FuncObjTreeItem)
		; i ++)
	{
		if (item == s_funcObjTreeItem[i].y)
			name = s_funcObjTreeItem[i].x;
	}
	return name;
}

const CViewObjHierarchy::TREEITEM CViewObjHierarchy::TreeItem(LPCSTR name)
{
	const TREEITEM c_invalid = ID_ITEM_TOTAL;
	TREEITEM item = c_invalid;
	for (int i = 0
		; c_invalid == item && i < sizeof(s_funcObjTreeItem)/sizeof(FuncObjTreeItem)
		; i ++)
	{
		if (name == s_funcObjTreeItem[i].x)
			item = s_funcObjTreeItem[i].y;
	}
	return item;
}

LPCSTR CViewObjHierarchy::s_funcCmdNewObj[] = {"CBox"};
const LPCSTR CViewObjHierarchy::ClsName(UINT id)
{
	const UINT c_idBase = ID_NEW_FOLDER;
	return s_funcCmdNewObj[id - c_idBase];
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CViewObjHierarchy::CViewObjHierarchy()
{
	m_Selected.hItem = NULL;
	m_Selected.pItem = NULL;
	m_nCurrSort = ID_SORTING_GROUPBYTYPE;
}

CViewObjHierarchy::~CViewObjHierarchy()
{
}

BEGIN_MESSAGE_MAP(CViewObjHierarchy, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CLASS_ADD_MEMBER_FUNCTION, OnClassAddMemberFunction)
	ON_COMMAND(ID_CLASS_ADD_MEMBER_VARIABLE, OnClassAddMemberVariable)
	ON_COMMAND(ID_CLASS_DEFINITION, OnClassDefinition)
	ON_COMMAND(ID_CLASS_PROPERTIES, OnClassProperties)
	ON_COMMAND_RANGE(ID_NEW_FOLDER, ID_NEW_FOLDER + 1, OnNewBox)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnSort)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnUpdateSort)
	ON_MESSAGE(WM_INITIALUPDATE, OnInitialUpdate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewObjHierarchy message handlers

LRESULT CViewObjHierarchy::OnInitialUpdate(WPARAM wParam, LPARAM lParam)
{
	// Fill in some static tree view data (dummy code, nothing magic here)
	FillObjHierarchy();
	CobjcontainerDoc* pDoc = GetDocument();
	pDoc->RegisterView(this);
	return 0;
}

void CViewObjHierarchy::OnUpdate(CWnd* pSender, CobjcontainerDoc::OP op, CObject3D* pObj)
{
	if (this == pSender)
		return;
	else
	{
		//todo: a tree traverse algorithm for the op
	}
}


BOOL CViewObjHierarchy::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (wParam == ID_TREEVIEW)
	{
		NMHDR* pNMHDR = (NMHDR*)lParam;
		ASSERT(pNMHDR != NULL);
		if (pNMHDR && pNMHDR->code == TVN_SELCHANGED)
		{
			HTREEITEM hItem = m_wndObjsTreeView.GetSelectedItem();
			TVITEM item;
			item.mask = TVIF_PARAM;
			item.hItem = hItem;
			m_wndObjsTreeView.GetItem(&item);
			CObject3D* pObj = (CObject3D *)(item.lParam);
			CobjcontainerDoc* pDoc = GetDocument();
			pDoc->UpdateAllViews(this, CobjcontainerDoc::OP_SEL, pObj);
			m_Selected.hItem = hItem;
			m_Selected.pItem = pObj;
		}
	}
	return CDockablePane::OnNotify(wParam, lParam, pResult);
}

int CViewObjHierarchy::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create views:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndObjsTreeView.Create(dwViewStyle, rectDummy, this, ID_TREEVIEW))
	{
		TRACE0("Failed to create Class View\n");
		return -1;      // fail to create
	}

	// Load images:
	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_SORT);
	m_wndToolBar.LoadToolBar(IDR_SORT, 0, 0, TRUE /* Is locked */);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	CMenu menuSort;
	menuSort.LoadMenu(IDR_POPUP_SORT);

	m_wndToolBar.ReplaceButton(ID_SORT_MENU, CClassViewMenuButton(menuSort.GetSubMenu(0)->GetSafeHmenu()));

	CClassViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CClassViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != NULL)
	{
		pButton->m_bText = FALSE;
		pButton->m_bImage = TRUE;
		pButton->SetImage(GetCmdMgr()->GetCmdImage(m_nCurrSort));
		pButton->SetMessageWnd(this);
	}



	return 0;
}

void CViewObjHierarchy::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}



void CViewObjHierarchy::FillObjHierarchy()
{
	m_wndObjsTreeView.DeleteAllItems();

	CobjcontainerDoc* pDoc = GetDocument();

	CObject3D* root = pDoc->RootObj();
	UINT uMask = TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_TEXT;
	CString str;
	root->GetName(str);
	CRuntimeClass* prt = root->GetRuntimeClass();
	int nImg = TreeItem(prt->m_lpszClassName);
	int nSelectedImg = TreeItem(prt->m_lpszClassName);
	UINT nState = TVIS_BOLD;
	UINT nStateMask = TVIS_BOLD;
	LPARAM lParam = (LPARAM)root;
	HTREEITEM hParent = NULL;
	HTREEITEM hInsertAfter = NULL;

	HTREEITEM hRoot = m_wndObjsTreeView.InsertItem(
										uMask
										, str
										, nImg
										, nSelectedImg
										, nState
										, nStateMask
										, lParam
										, hParent
										, hInsertAfter);
	typedef struct stTreeNodeInitializer
	{
		CObject3D* from;
		HTREEITEM to;
	} TreeNodeInitializer;
	std::queue<TreeNodeInitializer> queBFS;
	TreeNodeInitializer unit = {root, hRoot};
	queBFS.push(unit);
	while(!queBFS.empty())
	{
		TreeNodeInitializer unitThis = queBFS.front();
		queBFS.pop();
		TreeNodeInitializer unitNext = {unitThis.from->GetFirstChild(), NULL};
		hParent = unitThis.to;
		hInsertAfter = NULL;
		while (NULL != unitNext.from)
		{
			prt = unitNext.from->GetRuntimeClass();
			nImg = TreeItem(prt->m_lpszClassName);
			nSelectedImg = TreeItem(prt->m_lpszClassName);

			unitNext.from->GetName(str);
			lParam = (LPARAM)unitNext.from;
			unitNext.to = m_wndObjsTreeView.InsertItem(
										uMask
										, str
										, nImg
										, nSelectedImg
										, nState
										, nStateMask
										, lParam
										, hParent
										, hInsertAfter);
			queBFS.push(unitNext);

			hInsertAfter = unitNext.to;
			unitNext.from = unitNext.from->GetNextSibbling();;
		}
	}


	//std::stack<CObject3D*> stkObjs;
	//std::stack<HTREEITEM


	/*HTREEITEM hRoot = m_wndObjsTreeView.InsertItem(_T("VisObjects"), 0, 0);
	m_wndObjsTreeView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	HTREEITEM hClass = m_wndObjsTreeView.InsertItem(_T("3d Objects"), 1, 1, hRoot);
	m_wndObjsTreeView.InsertItem(_T("Car"), 3, 3, hClass);

	m_wndObjsTreeView.Expand(hRoot, TVE_EXPAND);

	hClass = m_wndObjsTreeView.InsertItem(_T("2d Objects"), 1, 1, hRoot);*/
}

void CViewObjHierarchy::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndObjsTreeView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// Select clicked item:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	CMenu menu;
	menu.LoadMenu(IDR_POPUP_SORT);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}
}

void CViewObjHierarchy::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndObjsTreeView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CViewObjHierarchy::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

void CViewObjHierarchy::OnSort(UINT id)
{
	if (m_nCurrSort == id)
	{
		return;
	}

	m_nCurrSort = id;

	CClassViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CClassViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != NULL)
	{
		pButton->SetImage(GetCmdMgr()->GetCmdImage(id));
		m_wndToolBar.Invalidate();
		m_wndToolBar.UpdateWindow();
	}
}

void CViewObjHierarchy::OnUpdateSort(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID == m_nCurrSort);
}

void CViewObjHierarchy::OnClassAddMemberFunction()
{
	AfxMessageBox(_T("Add member function..."));
}

void CViewObjHierarchy::OnClassAddMemberVariable()
{
	// TODO: Add your command handler code here
}

void CViewObjHierarchy::OnClassDefinition()
{
	// TODO: Add your command handler code here
}

void CViewObjHierarchy::OnClassProperties()
{
	// TODO: Add your command handler code here
}

void CViewObjHierarchy::OnNewBox(UINT nID)
{
	//AfxMessageBox(_T("New Folder..."));
	//todo: Add new Object here
	Item parent = {m_Selected.hItem, m_Selected.pItem};
	CobjcontainerDoc* pDoc = GetDocument();
	if (NULL == parent.hItem
	|| NULL == parent.pItem)
	{
		parent.hItem = m_wndObjsTreeView.GetRootItem();
		parent.pItem = pDoc->RootObj();
	}

	Item child;
	CRuntimeClass* prt = CRuntimeClass::FromName(ClsName(nID));
	child.pItem = static_cast<CObject3D*>(prt->CreateObject());

	parent.pItem->AddChild(child.pItem);

	UINT uMask = TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_TEXT;
	CString str;
	child.pItem->GetName(str);
	int nImg = TreeItem(prt->m_lpszClassName);
	int nSelectedImg = TreeItem(prt->m_lpszClassName);
	UINT nState = TVIS_BOLD;
	UINT nStateMask = TVIS_BOLD;
	LPARAM lParam = (LPARAM)child.pItem;
	HTREEITEM hParent = parent.hItem;
	HTREEITEM hInsertAfter = TVI_FIRST; //m_wndObjsTreeView.GetChildItem(parent.hItem);

	child.hItem = m_wndObjsTreeView.InsertItem(
										uMask
										, str
										, nImg
										, nSelectedImg
										, nState
										, nStateMask
										, lParam
										, hParent
										, hInsertAfter);

	pDoc->UpdateAllViews(this, CobjcontainerDoc::OP_NEW, child.pItem);
	m_wndObjsTreeView.Expand(parent.hItem, TVE_EXPAND);
	//m_wndObjsTreeView.Invalidate();
}

void CViewObjHierarchy::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndObjsTreeView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CViewObjHierarchy::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndObjsTreeView.SetFocus();
}

void CViewObjHierarchy::OnChangeVisualStyle()
{
	m_ClassViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_CLASS_VIEW_24 : IDB_CLASS_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("Can't load bitmap: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_ClassViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ClassViewImages.Add(&bmp, RGB(255, 0, 0));

	m_wndObjsTreeView.SetImageList(&m_ClassViewImages, TVSIL_NORMAL);

	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_SORT_24 : IDR_SORT, 0, 0, TRUE /* Locked */);
}


