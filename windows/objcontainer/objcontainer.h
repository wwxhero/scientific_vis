
// objcontainer.h : main header file for the objcontainer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CobjcontainerApp:
// See objcontainer.cpp for the implementation of this class
//

class CobjcontainerApp : public CWinAppEx
{
public:
	CobjcontainerApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();
	void GetModuleDirPath(CString& strPath);

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

private:
	CString m_strModuleDir;
};

extern CobjcontainerApp theApp;
