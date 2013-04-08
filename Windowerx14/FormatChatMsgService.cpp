/**************************************************************************
	created		:	2013-03-12
	filename	: 	FormatChatMsgService.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "FormatChatMsgService.h"

namespace Windower
{
	FormatChatMsgService::CallingContext * FormatChatMsgService::m_pContext = NULL;

	/*! \brief FormatChatMsgService constructor
		\param[in] Name_in : the name of the service
		\param[in] Hooks_in : the hooks associated with the service
		\param[in] InvokePermission_in : flag specifying if the service can be invoked
	*/
	FormatChatMsgService::FormatChatMsgService(const string_t& Name_in, const HookPointers &Hooks_in, bool InvokePermission_in)
		: ModuleService(Name_in, Hooks_in, InvokePermission_in)
	{
		// add compatible plugins
		PluginFramework::PluginUUID UUID;

		m_CompatiblePlugins.insert(UUID.FromString(_T("745E1230-0C81-4220-B099-3A3392EFA03A")));	// ChatLog
		m_CompatiblePlugins.insert(UUID.FromString(_T("AF8B3EE1-B092-45C7-80AA-A2BF2213DA2B")));	// Timestamp
		m_CompatiblePlugins.insert(UUID.FromString(_T("BC725A17-4E60-4EE2-9E48-EF33D7CBB7E9")));	// TellDetect
		m_CompatiblePlugins.insert(UUID.FromString(_T("6FA271DC-DB0A-4B71-80D3-FE0B5DBF3BBF")));	// ExpWatch
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
		if (m_pContext != NULL)
		{
#ifdef _DEBUG
			switch(MessageType_in)
			{
				case CHAT_MESSAGE_TYPE_SYSTEM_MESSAGE:
				case CHAT_MESSAGE_TYPE_SAY_MESSAGE:
				case CHAT_MESSAGE_TYPE_SHOUT_MESSAGE:
				case CHAT_MESSAGE_TYPE_OUTGOING_TELL_MESSAGE:
				case CHAT_MESSAGE_TYPE_INCOMING_TELL_MESSAGE:
				case CHAT_MESSAGE_TYPE_PARTY_MESSAGE:
				case CHAT_MESSAGE_TYPE_LINKSHELL_MESSAGE:
				case CHAT_MESSAGE_TYPE_INVALID_MESSAGE:
				case CHAT_MESSAGE_TYPE_ECHO_MESSAGE:
				case CHAT_MESSAGE_TYPE_SANCTUARY:
				case CHAT_MESSAGE_TYPE_ERROR_MESSAGE:
				case CHAT_MESSAGE_TYPE_PARTY_INVITE:
				case CHAT_MESSAGE_TYPE_PARTY_JOIN:
				case CHAT_MESSAGE_TYPE_BATTLE_MESSAGE:
				case BATTLE_MESSAGE_TYPE_ABILITY:
				case BATTLE_MESSAGE_TYPE_ABILITY_RESULT:
				case BATTLE_MESSAGE_TYPE_STATUS_EFFECT:			
					// known message
				break;
				default: {}
			}
#endif // _DEBUG

			if (m_pContext->m_pTrampoline != NULL)
			{
				DWORD dwResult = 0UL, dwNewSize = 0UL, dwOriginalSize = pMessage_in_out->dwSize;
				const char *pOriginalMsg = pMessage_in_out->pResBuf;
				PluginFramework::PluginSet::const_iterator PluginIt;			
				char *pModifiedMsg = NULL;
				IGameChatPlugin *pPlugin;
				bool bResult = false;

				for (PluginIt = m_pContext->m_Subscribers.begin(); PluginIt != m_pContext->m_Subscribers.end(); ++PluginIt)
				{
					pPlugin = static_cast<IGameChatPlugin*>(*PluginIt);

					if (pPlugin != NULL)
					{
						pPlugin->OnChatMessage(MessageType_in, pSender_in_out, pMessage_in_out, pOriginalMsg,
											   dwOriginalSize, &pModifiedMsg, dwResult);

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
											 StringNode* pMessage_in_out, char *pModifiedMsg_in, DWORD NewSize_in,
											 bool AlwaysShow_in)
	{
		bool Result = false;

		//if (AlwaysShow_in)
		{
			DWORD dwOriginalSenderSize = pSender_in_out->dwSize;
			char *pOriginalSender = pSender_in_out->pResBuf;
			StringNode OriginalMsg;

			if (pModifiedMsg_in != NULL)
				OriginalMsg = UpdateNode(pModifiedMsg_in, NewSize_in, *pMessage_in_out);
			// display the error message instead of the typed command
			Result = ((fnFormatChatMessage)m_pContext->m_pTrampoline)(pThis_in_out, MessageType_in,
																	  pSender_in_out, pMessage_in_out, NULL);
			// restore the original message and sender
			pSender_in_out->dwSize = dwOriginalSenderSize;
			pSender_in_out->pResBuf = pOriginalSender;

			if (pModifiedMsg_in != NULL)
				*pMessage_in_out = OriginalMsg;
		}

		// cleanup
		if (pModifiedMsg_in != NULL)
			free(pModifiedMsg_in);

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
}