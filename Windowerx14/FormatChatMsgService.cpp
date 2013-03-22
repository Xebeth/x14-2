/**************************************************************************
	created		:	2013-03-12
	filename	: 	FormatChatMsgService.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <FormatChatMsgTypes.h>
#include <PluginFramework.h>
#include <NonCopyable.h>

#include "ModuleService.h"
#include "FormatChatMsgService.h"

#include "IGameChatPlugin.h"

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
	bool FormatChatMsgService::FormatChatMessageHook(LPVOID pThis_in_out, USHORT MessageType_in, StringNode* pSender_in_out,
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
				PluginSet::const_iterator PluginIt;			
				bool bResult = false, bUnsubscribe;
				char *pModifiedMsg = NULL;
				IGameChatPlugin *pPlugin;

				for (PluginIt = m_pContext->m_Subscribers.begin(); PluginIt != m_pContext->m_Subscribers.end(); ++PluginIt)
				{
					pPlugin = static_cast<IGameChatPlugin*>(*PluginIt);

					if (pPlugin != NULL)
					{
						dwResult = pPlugin->OnChatMessage(MessageType_in, pSender_in_out, pMessage_in_out, pOriginalMsg,
														  dwOriginalSize, &pModifiedMsg, bUnsubscribe);

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
			DWORD dwOriginalMgsSize = pMessage_in_out->dwSize;
			char *pOriginalSender = pSender_in_out->pResBuf;
			char *pOriginalMsg = pMessage_in_out->pResBuf;			

			if (pModifiedMsg_in != NULL)
			{
				pMessage_in_out->dwSize = NewSize_in;
				pMessage_in_out->pResBuf = pModifiedMsg_in;
			}
			// display the error message instead of the typed command
			Result = ((fnFormatChatMessage)m_pContext->m_pTrampoline)(pThis_in_out, MessageType_in,
																	  pSender_in_out, pMessage_in_out, NULL);
			// restore the original message and sender
			pSender_in_out->dwSize = dwOriginalSenderSize;
			pMessage_in_out->dwSize = dwOriginalMgsSize;
			pSender_in_out->pResBuf = pOriginalSender;
			pMessage_in_out->pResBuf = pOriginalMsg;			
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

#if 0
	/*! \brief Filters commands received by the chat log and processes them
		\param[in] pThis_in_out : a pointer to the class containing the hooked method
		\param[in] MessageType_in : the type of the message
		\param[in,out] pSender_in_out : the sender of the message
		\param[in,out] pMessage_in_out : the message
		\return true if the message was formatted successfully; false otherwise
	*/
	bool GameChatCore::FilterCommands(LPVOID pThis_in_out, USHORT MessageType_in,
									  StringNode* pSender_in_out, StringNode* pMessage_in_out)
	{
		if (pMessage_in_out != NULL && pMessage_in_out->pResBuf != NULL && MessageType_in == CHAT_MESSAGE_TYPE_ECHO_MESSAGE)
		{
			// the message starts with 2 forward slashes => expect a command
			if (strstr(pMessage_in_out->pResBuf, "//") == pMessage_in_out->pResBuf)
			{
				bool Result = false;

				if (pMessage_in_out->dwSize > 2)
				{
					char *pFeedbackMsg = NULL;
					WindowerCommand Command;
					DWORD dwNewSize = 0UL;
					int ParseResult;

					if ((ParseResult = m_CommandParser.ParseCommand(pMessage_in_out->pResBuf + 2, Command, &pFeedbackMsg, dwNewSize)) >= 0)
					{
						std::string Feedback;

						Result = (Command.Execute(Feedback) == DISPATCHER_RESULT_SUCCESS);

						if (Feedback.empty() == false)
						{
							pFeedbackMsg = _strdup(Feedback.c_str());
							dwNewSize = Feedback.length() + 1;
						}
					}
					
					Result = FormatMessage(pThis_in_out, MessageType_in, pSender_in_out,
										   pMessage_in_out, pFeedbackMsg, dwNewSize, true);
				}

				return Result;
			}
		}

		return false;
	}
	
	/*! \brief Executes the command specified by its ID
		\param[in] CmdID_in : the ID of the command to execute
		\param[in] Command_in : the command to execute
		\param[out] Feedback_out : the result of the execution
		\return true if the command was executed successfully; false otherwise
	*/
	bool GameChatCore::ExecuteCommand(INT_PTR CmdID_in, const WindowerCommand &Command_in, std::string &Feedback_out)
	{
		switch(CmdID_in)
		{
			case CMD_TOGGLE:
				m_Active = !m_Active;
			break;
			case CMD_START:
				m_Active = true;
			break;
			case CMD_STOP:
				m_Active = false;
			break;
			default:
				return false;
		}

		if (m_Active)
			Feedback_out = "The chat is now on.";
		else
			Feedback_out = "The chat is now off.";

		return true;
	}

	/*! \brief Registers the commands of the plugin with the command dispatcher
		\return true if all the commands were registered successfully; false otherwise
	*/
	bool GameChatCore::RegisterCommands()
	{
		WindowerCommand *pCmd;

		pCmd = new WindowerCommand(ENGINE_KEY, CMD_TOGGLE, "chat::toggle", "Toggles the chat on/off", this, false);

		if (m_CommandDispatcher.RegisterCommand(pCmd))
			m_Commands[CMD_TOGGLE] = pCmd;
		else
			delete pCmd;

		pCmd = new WindowerCommand(ENGINE_KEY, CMD_STOP, "chat::off", "Prevents the chat from displaying messages", this, false);
		
		if (m_CommandDispatcher.RegisterCommand(pCmd))
			m_Commands[CMD_STOP] = pCmd;
		else
			delete pCmd;

		pCmd = new WindowerCommand(ENGINE_KEY, CMD_START, "chat::on", "Resumes the chat displaying messages", this, false);
		
		if (m_CommandDispatcher.RegisterCommand(pCmd))
			m_Commands[CMD_START] = pCmd;
		else
			delete pCmd;

		return true;
	}

	/*! \brief Unregisters the commands of the plugin with the command dispatcher
		\return true if all the commands were unregistered successfully; false otherwise
	*/
	bool GameChatCore::UnregisterCommands()
	{
		m_CommandDispatcher.UnregisterCommand(ENGINE_KEY, "chat::toggle");
		m_CommandDispatcher.UnregisterCommand(ENGINE_KEY, "chat::off");
		m_CommandDispatcher.UnregisterCommand(ENGINE_KEY, "chat::on");

		HandlerCommands::const_iterator CmdIt, EndIt(m_Commands.end());

		for (CmdIt = m_Commands.begin(); CmdIt != EndIt; ++CmdIt)
			delete CmdIt->second;

		m_Commands.clear();

		return true;
	}
#endif