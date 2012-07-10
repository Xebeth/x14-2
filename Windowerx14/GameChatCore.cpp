/**************************************************************************
	created		:	2010-10-23
	filename	: 	GameChatCore.cpp
	author		:	xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Interaction with the game chat memory
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <HookEngine.h>
#include "version.h"

#include <CommandHandler.h>
#include "WindowerSettings.h"
#include "WindowerEngine.h"

#include "FormatChatMessageHook.h"

#include "IGameChatPlugin.h"
#include "ICreateTextNodePlugin.h"

#include "ICoreModule.h"
#include "WindowerCore.h"
#include "GameChatCore.h"

#include "WindowerCommand.h"
#include "CommandDispatcher.h"
#include "CommandParser.h"

#include <SigScan.h>

namespace Windower
{
	/*! \brief GameChatCore constructor */
	GameChatCore::GameChatCore(WindowerEngine &Engine_in_out, CommandParser &Parser_in, CommandDispatcher &Dispatcher_in) 
		: WindowerCore(Engine_in_out), CommandHandler(ENGINE_KEY, "GameChatCore"), m_CommandParser(Parser_in),
		  m_CommandDispatcher(Dispatcher_in), m_pFormatChatMessageTrampoline(NULL), m_pFormatChatMessage(NULL),
		  m_pCreateTextNode(NULL), m_bCreateTextNodeSubEmpty(true), m_Active(true)
	{
		InitializeCriticalSection(&m_Lock);
		// create the services
		m_pFormatChatMessage = RegisterService(_T("OnChatMessage"), false);
		m_pCreateTextNode = RegisterService(_T("CreateTextNode"), false);
		// register the module
		m_Engine.RegisterModule(_T("GameChat"), this);
		// add compatible plugins
		PluginFramework::PluginUUID UUID;

		m_CompatiblePlugins.insert(UUID.FromString(_T("745E1230-0C81-4220-B099-3A3392EFA03A")));	// ChatLog
		m_CompatiblePlugins.insert(UUID.FromString(_T("AF8B3EE1-B092-45C7-80AA-A2BF2213DA2B")));	// Timestamp
		m_CompatiblePlugins.insert(UUID.FromString(_T("BC725A17-4E60-4EE2-9E48-EF33D7CBB7E9")));	// TellDetect
		m_CompatiblePlugins.insert(UUID.FromString(_T("6FA271DC-DB0A-4B71-80D3-FE0B5DBF3BBF")));	// ExpWatch
		m_CompatiblePlugins.insert(UUID.FromString(_T("932E0F5D-1D24-40B1-BA63-D729A6E42C90")));	// InjectVersion

		RegisterCommands();
	}

	
	//! \brief GameChatCore destructor
	GameChatCore::~GameChatCore()
	{
		DeleteCriticalSection(&m_Lock);
		UnregisterCommands();
	}

	/*! \brief Register the hooks for this module
		\param[in] HookManager_in : the hook manager
	*/
	void GameChatCore::RegisterHooks(IHookManager &HookManager_in)
	{
		SigScan::SigScan MemScan;
		DWORD_PTR dwFuncAddr;

		MemScan.Initialize(GetCurrentProcessId(), SIGSCAN_GAME_PROCESSW);

		dwFuncAddr = MemScan.Scan(FORMAT_CHAT_MESSAGE_OPCODES_SIGNATURE,
								  FORMAT_CHAT_MESSAGE_OPCODES_SIGNATURE_OFFSET);
		// set m_pFormatChatMessageTrampoline with the address of the original function in case of failure
		m_pFormatChatMessageTrampoline = (fnFormatChatMessage)dwFuncAddr;

		if (dwFuncAddr != NULL)
		{
			HookManager_in.RegisterHook("OnChatMessage", SIGSCAN_GAME_PROCESSA, (LPVOID)dwFuncAddr,
										::FormatChatMessageHook, FORMAT_CHAT_MESSAGE_OPCODES_HOOK_SIZE);
		}

		dwFuncAddr = MemScan.Scan(CREATETEXTNODE_OPCODES_SIGNATURE,
								  CREATETEXTNODE_OPCODES_SIGNATURE_OFFSET);
		// set m_pCreateTextNodeTrampoline with the address of the original function in case of failure
		m_pCreateTextNodeTrampoline = (fnCreateTextNode)dwFuncAddr;

		if (dwFuncAddr != NULL)
		{
			HookManager_in.RegisterHook("CreateTextNode", SIGSCAN_GAME_PROCESSA, (LPVOID)dwFuncAddr,
										::CreateTextNodeHook, CREATETEXTNODE_OPCODES_HOOK_SIZE);
		}
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void GameChatCore::OnHookInstall(IHookManager &HookManager_in)
	{
		m_pFormatChatMessageTrampoline	= (fnFormatChatMessage)HookManager_in.GetTrampolineFunc("OnChatMessage");
		m_pCreateTextNodeTrampoline = (fnCreateTextNode)HookManager_in.GetTrampolineFunc("CreateTextNode");
	}

	/*! \brief Callback function invoked when a plugin subscribes to a service
		\param[in] ServiceName_in : the name of the service
		\param[in] Subscribers_in : the service subscribers
	*/
	void GameChatCore::OnSubscribe(const string_t &ServiceName_in, const PluginSet &Subscribers_in)
	{
		EnterCriticalSection(&m_Lock);

		if (ServiceName_in.compare(_T("OnChatMessage")) == 0)
			m_ChatFormatSubscribers = Subscribers_in;
		if (ServiceName_in.compare(_T("CreateTextNode")) == 0)
		{
			m_bCreateTextNodeSubEmpty = Subscribers_in.empty();
			m_CreateTextNodeSubscribers = Subscribers_in;
		}

		LeaveCriticalSection(&m_Lock);
	}

	/*! \brief Callback function invoked when a plugin subscription to a service is revoked
		\param[in] ServiceName_in : the name of the service
		\param[in] Subscribers_in : the service subscribers
	*/
	void GameChatCore::OnUnsubscribe(const string_t &ServiceName_in, const PluginSet &Subscribers_in)
	{
		OnSubscribe(ServiceName_in, Subscribers_in);
	}

	/*! \brief Filters commands received by the chat log and processes them
		\param[in] pThis_in_out : a pointer to the class containing the hooked method
		\param[in] MessageType_in : the type of the message
		\param[in] pSender_in : the sender of the message
		\param[in,out] pMessage_in_out : the message
		\return true if the message was formatted successfully; false otherwise
	*/
	bool GameChatCore::FilterCommands(LPVOID pThis_in_out, USHORT MessageType_in,
									  const StringNode* pSender_in, StringNode* pMessage_in_out)
	{
		if (pMessage_in_out != NULL && pMessage_in_out->pResBuf != NULL && MessageType_in == CHAT_MESSAGE_TYPE_INVALID_MESSAGE)
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
					
					Result = FormatMessage(pThis_in_out, MessageType_in, pSender_in,
										   pMessage_in_out, pFeedbackMsg, dwNewSize, true);
				}

				return Result;
			}
		}

		return false;
	}

	bool GameChatCore::FormatMessage(LPVOID pThis_in_out, USHORT MessageType_in, const StringNode* pSender_in,
									 StringNode* pMessage_in_out, char *pModifiedMsg_in, DWORD NewSize_in,
									 bool AlwaysShow_in)
	{
		bool Result = false;

		if (m_Active || AlwaysShow_in)
		{
			DWORD dwOriginalSize = pMessage_in_out->dwSize;
			char *pOriginalMsg = pMessage_in_out->pResBuf;		

			if (pModifiedMsg_in != NULL)
			{
				pMessage_in_out->dwSize = NewSize_in;
				pMessage_in_out->pResBuf = pModifiedMsg_in;
			}
			// display the error message instead of the typed command
			Result = m_pFormatChatMessageTrampoline(pThis_in_out, MessageType_in, pSender_in, pMessage_in_out);
			// restore the original message
			pMessage_in_out->dwSize = dwOriginalSize;
			pMessage_in_out->pResBuf = pOriginalMsg;
		}

		// cleanup
		if (pModifiedMsg_in != NULL)
			free(pModifiedMsg_in);

		return Result;
	}

	/*! \brief Formats a message received by the game chat log
		\param[in] pThis_in_out : a pointer to the class containing the hooked method
		\param[in] MessageType_in : the type of the message
		\param[in] pSender_in : the sender of the message
		\param[in,out] pMessage_in_out : the message
		\return true if the message was formatted successfully; false otherwise
	*/
	bool GameChatCore::FormatChatMessageHook(LPVOID pThis_in_out, USHORT MessageType_in,
											 const StringNode* pSender_in,
											 StringNode* pMessage_in_out)
	{
		if (FilterCommands(pThis_in_out, MessageType_in, pSender_in, pMessage_in_out) == false && m_pFormatChatMessageTrampoline != NULL)
		{
			DWORD dwResult = 0UL, dwNewSize = 0UL, dwOriginalSize = pMessage_in_out->dwSize;
			const char *pOriginalMsg = pMessage_in_out->pResBuf;
			PluginSet::const_iterator PluginIt;			
			bool bResult = false, bUnsubscribe;
			char *pModifiedMsg = NULL;
			IGameChatPlugin *pPlugin;

			for (PluginIt = m_ChatFormatSubscribers.begin(); PluginIt != m_ChatFormatSubscribers.end(); ++PluginIt)
			{
				pPlugin = static_cast<IGameChatPlugin*>(*PluginIt);

				if (pPlugin != NULL)
				{
					dwResult = pPlugin->OnChatMessage(MessageType_in, pSender_in, pMessage_in_out, pOriginalMsg,
													  dwOriginalSize, &pModifiedMsg, bUnsubscribe);

					if (pModifiedMsg != NULL && dwResult > dwNewSize)
						dwNewSize = dwResult;
				}
			}

			return FormatMessage(pThis_in_out, MessageType_in, pSender_in,
								 pMessage_in_out, pModifiedMsg, dwNewSize);
		}

		return false;
	}

	/*! \brief Creates a text node from the specified string
		\param[out] pTextObject_out : a pointer to the class containing the hooked method
		\param[in] pText_in : the text used to initialize the node
		\param[in] TextLength_in : the length of the text
		\return a pointer to the text node
	*/
	StringNode* GameChatCore::CreateTextNodeHook(StringNode *pTextObject_out, const char *pText_in, int TextLength_in)
	{
		if (m_bCreateTextNodeSubEmpty == false)
		{
			EnterCriticalSection(&m_Lock);

			PluginSet::iterator Iter = m_CreateTextNodeSubscribers.begin();
			ICreateTextNodePlugin* pPlugin;
			bool bUnsubscribe;

			for (; Iter != m_CreateTextNodeSubscribers.end(); ++Iter)
			{
				pPlugin = static_cast<ICreateTextNodePlugin*>(*Iter);

				if (pPlugin != NULL)
				{
					pText_in = pPlugin->OnCreateTextNode(pText_in, bUnsubscribe);

					if (bUnsubscribe)
					{
						Iter = m_CreateTextNodeSubscribers.erase(Iter);

						// stop looping if the set becomes empty
						if (Iter == m_CreateTextNodeSubscribers.end())
						{
							m_bCreateTextNodeSubEmpty = true;
							break;
						}
					}
				}
			}

			LeaveCriticalSection(&m_Lock);
		}

		return m_pCreateTextNodeTrampoline(pTextObject_out, pText_in, TextLength_in);
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

		pCmd = new WindowerCommand(ENGINE_KEY, CMD_STOP, "chat::off", "Prevents the chat from displaying messages", this, false);
		
		if (m_CommandDispatcher.RegisterCommand(pCmd))
			m_Commands[CMD_STOP] = pCmd;

		pCmd = new WindowerCommand(ENGINE_KEY, CMD_START, "chat::on", "Resumes the chat displaying messages", this, false);
		
		if (m_CommandDispatcher.RegisterCommand(pCmd))
			m_Commands[CMD_START] = pCmd;

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
		m_Commands.clear();

		return true;
	}
}