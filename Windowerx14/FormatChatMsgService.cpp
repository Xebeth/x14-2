/**************************************************************************
	created		:	2013-03-12
	filename	: 	FormatChatMsgService.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "StringNode.h"
#include "ModuleService.h"
#include "FormatChatMsgService.h"

namespace Windower
{
	FormatChatMsgService::CallingContext * FormatChatMsgService::m_pContext = NULL;
	LPVOID FormatChatMsgService::m_pThis = NULL;

	/*! \brief FormatChatMsgService constructor
		\param[in] Name_in : the name of the service
		\param[in] InvokePermission_in : flag specifying if the service can be invoked
	*/
	FormatChatMsgService::FormatChatMsgService(const string_t& Name_in, bool InvokePermission_in)
		: ModuleService(Name_in, InvokePermission_in)
	{
		// add compatible plugins
		StringUtils::UUID PluginUUID;

		m_CompatiblePlugins.insert(PluginUUID.FromString(_T("745E1230-0C81-4220-B099-3A3392EFA03A")));	// ChatLog
		m_CompatiblePlugins.insert(PluginUUID.FromString(_T("AF8B3EE1-B092-45C7-80AA-A2BF2213DA2B")));	// Timestamp
		m_CompatiblePlugins.insert(PluginUUID.FromString(_T("BC725A17-4E60-4EE2-9E48-EF33D7CBB7E9")));	// TellDetect
		m_CompatiblePlugins.insert(PluginUUID.FromString(_T("6FA271DC-DB0A-4B71-80D3-FE0B5DBF3BBF")));	// ExpWatch
	}

	/*! \brief Formats a message received by the game chat log
		\param[in] pThis_in_out : a pointer to the class containing the hooked method
		\param[in] MessageType_in : the type of the message
		\param[in,out] pSender_in_out : the sender of the message
		\param[in,out] pMessage_in_out : the message
		\return true if the message was formatted successfully; false otherwise
	*/
	bool WINAPI FormatChatMsgService::FormatChatMessageHook(LPVOID pThis_in_out, USHORT MessageType_in, StringNode* pSender_in_out,
															StringNode* pMessage_in_out, const __time64_t *pTimestamp_in)
	{
		m_pThis = pThis_in_out;

		if (m_pContext != NULL)
		{
			if (m_pContext->m_pTrampoline != NULL)
			{
				PluginFramework::PluginSet::const_iterator PluginIt, EndIt = m_pContext->m_Subscribers.cend();
				DWORD dwResult = 0UL, dwOriginalSize = pMessage_in_out->dwSize, dwNewSize = dwOriginalSize;
				const char *pOriginalMsg = pMessage_in_out->pResBuf;				
				char *pModifiedMsg = NULL;
				IGameChatPlugin *pPlugin;
				bool bResult = false;

				for (PluginIt = m_pContext->m_Subscribers.cbegin(); PluginIt != EndIt; ++PluginIt)
				{
					pPlugin = static_cast<IGameChatPlugin*>(*PluginIt);

					if (pPlugin != NULL)
					{
						dwResult = pPlugin->OnChatMessage(MessageType_in, pSender_in_out->pResBuf,
														  dwOriginalSize, pOriginalMsg, &pModifiedMsg, dwNewSize);

						if (pModifiedMsg != NULL && dwResult > dwNewSize)
							dwNewSize = dwResult;
					}
				}

				return FormatMessage(pThis_in_out, MessageType_in, pSender_in_out,
									 pMessage_in_out, pModifiedMsg, dwNewSize);
			}

			return false;
		}

		return false;
	}

	bool FormatChatMsgService::FormatMessage(LPVOID pThis_in_out, USHORT MessageType_in, StringNode* pSender_in_out,
											 StringNode* pMessage_in_out, char *pModifiedMsg_in, DWORD NewSize_in)
	{
		DWORD dwOriginalSenderSize = pSender_in_out->dwSize;
		char *pOriginalSender = pSender_in_out->pResBuf;
		StringNode OriginalMsg = *pMessage_in_out;
		bool Result = false;

		if (pModifiedMsg_in != NULL)
			UpdateNode(pModifiedMsg_in, NewSize_in, *pMessage_in_out);
		// display the error message instead of the typed command
		Result = ((fnFormatChatMessage)m_pContext->m_pTrampoline)(pThis_in_out, MessageType_in,
																  pSender_in_out, pMessage_in_out, NULL);
		// restore the original message and sender
		pSender_in_out->dwSize = dwOriginalSenderSize;
		pSender_in_out->pResBuf = pOriginalSender;
		// cleanup
		if (pModifiedMsg_in != NULL)
		{
			*pMessage_in_out = OriginalMsg;
			free(pModifiedMsg_in);
		}

		return Result;
	}

	//! Destroys the calling context for the service
	void FormatChatMsgService::DestroyContext()
	{
		if (m_pContext != NULL)
		{
			delete m_pContext;
			m_pContext = NULL;
		}
	}

	//! Creates the calling context for the service
	void FormatChatMsgService::CreateContext()
	{
		if (m_pContext == NULL)
		{
			LPVOID pTrampoline = m_ServiceHooks[FORMAT_CHAT_MESSAGE_HOOK];

			if (pTrampoline != NULL)
				m_pContext = new CallingContext((fnFormatChatMessage)pTrampoline, m_Subscribers);
		}
	}

	bool FormatChatMsgService::InjectMessage(const std::string &Msg_in,
											 const std::string &Sender_in,
											 UINT MessageType_in)
	{
		if (m_pContext != NULL && m_pThis != NULL)
		{
			StringNode DummySender, DummyMsg;

			InitStringNode(DummySender, Sender_in);
			InitStringNode(DummyMsg, Msg_in);			

			FormatChatMessageHook(m_pThis, MessageType_in, &DummySender, &DummyMsg, NULL);

			return true;
		}

		return false;
	}
}