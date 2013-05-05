/**************************************************************************
	created		:	2011-06-04
	filename	: 	TellDetectPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Tell detect plugin
**************************************************************************/
#include "stdafx.h"
#include "resource.h"
#include <afxdlgs.h>

#include <PluginPropertyPage.h>

#include "TellDetectConfigDlg.h"
#include "TellDetectPlugin.h"
#include "version.h"

namespace Windower
{
	//! \brief TellDetectPlugin constructor
	TellDetectPlugin::TellDetectPlugin(PluginFramework::IPluginServices *pServices_in)
		: IGameChatPlugin(pServices_in)
	{
		// create the settings
		m_pSettings = new Windower::PluginSettings(IPlugin::GetConfigFile(), NULL);
		// set the sound file path from the settings
		OnSettingsChanged();
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
	
	/*! \brief Retrieves the property page used to configure the plugin
		\return a pointer to the property page of the plugin
	*/
	Windower::PluginPropertyPage* TellDetectPlugin::GetPropertyPage()
	{
		if (m_pSettings != NULL)
			return new TellDetectConfigDlg(m_pSettings);

		return NULL;
	}

	/*! \brief Callback invoked when the game chat receives a new line
		\param[in] MessageType_in : the type of the message
		\param[in] pSender_in : the sender of the message
		\param[in] MsgSize_in : the size of the unmodified message
		\param[in] pOriginalMsg_in : a pointer to the unmodified message
		\param[in] pModifiedMsg_in_out : the resulting text modified by the plugin
		\param[in] DWORD ModifiedSize_in : the modified message size
		\return the new size of the message
	*/
	DWORD TellDetectPlugin::OnChatMessage(USHORT MessageType_in, const char* pSender_in, DWORD MsgSize_in,
										  const char *pOriginalMsg_in, char **pModifiedMsg_in_out, DWORD ModifiedSize_in)
	{
		if (MessageType_in == CHAT_MESSAGE_TYPE_INCOMING_TELL_MESSAGE)
			PlaySound(m_SoundFile.c_str(), NULL, SND_FILENAME | SND_ASYNC);

		return MsgSize_in;
	}

	//! \brief Callback function invoked when the settings have changed
	void TellDetectPlugin::OnSettingsChanged()
	{
		m_SoundFile = m_pSettings->GetString(TELL_SOUND_KEY, TELL_SOUND_DEFAULT);
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
												TellDetectPlugin::Query, Windower::ConfigurablePlugin::Configure);
}