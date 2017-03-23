/**************************************************************************
	created		:	2013-04-18
	filename	: 	LauncherApp.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __LAUNCHERAPP_H__
#define __LAUNCHERAPP_H__

#define GAMEVER_BUFFER_SIZE 24

namespace Windower
{
	class SettingsManager;
	class PluginServices;
}

namespace PluginFramework
{
	class IUserSettings;
}

class LauncherCmdLine : public CCommandLineInfo
{
	enum eCommandFlags
	{
		FLAG_NONE		= -1,
		FLAG_FIRST_RUN	=  0,	//	/firstrun
		FLAG_PROFILE,			//	/profile
		FLAG_COUNT
	};
public:
	LauncherCmdLine() : m_bFirstRun(false), m_LastFlag(FLAG_NONE) {}

	void ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast) override;

	/*! \brief Retrieves the profile name found on the command line
		\param[out] ProfileName_out : string receiving the profile name
		\return a const reference to ProfileName_out
	*/
	const CString& GetProfileName(CString &ProfileName_out) const
	{ ProfileName_out = m_ProfileName; return ProfileName_out; }

	bool IsFirstRun() const
	{ return m_bFirstRun; }

protected:
	//! Flag specifying if the first run wizard should be displayed
	bool m_bFirstRun;
	//! name of the profile passed on the command line
	CString m_ProfileName;
	//! last flag parsed
	eCommandFlags m_LastFlag;
};

//! \brief Windower config application
class LauncherApp : public CWinAppEx
{
public:
	LauncherApp();
	~LauncherApp();

	BOOL InitInstance() override;

	static CString& ResolveLink(HWND hWnd_in, const TCHAR *pLinkPath_in, CString &LinkTarget_out);
	static bool CreateLink(const string_t &SavePath_in, const string_t &LinkTarget_in,
						   const string_t &WorkingDir_in, const TCHAR *pArgs_in = nullptr,
						   const TCHAR *pDesc_in = nullptr, int IconIndex_in = 0);

protected:
	PluginFramework::PluginManager *m_pPluginManager;
	Windower::SettingsManager *m_pSettingsManager;
	Windower::PluginServices *m_pPluginServices;
	Windower::CoreModules m_Modules;
private:
	void InitMFC();
};

extern LauncherApp g_pApp;


#endif//__LAUNCHERAPP_H__
