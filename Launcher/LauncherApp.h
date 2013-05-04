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
}
class LauncherCmdLine : public CCommandLineInfo
{
	enum eCommandFlags
	{
		FLAG_NONE		= -1,
		FLAG_FIRST_RUN	=  0,	//	/firstrun
		FLAG_PROFILE,			//	/profile
		FLAG_SID,				//	/SID
		FLAG_COUNT
	};
public:
	LauncherCmdLine()
		: m_LastFlag(FLAG_NONE),
			m_bFirstRun(NULL) {}

	void ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast);

	/*! \brief Retrieves the profile name found on the command line
		\param[out] ProfileName_out : string receiving the profile name
		\return a const reference to ProfileName_out
	*/
	const CString& GetProfileName(CString &ProfileName_out) const
	{ ProfileName_out = m_ProfileName; return ProfileName_out; }
	/*! \brief Retrieves the SID found on the command line
		\param[out] ProfileName_out : string receiving the profile name
		\return a const reference to ProfileName_out
	*/
	const CString& GetSID(CString &SID_out) const
	{ SID_out = m_SID; return SID_out; }

	bool IsFirstRun() const
	{ return m_bFirstRun; }

protected:
	//! Flag specifying if the first run wizard should be displayed
	bool m_bFirstRun;
	//! SID passed on the command line
	CString m_SID;
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

	BOOL InitInstance();

	static CString& ResolveLink(HWND hWnd_in, const TCHAR *pLinkPath_in, CString &LinkTarget_out);

protected:
	class DummyServices : public PluginFramework::IPluginServices
	{
	public:
		DummyServices(const PluginFramework::VersionInfo &Version_in, const string_t& WorkingDir_in)
			: PluginFramework::IPluginServices(Version_in), m_WorkingDir(WorkingDir_in) {}
	private:
		bool InvokeService(const string_t&, const string_t&, PluginFramework::ServiceParam &) const { return true; }
		bool UnsubscribeService(const string_t&, const string_t&, PluginFramework::IPlugin*) const { return true; }
		bool SubscribeService(const string_t&, const string_t&, PluginFramework::IPlugin*) const { return true; }
		const TCHAR* GetConfigFile() const { return m_WorkingDir.c_str(); }

		string_t m_WorkingDir;
	};

	bool CreateCmdLine(string_t &CmdLine_out, const string_t &GamePath_in,
						long LanguageID_in, const TCHAR *pSID_in) const;

	PluginFramework::PluginManager *m_pPluginManager;
	Windower::SettingsManager *m_pSettingsManager;
	DummyServices *m_pDummyServices;
};

extern LauncherApp g_pApp;


#endif//__LAUNCHERAPP_H__
