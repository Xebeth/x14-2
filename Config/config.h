// config.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CconfigApp:
// See config.cpp for the implementation of this class
//

class CconfigApp : public CWinApp
{
public:
	CconfigApp();
	~CconfigApp();

// Overrides
	virtual BOOL InitInstance();
// Implementation
	DECLARE_MESSAGE_MAP()
protected:
	SettingsManager *m_pSettingsManager;
};

extern CconfigApp g_pApp;