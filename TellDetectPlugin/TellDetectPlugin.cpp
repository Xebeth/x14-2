/**************************************************************************
	created		:	2011-06-04
	filename	: 	TellDetectPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Tell detect plugin
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>

#include <IGameChatPlugin.h>
#include <PluginEngine.h>

#include <Mmsystem.h>

#include "TellDetectPlugin.h"
#include "version.h"

using namespace PluginFramework;

namespace Windower
{
	/*! \brief Creates an instance of TellDetectPlugin
		\param[in] pServices_in : a pointer to the plugin services
		\return a pointer to the new TellDetectPlugin instance
	*/
	IPlugin* TellDetectPlugin::Create(PluginFramework::IPluginServices *pServices_in)
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
	void TellDetectPlugin::Query(PluginInfo& PluginInfo_out)
	{
		PluginInfo_out.SetDesc(_T("This plugin will play a sound whenever the player receives a tell"));
		PluginInfo_out.SetIdentifier(_T("BC725A17-4E60-4EE2-9E48-EF33D7CBB7E9"));
		PluginInfo_out.SetVersion(PLUGIN_VERSION);
		PluginInfo_out.SetName(_T("TellDetect"));
		PluginInfo_out.SetAuthor(_T("Xebeth`"));
	}
	
	/*! \brief Opens the configuration screen of the plugin
		\param[out] pInstance_in : the instance of the plugin to configure
		\return true if the user validated the configuration screen; false otherwise
	*/
	bool TellDetectPlugin::Configure(PluginFramework::IPlugin *pInstance_in)
	{
		MessageBox(NULL, _T("This plugin has no configuration."), _T(MODULE_FILENAME), MB_OK | MB_ICONINFORMATION);

		return true;
	}

	/*! \brief Callback invoked when the game chat receives a new line
		\param[in] MessageType_in : the type of the message
		\param[in] pSender_in : the sender of the message
		\param[in,out] pMessage_in_out : the message (might have been modified by other plugins)
		\param[in] pOriginalMsg_in : a pointer to the unmodified message
		\param[in] dwOriginalMsgSize_in : the size of the original message
		\param[in] pBuffer_in_out : the resulting text modified by the plugin
		\param[in] Unsubscribe_out : flag specifying if the plugin wants to revoke its subscription to the hook
		\return true if the message was logged; false otherwise
	*/
	bool TellDetectPlugin::OnChatMessage(USHORT MessageType_in, const StringNode* pSender_in_out,
										 StringNode* pMessage_in_out, const char *pOriginalMsg_in,
										 DWORD dwOriginalMsgSize_in, char **pBuffer_in_out,
										 bool &Unsubscribe_out)
	{
		if (MessageType_in == CHAT_MESSAGE_TYPE_INCOMING_TELL_MESSAGE)
			PlaySound(_T("tell.wav"), NULL, SND_FILENAME | SND_ASYNC);

		return true;
	}
}

using Windower::TellDetectPlugin;

/*! \brief Function exposed by the plugin DLL to initialize the plugin object
	\return a pointer to the plugin registration parameters if successful; NULL otherwise
*/
extern "C" PLUGIN_API RegisterParams* InitPlugin()
{
	return PluginFramework::IPlugin::Initialize(TellDetectPlugin::Create, TellDetectPlugin::Destroy,
												TellDetectPlugin::Query, TellDetectPlugin::Configure);
}