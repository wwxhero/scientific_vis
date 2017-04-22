
// objcontainerDoc.h : interface of the CobjcontainerDoc class
//


#pragma once
#include "Scene.h"

class CobjcontainerDoc : public CDocument
{
protected: // create from serialization only
	CobjcontainerDoc();
	DECLARE_DYNCREATE(CobjcontainerDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
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
		return &m_scene;
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


	CScene m_scene;
};
