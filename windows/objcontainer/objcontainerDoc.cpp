
// objcontainerDoc.cpp : implementation of the CobjcontainerDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "objcontainer.h"
#endif

#include "objcontainerDoc.h"

#include <propkey.h>

#include "Object3D.h"
#include <queue>
#include "ViewPane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CobjcontainerDoc

IMPLEMENT_DYNCREATE(CobjcontainerDoc, CDocument)

BEGIN_MESSAGE_MAP(CobjcontainerDoc, CDocument)
END_MESSAGE_MAP()


// CobjcontainerDoc construction/destruction

CobjcontainerDoc::CobjcontainerDoc() : m_pScene(NULL)
{
	// TODO: add one-time construction code here

}

CobjcontainerDoc::~CobjcontainerDoc()
{
	ClearScene();
}

void CobjcontainerDoc::ClearScene()
{
	if (NULL != m_pScene)
	{
		std::queue<CObject3D*> q;
		q.push(m_pScene);
		while (!q.empty())
		{
			CObject3D* n = q.front();
			CObject3D* c = n->GetFirstChild();
			while(c)
			{
				q.push(c);
				c = c->GetNextSibbling();
			}
			q.pop();
			delete n;
		}
		m_pScene = NULL;
	}
}

BOOL CobjcontainerDoc::OnNewDocument()
{
	ClearScene();
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	m_pScene = new CScene();
	return TRUE;
}

BOOL CobjcontainerDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	ClearScene();
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	return TRUE;
}

void CobjcontainerDoc::UpdateAllViews(CWnd* pSender, CobjcontainerDoc::OP op, CObject3D* pHint)
{
	if (pSender->IsKindOf(RUNTIME_CLASS(CView)))
	{
		CDocument::UpdateAllViews(static_cast<CView*>(pSender), LPARAM(op), pHint);
		pSender = NULL;
	}
	else
		CDocument::UpdateAllViews(NULL, op, pHint);
	for (std::list<CViewPane*>::iterator it = m_lstViews.begin(); it != m_lstViews.end(); it ++)
	{
		CViewPane* pView = *it;
		pView->OnUpdate(pSender, op, pHint);
	}
}



// CobjcontainerDoc serialization

void CobjcontainerDoc::Serialize(CArchive& ar)
{
#ifdef TEST_SIERALIZATION
	TestBasicSerialization(ar);
	TestTreeSerialization(ar);
#endif
	if (ar.IsStoring())
	{
		std::queue<CObject3D*> tq2;
		tq2.push(m_pScene);
		CObArray poolObjs;
		while(!tq2.empty())
		{
			CObject3D* n = tq2.front();
			tq2.pop();
			CObject3D* c = n->GetFirstChild();
			while (c)
			{
				tq2.push(c);
				c = c->GetNextSibbling();
			}
			n->SaveTreeNode(poolObjs);
		}
		poolObjs.Serialize(ar);
	}
	else
	{
		CObArray poolObjs;
		poolObjs.Serialize(ar);
		if (!poolObjs.IsEmpty())
		{
			CObject* pObj = poolObjs.GetAt(0);
			ASSERT(pObj->IsKindOf(RUNTIME_CLASS(CObject3D)));
			CObject3D* root = static_cast<CObject3D*>(pObj);
			for (int i = 0; i < poolObjs.GetCount(); i ++)
			{
				pObj = poolObjs.GetAt(i);
				ASSERT(pObj->IsKindOf(RUNTIME_CLASS(CObject3D)));
				CObject3D* node = static_cast<CObject3D*>(pObj);
				node->RestoreTreeNode(poolObjs);
			}

			ASSERT(root->IsKindOf(RUNTIME_CLASS(CScene)));
			m_pScene = static_cast<CScene*>(root);
		}
	}
}

#ifdef TEST_SIERALIZATION
void CobjcontainerDoc::TestBasicSerialization(CArchive& ar)
{
	CObArray arrObjs;
	if (ar.IsStoring())
	{
		// TODO: add storing code here
		TRACE(_T("Storing...\n"));
		srand(::GetTickCount());
		CObject3D *pObj = new CObject3D();
		arrObjs.Add(pObj);
		CObject3D *pObj2 = new CObject3D();
		arrObjs.Add(pObj2);
		arrObjs.Serialize(ar);
		arrObjs.RemoveAll();
		pObj->DumpData();
		pObj2->DumpData();
		delete pObj;
		delete pObj2;
	}
	else
	{
		TRACE(_T("Loading...\n"));
		arrObjs.Serialize(ar);
		INT_PTR sz = arrObjs.GetSize();
		for (INT_PTR i = 0; i < sz; i ++)
		{
			CObject *pObj = arrObjs.GetAt(i);
			ASSERT(pObj->IsKindOf(RUNTIME_CLASS(CObject3D)));
			(static_cast<CObject3D*>(pObj))->DumpData();
			delete pObj;
		}
		arrObjs.RemoveAll();
	}
}

void CobjcontainerDoc::TestTreeSerialization(CArchive& ar)
{
	CObArray arrObjs;
	if (ar.IsStoring())
	{
		TRACE(_T("Storing Tree...\n"));
		const int c_nullIdx = -1;
		struct stNode {
			int id;
			int left;
			int right;
			CObject3D* obj;
		} nodes [] = {
			{0, 1, 2, NULL}
			, {1, 3, 4, NULL}
			, {2, c_nullIdx, c_nullIdx, NULL}
			, {3, c_nullIdx, c_nullIdx, NULL}
			, {4, c_nullIdx, c_nullIdx, NULL}
		};

		TRACE(_T("Source tree...\n"));
		std::queue<stNode*> tq;
		CObject3D* root = new CObject3D();
		root->setNodeId(nodes[0].id);
		nodes[0].obj = root;
		tq.push(&nodes[0]);
		while(!tq.empty())
		{
			stNode* n = tq.front();
			tq.pop();
			TRACE(_T("%d: %d %d\n"), n->id, n->left, n->right);

			int iChildren[] = {n->left, n->right};
			CObject3D* nP = n->obj;
			for (int ic = 0; ic < sizeof(iChildren)/sizeof(int); ic ++)
			{
				if (c_nullIdx != iChildren[ic])
				{
					CObject3D* nC = new CObject3D();
					nC->setNodeId(nodes[iChildren[ic]].id);
					nodes[iChildren[ic]].obj = nC;
					nP->AddChild(nC);
					tq.push(&nodes[iChildren[ic]]);
				}
			}
		}

		TRACE(_T("Destination tree...\n"));
		std::queue<CObject3D*> tq2;
		tq2.push(root);
		CObArray poolObjs;
		while(!tq2.empty())
		{
			CObject3D* n = tq2.front();
			tq2.pop();
			TRACE(_T("%d:"), n->getNodeId());
			CObject3D* c = n->GetFirstChild();
			while (c)
			{
				TRACE(_T(" %d"), c->getNodeId());
				tq2.push(c);
				c = c->GetNextSibbling();
			}
			TRACE(_T("\n"));
			n->SaveTreeNode(poolObjs);
		}
		poolObjs.Serialize(ar);
		for (int i = 0; i < poolObjs.GetCount(); i ++)
		{
			CObject* obj = poolObjs.GetAt(i);
			delete obj;
		}
		poolObjs.RemoveAll();
	}
	else
	{
		TRACE(_T("Loading Tree...\n"));
		CObArray poolObjs;
		poolObjs.Serialize(ar);
		if (!poolObjs.IsEmpty())
		{
			CObject* pObj = poolObjs.GetAt(0);
			ASSERT(pObj->IsKindOf(RUNTIME_CLASS(CObject3D)));
			CObject3D* root = static_cast<CObject3D*>(pObj);
			for (int i = 0; i < poolObjs.GetCount(); i ++)
			{
				pObj = poolObjs.GetAt(i);
				ASSERT(pObj->IsKindOf(RUNTIME_CLASS(CObject3D)));
				CObject3D* node = static_cast<CObject3D*>(pObj);
				node->RestoreTreeNode(poolObjs);
			}

			std::queue<CObject3D*> tq;
			tq.push(root);
			while(!tq.empty())
			{
				CObject3D* n = tq.front();
				tq.pop();
				TRACE(_T("%d:"), n->getNodeId());
				CObject3D* c = n->GetFirstChild();
				while (c)
				{
					TRACE(_T(" %d"), c->getNodeId());
					tq.push(c);
					c = c->GetNextSibbling();
				}
				TRACE(_T("\n"));
			}

			for (int i = 0; i < poolObjs.GetCount(); i ++)
				delete poolObjs.GetAt(i);
			poolObjs.RemoveAll();
		}
	}
}

#endif

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CobjcontainerDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CobjcontainerDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CobjcontainerDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}


#endif // SHARED_HANDLERS

// CobjcontainerDoc diagnostics

#ifdef _DEBUG
void CobjcontainerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CobjcontainerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CobjcontainerDoc commands
