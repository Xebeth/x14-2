/**************************************************************************
	created		:	2013-04-18
	filename	: 	LauncherApp.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __LAUNCHERAPP_H__
#define __LAUNCHERAPP_H__

namespace Windower
{
	class SettingsManager;
}

class LauncherCmdLine : public CCommandLineInfo
{
public:
	enum eCommandFlags
	{
		FLAG_PROFILE = 0,	// /profile or /Profile
		FLAG_SID,
		FLAG_COUNT
	};

	virtual void ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast);

	/*! \brief Retrieves the profile name found on the command line
		\param[out] ProfileName_out : string receiving the profile name
		\return a const reference to ProfileName_out
	*/
	const CString& GetProfileName(CString &ProfileName_out)
	{ ProfileName_out = m_ProfileName; return ProfileName_out; }
	/*! \brief Retrieves the SID found on the command line
		\param[out] ProfileName_out : string receiving the profile name
		\return a const reference to ProfileName_out
	*/
	const CString& GetSID(CString &SID_out)
	{ SID_out = m_SID; return SID_out; }

protected:
	//! SID passed on the command line
	CString m_SID;
	//! name of the profile passed on the command line
	CString m_ProfileName;
	//! last flag parsed
	eCommandFlags m_LastFlag;
};

//! \brief Windower config application
class LauncherApp : public CWinApp
{
public:
	LauncherApp();
	~LauncherApp();

	virtual BOOL InitInstance();

protected:
	Windower::SettingsManager *m_pSettingsManager;
};

extern LauncherApp g_pApp;

#endif//__LAUNCHERAPP_H__
