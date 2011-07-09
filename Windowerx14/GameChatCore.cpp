/**************************************************************************
	created		:	2010-10-23
	filename	: 	GameChatCore.cpp
	author		:	xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Interaction with the game chat memory
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <NonCopyable.h>
#include <HookEngine.h>
#include <queue>

#include "BaseEngine.h"
#include "PluginEngine.h"
#include "WindowerEngine.h"

#include "FormatChatMessageHook.h"

#include "IGameChatPlugin.h"

#include "ICoreModule.h"
#include "WindowerCore.h"
#include "GameChatCore.h"

#include "WindowerCommand.h"
#include "CommandDispatcher.h"
#include "CommandParser.h"

#include <SigScan.h>

namespace Windower
{
	/*! \brief default constructor */
	GameChatCore::GameChatCore(WindowerEngine &Engine_in, CommandParser &Parser_in, CommandDispatcher &Dispatcher_in) 
		: WindowerCore(Engine_in), m_CommandParser(Parser_in), m_CommandDispatcher(Dispatcher_in)
	{
		m_pFormatChatMessageTrampoline = NULL;
		m_pPrevChatHead = m_pPrevChatTail = NULL;
		m_pChatTail = m_pChatHead = NULL;

		RegisterService(_T("FormatChatMessage"), false);
		// add compatible plugins
		PluginFramework::PluginUUID UUID;

		m_CompatiblePlugins.insert(UUID.FromString(_T("745E1230-0C81-4220-B099-3A3392EFA03A")));	// Chat log
		m_CompatiblePlugins.insert(UUID.FromString(_T("AF8B3EE1-B092-45C7-80AA-A2BF2213DA2B")));	// Timestamp
		m_CompatiblePlugins.insert(UUID.FromString(_T("BC725A17-4E60-4EE2-9E48-EF33D7CBB7E9")));	// Tell detect
	}

	/*! \brief destructor */
	GameChatCore::~GameChatCore() {}

	/*! \brief Initializes the chat hook */
	void GameChatCore::RegisterHooks(IHookManager *pHookManager)
	{
		if (pHookManager != NULL)
		{
			SigScan::InitializeSigScan(GetCurrentProcessId(), SIGSCAN_GAME_PROCESSW);
			m_dwFormatChatMessageAddr = SigScan::Scan(FORMAT_CHAT_MESSAGE_OPCODES_SIGNATURE,
													+ FORMAT_CHAT_MESSAGE_OPCODES_SIGNATURE_OFFSET);
			// set m_pFormatChatMessageTrampoline with the address of the original function in case of failure
			m_pFormatChatMessageTrampoline = (fnFormatChatMessage)m_dwFormatChatMessageAddr;
			SigScan::TerminateSigScan();

			if (m_dwFormatChatMessageAddr != NULL)
			{
				pHookManager->RegisterHook("FormatChatMessage", SIGSCAN_GAME_PROCESSA, (LPVOID)m_dwFormatChatMessageAddr,
										   ::FormatChatMessageHook, FORMAT_CHAT_MESSAGE_OPCODES_HOOK_SIZE);
			}
		}
	}

	void GameChatCore::OnHookInstall(IHookManager *pHookManager)
	{
		if (pHookManager != NULL)
		{
			m_pFormatChatMessageTrampoline	= (fnFormatChatMessage)pHookManager->GetTrampolineFunc("FormatChatMessage");
		}
	}

	bool GameChatCore::FilterCommands(LPVOID _this, USHORT MessageType_in,
									  const GameChatTextObject* pSender_in,
									  GameChatTextObject* pMessage_in)
	{
		if (pMessage_in != NULL && pMessage_in->pResBuf != NULL && MessageType_in == CHAT_MESSAGE_TYPE_INVALID_MESSAGE)
		{
			// the message starts with 2 forward slashes => expect a command
			if (strstr(pMessage_in->pResBuf, "//") == pMessage_in->pResBuf)
			{
				if (pMessage_in->dwSize > 2)
				{
					char *pOriginalMsg = pMessage_in->pResBuf, *pFeedbackMsg = NULL;
					DWORD dwOriginalSize = pMessage_in->dwSize, dwNewSize = 0;
					WindowerCommand Command;
					std::string HelpMsg;
					int ParseResult;

					if ((ParseResult = m_CommandParser.ParseCommand(pMessage_in->pResBuf + 2, Command, &pFeedbackMsg, dwNewSize)) >= 0)
					{
						m_CommandDispatcher.Dispatch(Command);
					}
					else if (pFeedbackMsg != NULL)
					{

						pMessage_in->dwSize = dwNewSize;
						pMessage_in->pResBuf = pFeedbackMsg;
						// display the error message instead of the typed command
						m_pFormatChatMessageTrampoline(_this, MessageType_in, pSender_in, pMessage_in);
						// restore the original message
						pMessage_in->dwSize = dwOriginalSize;
						pMessage_in->pResBuf = pOriginalMsg;
						// cleanup
						free(pFeedbackMsg);
					}
				}

				return true;
			}
		}

		return false;
	}

	bool GameChatCore::FormatChatMessageHook(LPVOID _this, USHORT MessageType,
											 const GameChatTextObject* pSender,
											 GameChatTextObject* pMessage)
	{
		// update the chat head & tail
		UpdateChatData(_this);

		if (FilterCommands(_this, MessageType, pSender, pMessage) == false && m_pFormatChatMessageTrampoline != NULL)
		{
			char *pModifiedMsg = NULL, *pOriginalMsg = pMessage->pResBuf;
			DWORD dwOriginalSize = pMessage->dwSize;
			PluginSet::iterator Iter;
			bool bResult;
		
			for (Iter = m_ChatFormatSubscribers.begin(); Iter != m_ChatFormatSubscribers.end(); ++Iter)
			{
				if (*Iter != NULL)
					static_cast<IGameChatPlugin*>(*Iter)->FormatChatMessage(MessageType, pSender, pMessage,
																			pOriginalMsg, dwOriginalSize,
																			&pModifiedMsg);
			}
			// call the original function
			bResult = m_pFormatChatMessageTrampoline(_this, MessageType, pSender, pMessage);
			// bResult = m_pFormatChatMessageTrampoline(_this, MessageType, pSender, pMessage);
			// restore the original pointer and size of the message object
			pMessage->pResBuf = pOriginalMsg;
			pMessage->dwSize = dwOriginalSize;
			// cleanup
			if (pModifiedMsg != NULL)
				free(pModifiedMsg);
		
			return bResult;
		}

		return false;
	}

	void GameChatCore::UpdateChatData(const void *pChatObj_in)
	{
		// pointer to the start of the chat
		if (m_pChatHead != NULL)
			m_pPrevChatHead = *m_pChatHead;

		m_pChatHead = (char**)((char*)pChatObj_in + FORMAT_CHAT_HEAD_POINTER_OFFSET);
		// pointer to the end of the chat
		if (m_pChatTail != NULL)
			m_pPrevChatTail = *m_pChatTail;

		m_pChatTail = (char**)((char*)pChatObj_in + FORMAT_CHAT_TAIL_POINTER_OFFSET);

		if (m_pChatHead != NULL && m_pPrevChatHead != *m_pChatHead)
			m_ChatHeadVector.push_back(*m_pChatHead);
	}

	void GameChatCore::OnSubscribe(const string_t &ServiceName_in, const PluginSet &Subscribers_in)
	{
		if (ServiceName_in.compare(_T("FormatChatMessage")) == 0)
			m_ChatFormatSubscribers = Subscribers_in;
	}

	void GameChatCore::OnUnsubscribe(const string_t &ServiceName_in, const PluginSet &Subscribers_in)
	{
		OnSubscribe(ServiceName_in, Subscribers_in);
	}
}