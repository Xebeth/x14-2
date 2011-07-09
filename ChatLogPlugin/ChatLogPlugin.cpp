/**************************************************************************
	created		:	2011-06-04
	filename	: 	ChatLogPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Chat log plugin
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <FormatChatMessageHook.h>
#include <IGameChatPlugin.h>
#include "ChatLogPlugin.h"

const PluginFramework::IPluginServices* PluginFramework::IPlugin::m_pPluginServices = NULL;

namespace Windower
{
	ChatLogPlugin::ChatLogPlugin() : IGameChatPlugin()
	{
		CreateDirectory(_T("logs"), NULL);
		m_bOpened = false;
		m_pFile = NULL;
	}

	ChatLogPlugin::~ChatLogPlugin()
	{
		if (m_bOpened && m_pFile != NULL)
		{
			string_t Line = _T("== End of session ==============================");
			WriteLine(Line);
			fclose(m_pFile);
			delete m_pFile;
			m_pFile = NULL;
		}
	}

	/*! \brief Creates an instance of ChatLogPlugin
		\return a pointer to the new ChatLogPlugin instance
	*/
	void* ChatLogPlugin::Create()
	{
		ChatLogPlugin *pNewInst = new ChatLogPlugin;
		ChatLogPlugin::Query(pNewInst->m_BasePluginInfo);

		if (m_pPluginServices->SubscribeService(_T("GameChat"), _T("FormatChatMessage"), pNewInst) == false)
		{
			delete pNewInst;
			pNewInst = NULL;
		}

		return pNewInst;
	}

	/*! \brief Destroys an instance of ChatLogPlugin
		\param[in] pInstance_in : an instance of ChatLogPlugin
	*/
	void ChatLogPlugin::Destroy(void *pInstance_in)
	{
		if (pInstance_in != NULL)
		{
			m_pPluginServices->UnsubscribeService(_T("GameChat"), _T("FormatChatMessage"), (IPlugin*)pInstance_in);

			delete (ChatLogPlugin *)pInstance_in;
			pInstance_in = NULL;
		}
	}

	/*! \brief Fills a PluginInfo structure with the plugin information
		\param[out] Info_out : a PluginInfo structure
	*/
	void ChatLogPlugin::Query(PluginInfo& Info_out)
	{
		Info_out.Author = _T("Xebeth`");
		Info_out.Name = _T("Chat log");
		Info_out.PluginVersion.FromString(_T("1.0.0"));
		Info_out.FrameworkVersion.FromString(_T("1.0.0"));
		Info_out.Descritpion = _T("This plugin will log the content of the game chat");
		Info_out.PluginIdentifier.FromString(_T("745E1230-0C81-4220-B099-3A3392EFA03A"));
	}

	bool ChatLogPlugin::FormatChatMessage(USHORT MessageType, const GameChatTextObject* pSender_in_out,
										  GameChatTextObject* pMessage_in_out, const char *pOriginalMsg_in,
										  DWORD dwOriginalMsgSize, char **pBuffer_in_out)
	{
		if (m_bOpened == false && m_pFile == NULL)
		{
			string_t LogFilename;
			SYSTEMTIME Time;

			GetSystemTime(&Time);
			format(LogFilename, _T("logs\\chatlog_%d-%02d-%02d.log"), Time.wYear, Time.wMonth, Time.wDay);

			m_bOpened = (_wfopen_s(&m_pFile, LogFilename.c_str(), _T("a+, ccs=UTF-8")) == 0);

			if (m_bOpened)
			{
				WriteLine(format(m_Buffer, _T("== New session : %ld-%02d-%02d %02d:%02d:%02d ==\r\n"),
								 Time.wYear, Time.wMonth, Time.wDay, Time.wHour, Time.wMinute, Time.wSecond));
			}
		}

		if (m_bOpened)
			WriteLine(format(m_Buffer, _T("%S: %S\r\n"), pSender_in_out->pResBuf, pOriginalMsg_in));

		return true;
	}

	bool ChatLogPlugin::WriteLine(string_t& Line_in)
	{
		if (m_pFile != NULL)
		{
			size_t DataWritten, BufferLength = Line_in.length();
			DataWritten = fwrite(Line_in.c_str(), BufferLength * sizeof(TCHAR), BufferLength, m_pFile);
			Line_in.clear();
			fflush(m_pFile);

			return (DataWritten == BufferLength);
		}

		return false;
	}
}

using Windower::ChatLogPlugin;

extern "C" PLUGIN_API void QueryPlugin(PluginInfo &Info_out)
{
	ChatLogPlugin::Query(Info_out);
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

		ChatLogPlugin::Query(pParams->Info);

		pParams->QueryFunc = ChatLogPlugin::Query;
		pParams->CreateFunc = ChatLogPlugin::Create;
		pParams->DestroyFunc = ChatLogPlugin::Destroy;

		ChatLogPlugin::SetPluginServices(pServicesParams_in);

		return pParams;
	}

	return NULL;
}