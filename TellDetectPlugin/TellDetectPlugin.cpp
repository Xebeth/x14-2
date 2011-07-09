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
	void* TellDetectPlugin::Create()
	{
		TellDetectPlugin *pNewInst = new TellDetectPlugin;
		TellDetectPlugin::Query(pNewInst->m_BasePluginInfo);

		if (m_pPluginServices->SubscribeService(_T("GameChat"), _T("FormatChatMessage"), pNewInst) == false)
		{
			delete pNewInst;
			pNewInst = NULL;
		}

		return pNewInst;
	}

	/*! \brief Destroys an instance of TellDetectPlugin
		\param[in] pInstance_in : an instance of TellDetectPlugin
	*/
	void TellDetectPlugin::Destroy(void *pInstance_in)
	{
		if (pInstance_in != NULL)
		{
			m_pPluginServices->UnsubscribeService(_T("GameChat"), _T("FormatChatMessage"), (IPlugin*)pInstance_in);

			delete (TellDetectPlugin *)pInstance_in;
			pInstance_in = NULL;
		}
	}

	/*! \brief Fills a PluginInfo structure with the plugin information
		\param[out] Info_out : a PluginInfo structure
	*/
	void TellDetectPlugin::Query(PluginInfo& Info_out)
	{
		Info_out.Author = _T("Xebeth`");
		Info_out.Name = _T("Tell detect");
		Info_out.PluginVersion.FromString(_T("1.0.0"));
		Info_out.FrameworkVersion.FromString(_T("1.0.0"));
		Info_out.Descritpion = _T("This plugin will play a sound whenever the player receives a tell");
		Info_out.PluginIdentifier.FromString(_T("BC725A17-4E60-4EE2-9E48-EF33D7CBB7E9"));
	}

	bool TellDetectPlugin::FormatChatMessage(USHORT MessageType, const GameChatTextObject* pSender_in_out,
											 GameChatTextObject* pMessage_in_out, const char *pOriginalMsg_in,
											 DWORD dwOriginalMsgSize, char **pBuffer_in_out)
	{
		if (MessageType == CHAT_MESSAGE_TYPE_INCOMING_TELL_MESSAGE)
			PlaySound(_T("tell.wav"), NULL, SND_FILENAME | SND_ASYNC);

		return true;
	}
}

using Windower::TellDetectPlugin;

extern "C" PLUGIN_API void QueryPlugin(PluginInfo &Info_out)
{
	TellDetectPlugin::Query(Info_out);
}

extern "C" PLUGIN_API int TerminatePlugin()
{
	return 0;
}

extern "C" PLUGIN_API RegisterParams* InitPlugin(const PluginFramework::IPluginServices *pServicesParams_in)
{
	if (pServicesParams_in != NULL)
	{
		RegisterParams *pParams = new RegisterParams;

		TellDetectPlugin::Query(pParams->Info);

		pParams->QueryFunc = TellDetectPlugin::Query;
		pParams->CreateFunc = TellDetectPlugin::Create;
		pParams->DestroyFunc = TellDetectPlugin::Destroy;

		TellDetectPlugin::SetPluginServices(pServicesParams_in);

		return pParams;
	}

	return NULL;
}