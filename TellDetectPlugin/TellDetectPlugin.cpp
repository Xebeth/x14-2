/**************************************************************************
	created		:	2011-06-04
	filename	: 	TellDetectPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Tell detect plugin
**************************************************************************/
#include "stdafx.h"
#include "resource.h"

#include "TellDetectConfigDlg.h"
#include "TellDetectSettings.h"
#include "TellDetectPlugin.h"
#include "version.h"

namespace Windower
{
	//! \brief TellDetectPlugin constructor
	TellDetectPlugin::TellDetectPlugin(PluginFramework::IPluginServices *pServices_in)
		: IGameChatPlugin(pServices_in), m_pSettings(new TellDetectSettings(IPlugin::GetConfigFile(), NULL))
	{
		if (m_pSettings != NULL)
			m_SoundFile = m_pSettings->GetFilename();
	}

	TellDetectPlugin::~TellDetectPlugin()
	{
		if (m_pSettings != NULL)
		{
			delete m_pSettings;
			m_pSettings = NULL;
		}
	}

	/*! \brief Creates an instance of TellDetectPlugin
		\param[in] pServices_in : a pointer to the plugin services
		\return a pointer to the new TellDetectPlugin instance
	*/
	PluginFramework::IPlugin* TellDetectPlugin::Create(PluginFramework::IPluginServices *pServices_in)
	{
		return new TellDetectPlugin(pServices_in);
	}

	/*! \brief Destroys an instance of TellDetectPlugin
		\param[in] pInstance_in : an instance of TellDetectPlugin
	*/
	void TellDetectPlugin::Destroy(IPlugin *pInstance_in)
	{
		if (pInstance_in != NULL)
		{
			delete pInstance_in;
			pInstance_in = NULL;
		}
	}

	/*! \brief Fills a VersionInfo structure with the plugin information
		\param[out] PluginInfo_out : a VersionInfo structure
	*/
	void TellDetectPlugin::Query(PluginFramework::PluginInfo& PluginInfo_out)
	{
		PluginInfo_out.SetDesc(_T("This plugin will play a sound whenever the player receives a tell"));
		PluginInfo_out.SetIdentifier(_T("BC725A17-4E60-4EE2-9E48-EF33D7CBB7E9"));
		PluginInfo_out.SetCompatibilityFlags(PLUGIN_COMPATIBILITY_WINDOWER);
		PluginInfo_out.SetVersion(PLUGIN_VERSION);
		PluginInfo_out.SetName(_T("TellDetect"));
		PluginInfo_out.SetAuthor(_T("Xebeth`"));
	}
	
	/*! \brief Opens the configuration screen of the plugin
		\param[in] pInstance_in : the instance of the plugin to configure
		\param[in] pUserData_in : a pointer to the user data to pass to the plugin
		\return true if the user validated the configuration screen; false otherwise
	*/
	bool TellDetectPlugin::Configure(PluginFramework::IPlugin *pInstance_in, const LPVOID pUserData_in)
	{
		TellDetectConfigDlg ConfigDlg(IPlugin::GetConfigFile(), reinterpret_cast<const TCHAR*>(pUserData_in));

		return (ConfigDlg.DoModal() == IDOK);
	}

	/*! \brief Callback invoked when the game chat receives a new line
		\param[in] MessageType_in : the type of the message
		\param[in] pSender_in : the sender of the message
		\param[in] MsgSize_in : the size of the unmodified message
		\param[in] pOriginalMsg_in : a pointer to the unmodified message
		\param[in] pModifiedMsg_in_out : the resulting text modified by the plugin
		\return the new size of the message if modified; 0 otherwise
	*/
	DWORD TellDetectPlugin::OnChatMessage(USHORT MessageType_in, const char* pSender_in, DWORD MsgSize_in,
										  const char *pOriginalMsg_in, char **pModifiedMsg_in_out)
	{
		if (MessageType_in == CHAT_MESSAGE_TYPE_INCOMING_TELL_MESSAGE)
			PlaySound(m_SoundFile.c_str(), NULL, SND_FILENAME | SND_ASYNC);

		return MsgSize_in;
	}
}

using Windower::TellDetectPlugin;

/*! \brief Function exposed by the plugin DLL to initialize the plugin object
	\param[out] RegisterParams_out : Registration structure to be able to use the plugin
	\return true if the initialization succeeded; false otherwise
*/
extern "C" PLUGIN_API bool InitPlugin(PluginFramework::RegisterParams &RegisterParams_out)
{
	return PluginFramework::IPlugin::Initialize(RegisterParams_out, TellDetectPlugin::Create, TellDetectPlugin::Destroy,
												TellDetectPlugin::Query, TellDetectPlugin::Configure);
}