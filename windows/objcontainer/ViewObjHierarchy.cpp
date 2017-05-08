
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
		if (0 == strcmp(name,s_funcObjTreeItem[i].x))
			item = s_funcObjTreeItem[i].y;
	}
	return item;
}

LPCSTR CViewObjHierarchy::s_funcCmdNewObj[] = {"CBox", "CObjectWaveFront"};
const LPCSTR CViewObjHierarchy::ClsName(UINT id)
{
	const UINT c_idBase = ID_NEW_BOX;
	return s_funcCmdNewObj[id - c_idBase];
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CViewObjHierarchy::CViewObjHierarchy()
{
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
	ON_COMMAND_RANGE(ID_NEW_BOX, ID_NEW_OBJWAVEFRONT, OnNewObj)
	ON_COMMAND(ID_DELETE_OBJ, OnDeleteObj)
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
		if (CobjcontainerDoc::OP_NAMECH == op)
		{
			HTREEITEM hItem = FindItem(pObj);
			ASSERT(NULL != hItem);
			CString strName;
			pObj->GetName(strName);
			m_wndObjsTreeView.SetItemText(hItem, strName);
		}
	}
}

HTREEITEM CViewObjHierarchy::FindItem(CObject3D* pObj)
{
	HTREEITEM hRoot = m_wndObjsTreeView.GetRootItem();
	std::queue<HTREEITEM> tq;
	tq.push(hRoot);
	HTREEITEM hFind = NULL;
	while(!tq.empty()
		&& NULL == hFind)
	{
		HTREEITEM hNode = tq.front();
		tq.pop();
		TVITEM item;
		item.mask = TVIF_PARAM;
		item.hItem = hNode;
		m_wndObjsTreeView.GetItem(&item);
		CObject3D* pItem = (CObject3D *)(item.lParam);
		if (pItem == pObj)
			hFind = hNode;
		if (NULL == hFind)
		{
			HTREEITEM hChild = m_wndObjsTreeView.GetChildItem(hNode);
			while (NULL != hChild)
			{
				tq.push(hChild);
				hChild = m_wndObjsTreeView.GetNextSiblingItem(hChild);
			}
		}
	}
	return hFind;

}


BOOL CViewObjHierarchy::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (wParam == ID_TREEVIEW)
	{
		NMHDR* pNMHDR = (NMHDR*)lParam;
		ASSERT(pNMHDR != NULL);
		if (pNMHDR && pNMHDR->code == TVN_SELCHANGED)
		{
			Item item;
			GetSelectedItem(item);
			CobjcontainerDoc* pDoc = GetDocument();
			pDoc->UpdateAllViews(this, CobjcontainerDoc::OP_SEL, item.pItem);
		}
		else if (pNMHDR && pNMHDR->code == TVN_BEGINDRAG)
		{
			struct _DragOP
			{
				Item from;
				Item to;
			} op = {
				{NULL, NULL}
				, {NULL, NULL}
			};

			enum {Dragging, Dropping} state;
			state = Dragging;

			LPNMTREEVIEW pnmtv = (LPNMTREEVIEW)lParam;
			op.from.hItem = pnmtv->itemNew.hItem;
			op.from.pItem = (CObject3D*)pnmtv->itemNew.lParam;

			if (!op.from.pItem->IsKindOf(RUNTIME_CLASS(CScene)))
			{
				CImageList* lstImg = m_wndObjsTreeView.CreateDragImage(op.from.hItem);
				IMAGEINFO info;
				lstImg->GetImageInfo(0, &info);
				CRect rcImg(info.rcImage);
				lstImg->BeginDrag(0, rcImg.CenterPoint());
				CImageList::DragEnter(&m_wndObjsTreeView, pnmtv->ptDrag);
				m_wndObjsTreeView.SetCapture();

				while (state == Dragging)
				{
					MSG msg;
					::GetMessage(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST);
					if (msg.message == WM_MOUSEMOVE
						&& msg.wParam == MK_LBUTTON)
					{
						CPoint pt(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
						CImageList::DragMove(pt);
						CImageList::DragShowNolock(false);
						UINT uFlags = 0;
						HTREEITEM hItem = m_wndObjsTreeView.HitTest(pt, &uFlags);
						//if ((hItem != NULL) && (TVHT_ONITEM & uFlags))
						if (NULL != hItem )
						{
							m_wndObjsTreeView.SelectDropTarget(hItem);
						}
						CImageList::DragShowNolock(true);

					}
					else if (msg.message == WM_LBUTTONUP)
					{
						state = Dropping;
						CPoint pt(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
						op.to.hItem = m_wndObjsTreeView.HitTest(pt);
						TVITEM item;
						item.mask = TVIF_PARAM;
						item.hItem = op.to.hItem;
						m_wndObjsTreeView.GetItem(&item);
						op.to.pItem = (CObject3D *)(item.lParam);
						CImageList::DragLeave(&m_wndObjsTreeView);
						CImageList::EndDrag();
						ReleaseCapture();
					}
					else
						::DispatchMessage(&msg);

				}

				if (NULL != op.to.hItem
					&& NULL != op.to.pItem)
				{
					m_wndObjsTreeView.SelectDropTarget(NULL);
					TRACE(_T("%s->%s\n"), op.from.pItem->GetRuntimeClass()->m_lpszClassName, op.to.pItem->GetRuntimeClass()->m_lpszClassName);
					if (CObject3D::Connect(op.to.pItem, op.from.pItem))
					{
						CobjcontainerDoc* pDoc = GetDocument();
						pDoc->UpdateAllViews(this, CobjcontainerDoc::OP_CNN_PARENT, op.to.pItem);
						pDoc->UpdateAllViews(this, CobjcontainerDoc::OP_CNN_CHILD, op.from.pItem);
#if !defined TEST_CNN
						m_wndObjsTreeView.DeleteItem(op.from.hItem);
						HTREEITEM c = m_wndObjsTreeView.GetChildItem(op.to.hItem);
						while(c != NULL)
						{
							HTREEITEM n = m_wndObjsTreeView.GetNextSiblingItem(c);
							m_wndObjsTreeView.DeleteItem(c);
							c = n;
						}
						Reload(op.to);
#endif
					}
					else
						AfxMessageBox(_T("Not a legal tree if the node is dropped there!!!"));

#ifdef TEST_CNN
					m_wndObjsTreeView.DeleteAllItems();
					FillObjHierarchy();
#endif

				}
				//SetCursor(cursorRestore);
				delete lstImg;
				//m_wndObjsTreeView.Invalidate();
			}

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

	Item unit = {hRoot, root};
	Reload(unit);

}

void CViewObjHierarchy::Reload(Item& unit, bool expand)
{
	UINT uMask = TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_TEXT;
	UINT nState = TVIS_BOLD;
	UINT nStateMask = TVIS_BOLD;

	CRuntimeClass *prt = NULL;
	CString str;

	std::queue<Item> queBFS;
	queBFS.push(unit);
	while(!queBFS.empty())
	{
		Item unitThis = queBFS.front();
		queBFS.pop();
		Item unitNext = {NULL, unitThis.pItem->GetFirstChild()};
		HTREEITEM hParent = unitThis.hItem;
		HTREEITEM hInsertAfter = NULL;

		while (NULL != unitNext.pItem)
		{
			prt = unitNext.pItem->GetRuntimeClass();
			UINT nImg = TreeItem(prt->m_lpszClassName);
			UINT nSelectedImg = TreeItem(prt->m_lpszClassName);
			unitNext.pItem->GetName(str);
			LPARAM lParam = (LPARAM)unitNext.pItem;
			unitNext.hItem = m_wndObjsTreeView.InsertItem(
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

			hInsertAfter = unitNext.hItem;
			unitNext.pItem = unitNext.pItem->GetNextSibbling();;
		}
	}
	UINT nCode = expand?TVE_EXPAND:TVE_COLLAPSE;
	m_wndObjsTreeView.Expand(unit.hItem, nCode);
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


VOID CViewObjHierarchy::OnDeleteObj()
{
	Item item;
	GetSelectedItem(item);
	if (NULL != item.pItem
		&& !item.pItem->IsKindOf(RUNTIME_CLASS(CScene)))
	{
		CobjcontainerDoc* pDoc = GetDocument();
		pDoc->UpdateAllViews(this, CobjcontainerDoc::OP_DEL, item.pItem);
		item.pItem->RemoveSelf();
		m_wndObjsTreeView.DeleteItem(item.hItem);
	}
}

void CViewObjHierarchy::GetSelectedItem(Item& a_item)
{
	HTREEITEM hItem = NULL;
	CObject3D* pObj = NULL;
	if (NULL != (hItem = m_wndObjsTreeView.GetSelectedItem()))
	{
		TVITEM item;
		item.mask = TVIF_PARAM;
		item.hItem = hItem;
		m_wndObjsTreeView.GetItem(&item);
		pObj = (CObject3D *)(item.lParam);
	}
	a_item.pItem = pObj;
	a_item.hItem = hItem;
	ASSERT((NULL == hItem) == (NULL == pObj));
}

void CViewObjHierarchy::OnNewObj(UINT nID)
{
	Item parent;
	GetSelectedItem(parent);
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
	m_wndObjsTreeView.SelectItem(child.hItem);
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


