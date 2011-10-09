/**************************************************************************
	created		:	2010-10-23
	filename	: 	GameChatCore.h
	author		:	xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Interaction with the game chat memory
**************************************************************************/
#ifndef __GAME_CHAT_CORE_H__
#define __GAME_CHAT_CORE_H__

namespace Windower
{
	class IGameChatPlugin;

	typedef HookEngineLib::IHookManager IHookManager;

	class GameChatCore : public WindowerCore
	{
	public:
		GameChatCore(WindowerEngine &Engine_in,
					 CommandParser &Parser_in,
					 CommandDispatcher &Dispatcher_in);
		~GameChatCore();

		// ICoreModule interface implementation
		void RegisterHooks(IHookManager *pHookManager);
		void OnHookInstall(IHookManager *pHookManager);

		//! \brief FormatChatMessage hook
		bool FormatChatMessageHook(LPVOID _this, USHORT MessageType, const GameChatTextObject* pSender, GameChatTextObject* pMessage);

	protected:
		bool DisplayWindowerVersion();

		void OnSubscribe(const string_t &ServiceName_in,
						 const PluginSet &Subscribers_in);
		void OnUnsubscribe(const string_t &ServiceName_in,
						   const PluginSet &Subscribers_in);
		bool FilterCommands(LPVOID _this, USHORT MessageType_in,
							const GameChatTextObject* pSender_in,
							GameChatTextObject* pMessage_in);

		fnFormatChatMessage	m_pFormatChatMessageTrampoline;
		//! FormatChatMessage address in the game process
		DWORD_PTR m_dwFormatChatMessageAddr;
		//! pointer to the start of the chat memory
		char			 **m_pChatHead;
		//! pointer to the previous start of the chat memory
		char			  *m_pPrevChatHead;
		//! pointer to the write position in the chat memory
		char			 **m_pChatTail;
		//! pointer to the previous write position in the chat memory
		char			  *m_pPrevChatTail;
		//! vector keeping track of the chat head positions
		std::vector<char*> m_ChatHeadVector;
	private:
		void UpdateChatData(const void *pChatObj_in);

		PluginSet					 m_ChatFormatSubscribers;
		const GameChatTextObject	*m_pLastSender;
		CommandDispatcher			&m_CommandDispatcher;
		CommandParser				&m_CommandParser;
		LPVOID						 m_pGameChatObject;
	};
}

#endif//__GAME_CHAT_CORE_H__