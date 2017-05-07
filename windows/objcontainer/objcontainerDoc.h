
// objcontainerDoc.h : interface of the CobjcontainerDoc class
//


#pragma once
#include <list>
#include "Scene.h"
class CViewPane;
class CobjcontainerDoc : public CDocument
{
protected: // create from serialization only
	CobjcontainerDoc();
	DECLARE_DYNCREATE(CobjcontainerDoc)

// Attributes
public:
	enum OP {OP_SEL = 0, OP_NEW, OP_DEL, OP_CNN_PARENT, OP_CNN_CHILD };
// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CobjcontainerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CObject3D* RootObj()
	{
		return m_pScene;
	}

	void ClearScene();

	void UpdateAllViews(CWnd* pSender, OP op, CObject3D* pHint);
	void RegisterView(CViewPane* pView)
	{
		bool registered = false;
		for(std::list<CViewPane*>::iterator it = m_lstViews.begin()\
			; !registered && it != m_lstViews.end()
			; it ++)
		{
			registered = (*it == pView);
		}
		if (!registered)
			m_lstViews.push_back(pView);
	}

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()



#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

private:

#ifdef TEST_SIERALIZATION
	void TestBasicSerialization(CArchive& ar);
	void TestTreeSerialization(CArchive& ar);
#endif


	CScene* m_pScene;
	std::list<CViewPane*> m_lstViews;

};
