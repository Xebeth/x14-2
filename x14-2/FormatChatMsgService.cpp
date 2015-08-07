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

#include "WindowerCore.h"
#include "CmdLineCore.h"
#include "WindowerEngine.h"

extern Windower::WindowerEngine *g_pEngine;

namespace Windower
{
	FormatChatMsgService::CallingContext<FormatChatMsgService> FormatChatMsgService::m_Context;

	/*! \brief FormatChatMsgService constructor
		\param[in] Name_in : the name of the service
		\param[in] InvokePermission_in : flag specifying if the service can be invoked
	*/
	FormatChatMsgService::FormatChatMsgService(const string_t& Name_in, bool InvokePermission_in)
		: ModuleService(Name_in, InvokePermission_in), m_pChatMsg(NULL), m_pFormatChatMsgTrampoline(NULL)
	{
		// add compatible plugins
		StringUtils::UUID PluginUUID;

		m_CompatiblePlugins.insert(PluginUUID.FromString(_T("745E1230-0C81-4220-B099-3A3392EFA03A")));	// ChatLog
		m_CompatiblePlugins.insert(PluginUUID.FromString(_T("AF8B3EE1-B092-45C7-80AA-A2BF2213DA2B")));	// Timestamp
		m_CompatiblePlugins.insert(PluginUUID.FromString(_T("BC725A17-4E60-4EE2-9E48-EF33D7CBB7E9")));	// TellDetect
		m_CompatiblePlugins.insert(PluginUUID.FromString(_T("6FA271DC-DB0A-4B71-80D3-FE0B5DBF3BBF")));	// ExpWatch
		m_CompatiblePlugins.insert(PluginUUID.FromString(_T("80C85F2E-BC84-4D29-9BB6-FFE43892CC50")));	// AutoBlacklist

		m_Context.Set(this);
	}

	/*! \brief Formats a message received by the game chat log
		\param[in] pThis_in_out : a pointer to the class containing the hooked method
		\param[in] MessageType_in : the type of the message
		\param[in,out] pSender_in_out : the sender of the message
		\param[in,out] pMessage_in_out : the message
		\return true if the message was formatted successfully; false otherwise
	*/
	bool WINAPI FormatChatMsgService::FormatChatMessageHook(LPVOID pThis_in_out, CHAT_MESSAGE_TYPE MessageType_in,
															StringNode* pSender_in_out, StringNode* pMessage_in_out,
															const __time64_t *pTimestamp_in, bool Unknown1)
	{
		m_Context->m_pChatMsg = pThis_in_out;

		if (m_Context->m_pChatMsg != NULL && m_Context->m_pFormatChatMsgTrampoline != NULL)
		{
			PluginFramework::PluginSet::const_iterator PluginIt, EndIt = m_Context->m_Subscribers.cend();			
			DWORD_PTR dwResult = 0UL, dwOriginalSize = pMessage_in_out->dwSize, dwNewSize = dwOriginalSize;
			const char *pOriginalMsg = pMessage_in_out->pResBuf;
			DWORD MessageFlags = MSG_FLAG_NONE;
			char *pModifiedMsg = NULL;
			IGameChatPlugin *pPlugin;
			bool bResult = false;

			if (g_pEngine != NULL)
			{
				g_pEngine->OnChatMessage(MessageType_in, pSender_in_out->pResBuf, dwOriginalSize,
										 pOriginalMsg, &pModifiedMsg, dwNewSize, MessageFlags);
			}

			for (PluginIt = m_Context->m_Subscribers.cbegin(); PluginIt != EndIt; ++PluginIt)
			{
				pPlugin = static_cast<IGameChatPlugin*>(*PluginIt);

				if (pPlugin != NULL)
				{
					dwResult = pPlugin->OnChatMessage(MessageType_in, pSender_in_out->pResBuf, dwOriginalSize,
													  pOriginalMsg, &pModifiedMsg, dwNewSize, MessageFlags);

					// discard message
					if ((MessageFlags & MSG_FLAG_DISCARD) == MSG_FLAG_DISCARD)
					{
						if (pModifiedMsg != NULL)
							free(pModifiedMsg);

						return false;
					}
					// execute result
					if ((MessageFlags & MSG_FLAG_EXEC) == MSG_FLAG_EXEC && pModifiedMsg != NULL)
						CmdLineCore::InjectCommand(pModifiedMsg);
					// force the message to be an echo
					if ((MessageFlags & MSG_FLAG_FORCE_ECHO) == MSG_FLAG_FORCE_ECHO)
						MessageType_in = CHAT_MESSAGE_TYPE_ECHO_MESSAGE;

					if (pModifiedMsg != NULL && dwResult > dwNewSize)
						dwNewSize = dwResult;
				}
			}

			return m_Context->FormatMessage(pThis_in_out, MessageType_in, pSender_in_out,
											pMessage_in_out, pModifiedMsg, dwNewSize,
											pTimestamp_in, Unknown1);
		}

		return false;
	}

	bool FormatChatMsgService::FormatMessage(LPVOID pThis_in_out, CHAT_MESSAGE_TYPE MessageType_in, StringNode* pSender_in_out,
											 StringNode* pMessage_in_out, char *pModifiedMsg_in, DWORD_PTR NewSize_in,
											 const __time64_t *pTimestamp_in, bool Unknown1)
	{
		StringNode *pNode, ModifiedMsgNode;
		bool Result = false;
		 
		// update the message
		if (pModifiedMsg_in != NULL)
		{
			InitStringNode(ModifiedMsgNode, pModifiedMsg_in);
			pNode = &ModifiedMsgNode;
		}
		else
			pNode = pMessage_in_out;
		
		g_pEngine->LockMacroThread();
		// display the error message instead of the typed command
		Result = m_Context->m_pFormatChatMsgTrampoline(pThis_in_out, MessageType_in,
													   pSender_in_out, pNode,
													   pTimestamp_in, Unknown1);
		g_pEngine->UnlockMacroThread();
		// cleanup
		if (pModifiedMsg_in != NULL)
			free(pModifiedMsg_in);

		return Result;
	}

	bool FormatChatMsgService::InjectMessage(const std::string &Msg_in,
											 const std::string &Sender_in,
											 CHAT_MESSAGE_TYPE MessageType_in)
	{
		if (m_Context->m_pChatMsg != NULL && m_Context->m_pFormatChatMsgTrampoline != NULL)
		{

			if (Msg_in.empty() == false)
			{
				StringNode DummySender, DummyMsg;

				InitStringNode(DummyMsg, Msg_in.c_str());
				InitStringNode(DummySender, Sender_in.c_str());

				FormatChatMessageHook(m_Context->m_pChatMsg, MessageType_in, 
									  &DummySender, &DummyMsg, NULL, false);
			}

			return true;
		}

		return false;
	}

	void FormatChatMsgService::OnPointerChange(const std::string &HookName_in, LPVOID pPointer_in)
	{
		if (HookName_in.compare(FORMAT_CHAT_MESSAGE_HOOK) == 0)
		{
			m_Context->m_pFormatChatMsgTrampoline = (fnFormatChatMessage)pPointer_in;
		}
	}
}