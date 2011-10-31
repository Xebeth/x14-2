/**************************************************************************
	created		:	2011-06-04
	filename	: 	TellDetectPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Tell detect plugin
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>

#include <FormatChatMessageHook.h>
#include <IGameChatPlugin.h>
#include "TellDetectPlugin.h"

#include <Mmsystem.h>

const PluginFramework::IPluginServices* PluginFramework::IPlugin::m_pPluginServices = NULL;

namespace Windower
{
	/*! \brief Creates an instance of TellDetectPlugin
		\return a pointer to the new TellDetectPlugin instance
	*/
	PluginFramework::IPlugin* TellDetectPlugin::Create()
	{
		TellDetectPlugin *pNewInst = new TellDetectPlugin;
		TellDetectPlugin::Query(pNewInst->m_PluginInfo);

		if (m_pPluginServices->SubscribeService(_T("GameChat"), _T("OnChatMessage"), pNewInst) == false)
		{
			delete pNewInst;
			pNewInst = NULL;
		}

		return pNewInst;
	}

	/*! \brief Destroys an instance of TellDetectPlugin
		\param[in] pInstance_in : an instance of TellDetectPlugin
	*/
	void TellDetectPlugin::Destroy(PluginFramework::IPlugin *pInstance_in)
	{
		if (pInstance_in != NULL)
		{
			m_pPluginServices->UnsubscribeService(_T("GameChat"), _T("OnChatMessage"), pInstance_in);

			delete pInstance_in;
			pInstance_in = NULL;
		}
	}

	/*! \brief Fills a PluginInfo structure with the plugin information
		\param[out] Info_out : a PluginInfo structure
	*/
	void TellDetectPlugin::Query(PluginInfo& Info_out)
	{
		Info_out.Author = _T("Xebeth`");
		Info_out.Name = _T("TellDetect");
		Info_out.PluginVersion.FromString(_T("1.0.0"));
		Info_out.FrameworkVersion.FromString(_T("1.0.0"));
		Info_out.Descritpion = _T("This plugin will play a sound whenever the player receives a tell");
		Info_out.PluginIdentifier.FromString(_T("BC725A17-4E60-4EE2-9E48-EF33D7CBB7E9"));
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

/*! \brief Function exposed by the plugin DLL to retrieve the plugin information
	\param[in] PluginInfo_out : the plugin information
*/
extern "C" PLUGIN_API void QueryPlugin(PluginInfo &Info_out)
{
	TellDetectPlugin::Query(Info_out);
}

/*! \brief Function exposed by the plugin DLL to initialize the plugin object
	\param[in] PluginServices_in : the plugin services
	\return a pointer to the plugin registration parameters if successful; NULL otherwise
*/
extern "C" PLUGIN_API RegisterParams* InitPlugin(const PluginFramework::IPluginServices &ServicesParams_in)
{
	RegisterParams *pParams = new RegisterParams;

	TellDetectPlugin::Query(pParams->Info);

	pParams->QueryFunc = TellDetectPlugin::Query;
	pParams->CreateFunc = TellDetectPlugin::Create;
	pParams->DestroyFunc = TellDetectPlugin::Destroy;

	TellDetectPlugin::SetPluginServices(ServicesParams_in);

	return pParams;
}