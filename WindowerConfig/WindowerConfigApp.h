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

class ConfigCmdLine : public CCommandLineInfo
{
public:
	enum eCommandFlags
	{
		FLAG_PROFILE = 0,	// /profile or /Profile
		FLAG_COUNT
	};

	virtual void ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast);

	/*! \brief Retrieves the profile name found on the command line
		\param[out] ProfileName_out : string receiving the profile name
		\return a const reference to ProfileName_out
	*/
	const CString& GetProfileName(CString &ProfileName_out)
	{ ProfileName_out = m_ProfileName; return ProfileName_out; }

protected:
	//! name of the profile passed on the command line
	CString m_ProfileName;
	//! last flag parsed
	eCommandFlags m_LastFlag;
};

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