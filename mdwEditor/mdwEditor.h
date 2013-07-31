
// mdwEditor.h : main header file for the mdwEditor application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CmdwEditorApp:
// See mdwEditor.cpp for the implementation of this class
//

class CmdwEditorApp : public CWinApp
{
public:
	CmdwEditorApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CmdwEditorApp theApp;
