/**************************************************************************
	created		:	2011-10-29
	filename	: 	AutoLoginConfigApp.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	AutoLogin config application
**************************************************************************/
#ifndef __AUTOLOGIN_CONFIG_APP_H__
#define __AUTOLOGIN_CONFIG_APP_H__

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

//! \brief AutoLogin config application
class AutoLoginConfigApp : public CWinApp
{
public:
	virtual BOOL InitInstance();
};

extern AutoLoginConfigApp g_pApp;

#endif//__AUTOLOGIN_CONFIG_APP_H__