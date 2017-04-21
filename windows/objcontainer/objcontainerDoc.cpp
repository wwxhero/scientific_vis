
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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CobjcontainerDoc

IMPLEMENT_DYNCREATE(CobjcontainerDoc, CDocument)

BEGIN_MESSAGE_MAP(CobjcontainerDoc, CDocument)
END_MESSAGE_MAP()


// CobjcontainerDoc construction/destruction

CobjcontainerDoc::CobjcontainerDoc()
{
	// TODO: add one-time construction code here

}

CobjcontainerDoc::~CobjcontainerDoc()
{
}

BOOL CobjcontainerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CobjcontainerDoc serialization

void CobjcontainerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
#ifdef TEST_SIERALIZATION
		srand(::GetTickCount());
		CObject3D *pObj = new CObject3D();
		m_arrObjs.Add(pObj);
		CObject3D *pObj2 = new CObject3D();
		m_arrObjs.Add(pObj2);
		m_arrObjs.Serialize(ar);
		m_arrObjs.RemoveAll();
		delete pObj;
		delete pObj2;
#endif
	}
	else
	{
#ifdef TEST_SIERALIZATION
		m_arrObjs.Serialize(ar);
		INT_PTR sz = m_arrObjs.GetSize();
		for (INT_PTR i = 0; i < sz; i ++)
			delete m_arrObjs.GetAt(i);
		m_arrObjs.RemoveAll();
#endif
	}

}

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
