#pragma once

#include "objcontainerDoc.h"
// CViewPane

class CViewPane : public CDockablePane
{
	DECLARE_DYNAMIC(CViewPane)

public:
	CViewPane();
	virtual ~CViewPane();
	virtual void OnUpdate(CWnd* pSender, CobjcontainerDoc::OP op, CObject3D* obj) = 0;
protected:
	CobjcontainerDoc* GetDocument()
	{
		CFrameWnd* pFrame = (CFrameWnd*)AfxGetMainWnd();
		CobjcontainerDoc* pDoc = (CobjcontainerDoc*)(pFrame->GetActiveDocument());
		return pDoc;
	}

protected:
	DECLARE_MESSAGE_MAP()
};


