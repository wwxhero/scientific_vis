
#include "stdafx.h"

#include "ViewProperties.h"
#include "Resource.h"
#include "MainFrm.h"
#include "objcontainer.h"
#include "PropertyItem.h"
#include "PropertyGridGroup.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CViewProperties::CViewProperties() : m_pActObj(NULL)
{
}

CViewProperties::~CViewProperties()
{
}

BEGIN_MESSAGE_MAP(CViewProperties, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_MESSAGE(WM_INITIALUPDATE, OnInitialUpdate)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, &CViewProperties::OnPropertyChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

void CViewProperties::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient,rectCombo;
	GetClientRect(rectClient);

	//m_wndObjectCombo.GetWindowRect(&rectCombo);

	int cyCmb = rectCombo.Size().cy;
	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	//m_wndObjectCombo.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), 200, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb + cyTlb, rectClient.Width(), rectClient.Height() -(cyCmb+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

void CViewProperties::OnUpdate(CWnd* pSender, CobjcontainerDoc::OP op, CObject3D* obj)
{
	if (pSender == this)
		return;
	if (op == CobjcontainerDoc::OP_SEL)
	{
		m_pActObj = obj;
		InitPropList();
	}
	else if(op == CobjcontainerDoc::OP_DEL)
	{
		if (m_pActObj == obj)
		{
			CobjcontainerDoc* pDoc = GetDocument();
			m_pActObj = pDoc->RootObj();
		}
		InitPropList();
	}
	else if(op == CobjcontainerDoc::OP_CNN_PARENT
		|| op == CobjcontainerDoc::OP_CNN_CHILD)
	{
		if (m_pActObj == obj)
			UpdatePropList();
	}
}

LRESULT CViewProperties::OnInitialUpdate(WPARAM, LPARAM)
{
	CobjcontainerDoc* pDoc = GetDocument();
	pDoc->RegisterView(this);
	m_pActObj = pDoc->RootObj();
	InitPropList();
	return 0;
}


int CViewProperties::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create combo:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	// if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, 1))
	// {
	// 	TRACE0("Failed to create Properties Combo \n");
	// 	return -1;      // fail to create
	// }

	// m_wndObjectCombo.AddString(_T("Application"));
	// m_wndObjectCombo.AddString(_T("Properties Window"));
	// m_wndObjectCombo.SetCurSel(0);

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("Failed to create Properties Grid \n");
		return -1;      // fail to create
	}

	//InitPropList();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* Is locked */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* Locked */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

void CViewProperties::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CViewProperties::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CViewProperties::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void CViewProperties::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CViewProperties::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CViewProperties::OnProperties1()
{
	// TODO: Add your command handler code here
	TRACE(_T("OnProperties1\n"));
}

void CViewProperties::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: Add your command update UI handler code here
}

void CViewProperties::OnProperties2()
{
	// TODO: Add your command handler code here
	TRACE(_T("OnProperties2\n"));
}

void CViewProperties::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: Add your command update UI handler code here
}

bool CViewProperties::UpdatePropList()
{
	//TODO: Update properties data
	ASSERT(NULL != m_pActObj);
	return m_wndPropList.Update(m_pActObj);
}

void CViewProperties::InitPropList()
{
	SetPropListFont();
	ASSERT(NULL != m_pActObj);
	m_wndPropList.Init(m_pActObj);
	UpdateTitle();
}

void CViewProperties::UpdateTitle()
{
	CString strTitle;
	strTitle.LoadString(IDS_PROPERTIES_WND);
	CString subTitle;
	m_pActObj->GetName(subTitle);
	CString strNTitle;
	strNTitle.Format(_T("%s-%s"), strTitle, subTitle);
	SetWindowText(strNTitle);
}

void CViewProperties::InitPropListObsolete()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("Appearance"));

	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("3D Look"), (_variant_t) false, _T("Specifies the window's font will be non-bold and controls will have a 3D border")));

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Border"), _T("Dialog Frame"), _T("One of: None, Thin, Resizable, or Dialog Frame"));
	pProp->AddOption(_T("None"));
	pProp->AddOption(_T("Thin"));
	pProp->AddOption(_T("Resizable"));
	pProp->AddOption(_T("Dialog Frame"));
	pProp->AllowEdit(FALSE);

	pGroup1->AddSubItem(pProp);
	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("Caption"), (_variant_t) _T("About"), _T("Specifies the text that will be displayed in the window's title bar")));

	m_wndPropList.AddProperty(pGroup1);

	CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("Window Size"), 0, TRUE);

	pProp = new CMFCPropertyGridProperty(_T("Height"), (_variant_t) 250l, _T("Specifies the window's height"));
	pProp->EnableSpinControl(TRUE, 50, 300);
	pSize->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty( _T("Width"), (_variant_t) 150l, _T("Specifies the window's width"));
	pProp->EnableSpinControl(TRUE, 50, 200);
	pSize->AddSubItem(pProp);

	m_wndPropList.AddProperty(pSize);

	CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("Font"));

	LOGFONT lf;
	CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	font->GetLogFont(&lf);

	lstrcpy(lf.lfFaceName, _T("Arial"));

	pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("Font"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("Specifies the default font for the window")));
	pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("Use System Font"), (_variant_t) true, _T("Specifies that the window uses MS Shell Dlg font")));

	m_wndPropList.AddProperty(pGroup2);

	CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("Misc"));
	pProp = new CMFCPropertyGridProperty(_T("(Name)"), _T("Application"));
	pProp->Enable(FALSE);
	pGroup3->AddSubItem(pProp);

	CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("Window Color"), RGB(210, 192, 254), NULL, _T("Specifies the default window color"));
	pColorProp->EnableOtherButton(_T("Other..."));
	pColorProp->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));
	pGroup3->AddSubItem(pColorProp);

	static const TCHAR szFilter[] = _T("Icon Files(*.ico)|*.ico|All Files(*.*)|*.*||");
	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Icon"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("Specifies the window icon")));

	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Folder"), _T("c:\\")));

	m_wndPropList.AddProperty(pGroup3);

	CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("Hierarchy"));

	CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("First sub-level"));
	pGroup4->AddSubItem(pGroup41);

	CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("Second sub-level"));
	pGroup41->AddSubItem(pGroup411);

	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 1"), (_variant_t) _T("Value 1"), _T("This is a description")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 2"), (_variant_t) _T("Value 2"), _T("This is a description")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 3"), (_variant_t) _T("Value 3"), _T("This is a description")));

	pGroup4->Expand(FALSE);
	m_wndPropList.AddProperty(pGroup4);
}


LRESULT CViewProperties::OnPropertyChanged(WPARAM wp,LPARAM lp)
{
	CMFCPropertyGridProperty* modified = (CMFCPropertyGridProperty*)lp;
	IUIProperty* pProperty = NULL;
	if (modified->IsKindOf(RUNTIME_CLASS(CPropertyItem)))
	{
		 pProperty = static_cast<CPropertyItem*>(modified);
	}
	else if(modified->IsKindOf(RUNTIME_CLASS(CPropertyItemFile)))
	{
		pProperty = static_cast<CPropertyItemFile*>(modified);
	}
	else if(modified->IsKindOf(RUNTIME_CLASS(CPropertyGridGroup)))
	{
		pProperty = static_cast<CPropertyGridGroup*>(modified);
	}
	ASSERT(NULL != pProperty);
	pProperty->Update(m_pActObj, false);
	CobjcontainerDoc* pDoc = GetDocument();
	CobjcontainerDoc::OP code = pProperty->GetCode();
	if (code != CobjcontainerDoc::OP_UNDEF)
		pDoc->UpdateAllViews(this, code, m_pActObj);
	UpdateTitle();
	return 0;
}

void CViewProperties::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CViewProperties::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
	TRACE(_T("OnSettingChange\n"));
}

void CViewProperties::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
	//m_wndObjectCombo.SetFont(&m_fntPropList);
}
