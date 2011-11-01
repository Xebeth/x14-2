/**************************************************************************
	created		:	2011-10-29
	filename	: 	WindowerConfigApp.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower config application
**************************************************************************/
#ifndef __WINDOWER_CONFIG_APP_H__
#define __WINDOWER_CONFIG_APP_H__

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

//! \brief Windower config application
class WindowerConfigApp : public CWinApp
{
public:
	WindowerConfigApp();
	~WindowerConfigApp();

	virtual BOOL InitInstance();

protected:
	Windower::SettingsManager *m_pSettingsManager;
};

extern WindowerConfigApp g_pApp;

#endif//__WINDOWER_CONFIG_APP_H__